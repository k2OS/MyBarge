/********************************************************************************
*	RC boat controller using nrF24L01
* heavily inspired by https://github.com/shane-taurean/Pepsi_Bottle_RC_Boat											
* Initial one-arduino setup to test and add functionality in control of thrust with PS2 Joystick
*
* TODO/Ideas:
* - add trim on remote controller side to 1) allow for slight drift of the pot-values read from the joystick and 2) to conpensate for differences in the motors/drag of the boat. Might not be necessary at all.
* - add one or more buttons to the remote controlm, in addition to the button the joystick for various functions
/*******************************************************************************/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
RF24 radio(9,10);
// radio addresses
uint8_t addresses[][13] = {"MyBargeComs1","MyBargeComs2"}; // coms1 = remote, coms2 = barge

int x_val,y_val = 0; // values read from the joystick controller
                     // these are to be sent to the receiver

// for now we are only sending over 2 bytes (x_val and y_val) - we should send multiple bytes with different commands
// attached to them - ie. winchUP/DOWN, lockHOME, goHOME etc. 
uint8_t commandstring[8];
int 

unsigned long time = 0;


// joystick pins
int potPin_x = 0; //potentiometer to control the speed of motor 1
int potPin_y = 1; //potentiometer to control the speed of motor 2
int joyButton = A2;

//**************************************************
void setup() 
{
	Serial.begin( 57600 );
        printf_begin();
	pinMode(potPin_x, INPUT); 
	pinMode(potPin_y, INPUT); 

        // init radio
          radio.begin();                           // Setup and configure rf radio
      //    radio.setChannel(1); // default is 76.. should be good
          radio.setPALevel(RF24_PA_MAX);
          radio.setDataRate(RF24_1MBPS);
          radio.setAutoAck(1);                     // Ensure autoACK is enabled
          radio.setRetries(2,15);                   // Optionally, increase the delay between retries & # of retries
          radio.setCRCLength(RF24_CRC_8); 
          radio.openWritingPipe(addresses[0]);          // must be reversed for the barge receiver
//          radio.openReadingPipe(1,addresses[1]);
      
//          radio.startListening();                 // Start listening
          radio.printDetails();                   // Dump the configuration of the rf unit for debugging

	delay( 50 );

	Serial.println( "***** remote control ready *****" );
}

//**************************************************
void loop() {
  x_val = analogRead(potPin_x)/4;
  y_val = analogRead(potPin_y)/4;

  commandstring[0] = x_val;
  commandstring[1] = y_val;
  
  Serial.print("X: "); Serial.print(x_val); Serial.print(" Y: "); Serial.println(y_val);

  bool ok = radio.write( commandstring,8 );
  if (!ok)
    printf("failed\n\r");
  // Try again in a short while
  delay(20);
}


//**************************************************




