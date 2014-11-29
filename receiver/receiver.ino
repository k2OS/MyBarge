/********************************************************************************
*	RC boat controller using nrF24L01
* heavily inspired by https://github.com/shane-taurean/Pepsi_Bottle_RC_Boat											
* Initial one-arduino setup to test and add functionality in control of thrust with PS2 Joystick
*
* TODO/Ideas:
* - add trim on remote controller side to 1) allow for slight drift of the pot-values read from the joystick and 2) to conpensate for differences in the motors/drag of the boat. Might not be necessary at all.
* - add one or more buttons to the remote controlm, in addition to the button the joystick for various functions
* - auto-kill motors if no command-strings received for 5 seconds
/*******************************************************************************/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// CE,CSN
RF24 radio(9,10);
// radio addresses
uint8_t addresses[][13] = {"MyBargeComs1","MyBargeComs2"}; // coms1 = remote, coms2 = barge

// motor pins
int EN_A  = 5;      // to 1,2EN of L293D
int IN_A1 = 2;      // to 1A of L293D
int IN_A2 = 3;      // to 2A of L293D

int EN_B  = 6;      // to 1,2EN of L293D
int IN_B1 = 4;      // to 1A of L293D
int IN_B2 = 7;      // to 2A of L293D


boolean dirx,diry = 1;  // 1 (true) indicates forward, 0 indicates reverse

int x_val,y_val = 0; // values read from the joystick controller
                     // these are to be sent from the remote controller

int left_speed,right_speed = 0;


// for now we are only send 8 bytes
// 0 = x_val
// 1 = y_val
uint8_t commandstring[8];

unsigned long time = 0;



//**************************************************
void setup() 
{
	Serial.begin( 57600 );
	init_motors();
        // init radio
          radio.begin();                           // Setup and configure rf radio
      //    radio.setChannel(1); // default is 76.. should be good
          radio.setPALevel(RF24_PA_MAX);
          radio.setDataRate(RF24_1MBPS);
          radio.setAutoAck(1);                     // Ensure autoACK is enabled
          radio.setRetries(2,15);                   // Optionally, increase the delay between retries & # of retries
          radio.setCRCLength(RF24_CRC_8); 
//          radio.openWritingPipe(addresses[1]);          // must be reversed for the barge remote control
          radio.openReadingPipe(1,addresses[0]);
      
          radio.startListening();                 // Start listening
          radio.printDetails();                   // Dump the configuration of the rf unit for debugging

        delay( 50 );

	Serial.println( "***** receiver ready *****" );
}

//**************************************************
void loop() {
/*  x_val = analogRead(potPin_x)/4;
  y_val = analogRead(potPin_y)/4;
*/
// read from radio, do stuff below - if timout reached, kill motors

    if ( radio.available() ) {
      // Dump the payloads until we've gotten everything
      while (radio.available()) {
        // Fetch the payload, and see if this was the last one.
        radio.read( commandstring, 8 );
        x_val=commandstring[0];       
        y_val=commandstring[1];
        Serial.print("X: "); Serial.print(x_val); Serial.print(" Y: "); Serial.println(y_val);
        // forwards or backwards
        if ( y_val < 130 || y_val > 140 ) {
  	  if ( y_val < 130 ) {
	  y_val = map( y_val, 130, 0, 0, 255 );
  		dirx = diry = 0;
	  } else if ( y_val > 140 ) {
	  	y_val = map( y_val, 140, 255, 0, 255 );
  		dirx = diry = 1;
	  }
	  left_speed = y_val;
	  right_speed = y_val;
	  if ( x_val < 120 || x_val > 130 ) {
	  	if ( x_val < 120 ) {	// turn left
	  		x_val = map( x_val, 120, 0, 0, y_val );
	  		left_speed -= x_val;
	  	} else if ( x_val > 130 ) {	// turn right
	  		x_val = map( x_val, 130, 255, 0, y_val );
	  		right_speed -= x_val;
	  	}
	    }
         } else if ( (x_val < 120 || x_val > 130) && (y_val >= 128 && y_val <= 132) ) {
    	    if ( x_val < 120 ) {	// turn left
	  	x_val = map( x_val, 120, 0, 0, 255l );
		dirx = 1;
		left_speed = x_val;
                diry = 0;
                right_speed = x_val;
	    } else if ( x_val > 130 ) {	// turn right
	  	x_val = map( x_val, 130, 255, 0, 255 );
                dirx = 0;
		left_speed = x_val;
                diry = 1;
                right_speed = x_val;
	    }
         } else {
	   left_speed = 0;
	   right_speed = 0;
         }
         set_left_motor(left_speed, dirx);
         set_right_motor(right_speed, diry);
       
      } // while radio..
    } // if radio..
} // loop


//**************************************************

void init_motors() {
  // set output modes
  pinMode(IN_A1, OUTPUT);
  pinMode(IN_A2, OUTPUT);
  pinMode(EN_A, OUTPUT);
  pinMode(IN_B1, OUTPUT);
  pinMode(IN_B2, OUTPUT);
  pinMode(EN_B, OUTPUT);

  
  // initialize ports to safely turn off the motors
  stop_motors();
}

void stop_motors(){
  set_left_motor( 0, dirx);
  set_right_motor( 0, diry);

}

void set_left_motor(int speed, boolean dir) {
  speed = constrain(speed, 0, 255);
  analogWrite(EN_A, speed);     // PWM on enable lines
  digitalWrite(IN_A1, dir);
  digitalWrite(IN_A2, ! dir);
}

void set_right_motor(int speed, boolean dir) {
  speed = constrain(speed, 0, 255);
  analogWrite(EN_B, speed);     // PWM on enable lines
  digitalWrite(IN_B1, dir);
  digitalWrite(IN_B2, ! dir);
}

