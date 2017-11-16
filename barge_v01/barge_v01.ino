/********************************************************************************
*	RC boat controller using nrF24L01
* heavily inspired by https://github.com/shane-taurean/Pepsi_Bottle_RC_Boat											
* Initial one-arduino setup to test and add functionality in control of thrust with PS2 Joystick
*
* TODO/Ideas:
* - add trim on remote controller side to 1) allow for slight drift of the pot-values read from the joystick and 2) to compensate for differences in the motors/drag of the boat. Might not be necessary at all.
* - add one or more buttons to the remote controlm, in addition to the button the joystick for various functions like winch up/down, lights on/off, go-home, lock position etc.

* Prepare to change to another motor controller as I suspect the L293 won't handle slightly larger motors and amp-draws
* - L298 is used instead - looks like a drop-in replacement, but hasn't been tested yet
* See this guy for inspirating for cleaner/more readable code: https://www.youtube.com/watch?v=eVnyPSAefxU
* DONE:
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

// motor A  - left
int enA = 3;

// motor B - right
int enB = 6; 


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
	//Serial.begin( 9600 );
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
  
  // Motor A
  digitalWrite(enA, LOW);
  // Motor B
  digitalWrite(enB, LOW);
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
          motorSpeedA = data.throttle;
          motorSpeedB = data.throttle;
        } else if (data.throttle < -5) { // going reverse - can't do that yet, when we don't have any reverse thrusters on..
          // Set Motor A backwards

           // no reverse thrusters yet, so we just set speed to 0
          motorSpeedA = data.throttle*-1;
          motorSpeedB = data.throttle*-1;

          motorSpeedA = 0;
          motorSpeedB = 0;

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
        // Set the motor speeds
        analogWrite(enA, motorSpeedA);
        analogWrite(enB, motorSpeedB);
//        Serial.print(data.throttle); Serial.print(" / Speed A : "); Serial.print(motorSpeedA); Serial.print(" / Speed B : "); Serial.println(motorSpeedB);

} // loop


//**************************************************


