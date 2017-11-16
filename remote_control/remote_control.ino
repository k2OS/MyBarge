/********************************************************************************
*	RC boat controller using nrF24L01
* heavily inspired by https://github.com/shane-taurean/Pepsi_Bottle_RC_Boat											
* Initial one-arduino setup to test and add functionality in control of thrust with PS2 Joystick
*
* TODO/Ideas:
* - add trim on remote controller side to 1) allow for slight drift of the pot-values read from the joystick and 2) to conpensate for differences in the motors/drag of the boat. Might not be necessary at all.
* - add one or more buttons to the remote control, in addition to the button the joystick for various functions
* * See this guy for inspirating for cleaner/more readable code: https://www.youtube.com/watch?v=eVnyPSAefxU
 */
/*******************************************************************************/

#include <Expo.h>
#include <SPI.h>
//#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
// CE, CS
RF24 radio(9,10);
// radio addresses
uint8_t addresses[][13] = {"MyBargeComs1","MyBargeComs2"}; // coms1 = remote, coms2 = barge

rc::Expo g_expo;


int x_val,y_val = 0; // values read from the joystick controller
                     // these are to be sent to the receiver

struct myData {
  int throttle; // joystick x direction
  int direction; // joystick y direction
  int aux1; // joystick button
  int aux2; // extra button
};
myData data;

unsigned long time = 0;


// joystick pins
int potPin_x = 0; 
int potPin_y = 1; 
int joyButton = A2;
int centerX,centerY; // for adjusting center on my crappy joystick(s)
float xThrow = 1; // fraction of 1 to adjust throw
float yThrow = 0.5; // fraction of 1 to adjust throw

//**************************************************
void setup() 
{
//	 Serial.begin(115200);
  
  g_expo = 20;
  
	pinMode(potPin_x, INPUT); 
	pinMode(potPin_y, INPUT); 

        // init radio
          radio.begin();                           // Setup and configure rf radio
//          radio.setPALevel(RF24_PA_MAX);
          radio.setPALevel(RF24_PA_LOW);
          radio.setDataRate(RF24_250KBPS);
          radio.setAutoAck(false);                     // Ensure autoACK is disabled
          radio.setCRCLength(RF24_CRC_8); 
          radio.openWritingPipe(addresses[1]);          // must be reversed for the barge receiver
//          radio.printDetails();                   // Dump the configuration of the rf unit for debugging

	
  // my joysticks are way offcenter, so a little adjusting.. taking from a very confusing instructables, but I made it work.. these initial values are used after analogRead in loop()
  centerX = analogRead(potPin_x);
  centerY = analogRead(potPin_y);
  delay( 50 );
  centerX = analogRead(potPin_x);
  centerY = analogRead(potPin_y);

//	Serial.println( "***** remote control ready *****" );
}

//**************************************************
void loop() {
  data.throttle = analogRead(potPin_x);
  if(data.throttle < centerX + 1){ 
    data.throttle = map(data.throttle, 0, centerX - 1, -255*xThrow, 0);
  } else if (data.throttle >= centerX){ 
    data.throttle = map(data.throttle, centerX, 1023, 0, 255*xThrow);
  }

  data.direction = analogRead(potPin_y);
  if(data.direction < centerY + 1){ 
    data.direction = map(data.direction, 0, centerY - 1, -255*yThrow, 0);
  } else if (data.direction >= centerY){ 
    data.direction = map(data.direction, centerY, 1023, 0, 255*yThrow);
  }
  data.direction = g_expo.apply(data.direction);

  //Serial.print("Throttle: "); Serial.print("(");Serial.print(centerX); Serial.print(") "); Serial.print(data.throttle); Serial.print(" / Direction: "); Serial.println(data.direction);
//  delay(1000);
  bool ok = radio.write( &data,sizeof(myData) );

  // aucto-ack has been disabled, write() will always return true
//  delay(50);

}

//**************************************************




