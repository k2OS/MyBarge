/********************************************************************************
*	RC boat controller using nrF24L01
* heavily inspired by https://github.com/shane-taurean/Pepsi_Bottle_RC_Boat											
* Initial one-arduino setup to test and add functionality in control of thrust with PS2 Joystick
*
* TODO/Ideas:
* - add trim on remote controller side to 1) allow for slight drift of the pot-values read from the joystick and 2) to compensate for differences in the motors/drag of the boat. Might not be necessary at all.
* - add one or more buttons to the remote controlm, in addition to the button the joystick for various functions like winch up/down, lights on/off, go-home, lock position etc.
* 
* * * See this guy for inspirating for cleaner/more readable code: https://www.youtube.com/watch?v=eVnyPSAefxU
* DONE:
* - changed to L298N
*  - auto-kill all motors if no command-strings received for 1 second
*/


/*******************************************************************************/

#include <SPI.h>
#include "RF24.h"

// CE,CSN
RF24 radio(9,10);
// radio addresses
uint8_t addresses[][13] = {"MyBargeComs1","MyBargeComs2"}; // coms1 = remote, coms2 = barge

boolean dirA,dirB = 1;  // 1 (true) indicates forward, 0 indicates reverse

// throttle = forwards or backwards (256= full speed ahead - -256 == reverse)
// direction = turn - 0 = turn/rotate left, 255 = turn/rotate right
int x_val,y_val = 0; // values read from the joystick controller
                     // these are to be sent from the remote controller
int motorSpeedA,motorSpeedB = 0;
int maxSpeed = 255;


// ramp-stuff
unsigned long int prevRampTimeA = 0;
unsigned long int prevRampTimeB = 0;
unsigned long int rampdelay = 50; // ms mellem hver pwm-skifte
int speedstep = 5;
int targetspeedA = 0; // to be read from radio/joystick
int targetspeedB = 0; // to be read from radio/joystick
int rampDirA = 1; // 1 == up, -1 == down
int rampDirB = 1; // 1 == up, -1 == down
int currentspeedA = 0; // what is our current speed (to be written to PWM)
int currentspeedB = 0; // what is our current speed (to be written to PWM)



// motor B (3,4,5)
int enB = 3; 
int in1 = 5;
int in2 = 4;

// motor A (6,7,8)
int enA = 6;
int in3 = 7;
int in4 = 8;

struct myData {
  int throttle; // joystick x direction
  int direction; // joystick y direction - move this to a 2nd joystick when I get around to building it
  int aux1; // joystick button
  int aux2; // extra button
};
myData data;


unsigned long time = 0;
unsigned long lastMsg = 0;

unsigned long RXtout = 1000; // how long will we wait for new commands before total shutdown?
                             // not implemented yet

//**************************************************
void setup() 
{
//	Serial.begin( 115200 );
        // init radio
          radio.begin();                           // Setup and configure rf radio
          radio.setAutoAck(false);                     // Ensure autoACK is disabled
//          radio.setPALevel(RF24_PA_LOW);
          radio.setDataRate(RF24_250KBPS);

          radio.setCRCLength(RF24_CRC_8); 
          radio.openReadingPipe(1,addresses[1]);
      
          radio.startListening();                 // Start listening
 //         radio.printDetails();                   // Dump the configuration of the rf unit for debugging
 
data.throttle = 0;
data.direction = 0;

  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // Start with motors disabled and direction forward
  
  // Motor A
  digitalWrite(enA, LOW);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // Motor B
  digitalWrite(enB, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

//**************************************************
void loop() {
        if( radio.available()){
          while (radio.available()) {                                   // While there is data ready
            radio.read( &data, sizeof(myData) );             // Get the payload
            lastMsg = millis();
          }
    //      Serial.print("Throttle: "); Serial.print(data.throttle); Serial.print("Direction: ");Serial.println(data.direction);
        }
        if (millis() - lastMsg > RXtout) {
          data.throttle = 0;
          data.direction = 0;
        }
 
        // going forward:
        if (data.throttle > 5) {
          // Set Motor A forward
          digitalWrite(in1, HIGH);
          digitalWrite(in2, LOW);
          // Set Motor B forward
          digitalWrite(in3, HIGH);
          digitalWrite(in4, LOW);
 
//          motorSpeedA = map(data.throttle,0,256,0,maxSpeed);
//          motorSpeedB = map(data.throttle,0,256,0,maxSpeed);
          motorSpeedA = data.throttle;
          motorSpeedB = data.throttle;
        } else if (data.throttle < -5) { // going reverse
          // Set Motor A backwards
          digitalWrite(in1, LOW);
          digitalWrite(in2, HIGH);
          // Set Motor B backwards
          digitalWrite(in3, LOW);
          digitalWrite(in4, HIGH);
 
          motorSpeedA = data.throttle*-1;
          motorSpeedB = data.throttle*-1;
        
        } else {
          motorSpeedA = 0;
          motorSpeedB = 0;
        }

        // Steering - left
        if (data.direction < -5) {
          y_val = data.direction*-1;
          motorSpeedA = motorSpeedA-y_val;
  //        motorSpeedB = motorSpeedB+y_val;

          if (motorSpeedA < 0) { motorSpeedA = 0; }
          if (motorSpeedB > maxSpeed) { motorSpeedB = maxSpeed; }
        } else if (data.direction > 5) { // steering right
          y_val = data.direction;
//          motorSpeedA = motorSpeedA+y_val;
          motorSpeedB = motorSpeedB-y_val;

          if (motorSpeedA > maxSpeed) { motorSpeedA = maxSpeed; }
          if (motorSpeedB < 0) { motorSpeedB = 0; }
          
        }
        if (motorSpeedA < 10) { motorSpeedA = 0; }
        if (motorSpeedB < 10) { motorSpeedB = 0; }
        //delay(10);

  targetspeedA = motorSpeedA;
  targetspeedB = motorSpeedB;
  if (currentspeedA != targetspeedA) { // we have not reached the targetspeed yet
      if ( millis()-rampdelay > prevRampTimeA) { // only change speed every $rampdelay
        int diff = targetspeedA-currentspeedA; // negative number == go slower / positive == go faster
        rampDirA = diff/abs(diff); // returns -1 if we are above target and 1 if we are below
        currentspeedA += speedstep*rampDirA;
        if (currentspeedA > targetspeedA && rampDirA == 1) { currentspeedA = targetspeedA; }
        if (currentspeedA < targetspeedA && rampDirA == -1) { currentspeedA = targetspeedA; }
        prevRampTimeA = millis();
      }
  }
  if (currentspeedB != targetspeedB) { // we have not reached the targetspeed yet
      if ( millis()-rampdelay > prevRampTimeB) { // only change speed every $rampdelay
        int diff = targetspeedB-currentspeedB; // negative number == go slower / positive == go faster
        rampDirB = diff/abs(diff); // returns -1 if we are above target and 1 if we are below
        currentspeedB += speedstep*rampDirB;
        if (currentspeedB > targetspeedB && rampDirB == 1) { currentspeedB = targetspeedB; }
        if (currentspeedB < targetspeedB && rampDirB == -1) { currentspeedB = targetspeedB; }
        prevRampTimeB = millis();
      }
  }
        
        // Set the motor speeds
        analogWrite(enA, motorSpeedA);
        analogWrite(enB, motorSpeedB);
        // Set the motor speeds - with ramps
//        analogWrite(enA, currentspeedA);
//        analogWrite(enB, currentspeedB);

} // loop


//**************************************************


