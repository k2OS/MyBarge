/********************************************************************************
*	RC boat controller using nrF24L01
* heavily inspired by https://github.com/shane-taurean/Pepsi_Bottle_RC_Boat											
* Initial one-arduino setup to test and add functionality in control of thrust with PS2 Joystick
*
* TODO/Ideas:
* - add trim on remote controller side to 1) allow for slight drift of the pot-values read from the joystick and 2) to conpensate for differences in the motors/drag of the boat. Might not be necessary at all.
* - add one or more buttons to the remote controlm, in addition to the button the joystick for various functions
/*******************************************************************************/

int EN_A  = 5;      // to 1,2EN of L293D

int IN_A1 = 2;      // to 1A of L293D
int IN_A2 = 3;      // to 2A of L293D

int EN_B  = 6;      // to 1,2EN of L293D

int IN_B1 = 4;      // to 1A of L293D
int IN_B2 = 10;      // to 2A of L293D


int speed = 100;
boolean dirx,diry = 1;  // 1 (true) indicates forward, 0 indicates reverse

int x_val,y_val = 0;

int left_speed,right_speed = 0;

unsigned long time = 0;


int potPin_x = 1; //potentiometer to control the speed of motor 1
int potPin_y = 0; //potentiometer to control the speed of motor 2


//**************************************************
void setup() 
{
	Serial.begin( 9600 );
	pinMode(potPin_x, INPUT); 
	pinMode(potPin_y, INPUT); 
	init_motors();

	delay( 50 );

	Serial.println( "******************" );
}

//**************************************************
void loop() {
  x_val = analogRead(potPin_x)/4;
  y_val = analogRead(potPin_y)/4;

  Serial.print("X: "); Serial.print(x_val); Serial.print(" Y: "); Serial.println(y_val);
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

}


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

