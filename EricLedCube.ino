/*
  EricLedCube
 */

#define ACTIVE_AL LOW
#define INACTIVE_AL HIGH
#define ACTIVE_AH HIGH
#define INACTIVE_AH LOW

#define ALWAYS_ON false

#define NB_MAX_EFFET 5

// Pin 13 has an LED connected on most Arduino boards.
int led = 13;
int clock = 2;
int shin = 3;

int sr_ser = 5;
int sr_oe = 7;
int sr_rclk = 8;
int sr_srclk = 10;

int wait = 100;

boolean isInit = false;
byte data = 0x00;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  pinMode(clock, OUTPUT);     
  pinMode(shin, OUTPUT);     
  
  pinMode(sr_oe, OUTPUT);
  pinMode(sr_rclk, OUTPUT);
  pinMode(sr_ser, OUTPUT);
  pinMode(sr_srclk, OUTPUT);

  digitalWrite(sr_oe, INACTIVE_AL);

  digitalWrite(sr_rclk, LOW);
  shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000001);
  digitalWrite(sr_rclk, HIGH);

  digitalWrite(sr_oe, ACTIVE_AL);

  //digitalWrite(gnd, LOW);
  //shiftOut(shin, clock, LSBFIRST, 0x00);
    
//  digitalWrite(shin, LOW);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);

  delay(1);
}

void updateLeds()
{
  //digitalWrite(gnd, LOW);
  digitalWrite(sr_oe, INACTIVE_AL);

  shiftOut(shin, clock, LSBFIRST, data);
  
//  byte thisData = data;
//  
//  digitalWrite(shin, thisData & 0x01);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  thisData = thisData >> 1;
//  digitalWrite(shin, thisData & 0x01);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  thisData = thisData >> 1;
//  digitalWrite(shin, thisData & 0x01);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  thisData = thisData >> 1;
//  digitalWrite(shin, thisData & 0x01);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  thisData = thisData >> 1;
//  digitalWrite(shin, thisData & 0x01);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  thisData = thisData >> 1;
//  digitalWrite(shin, thisData & 0x01);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  thisData = thisData >> 1;
//  digitalWrite(shin, thisData & 0x01);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
//  thisData = thisData >> 1;
//  digitalWrite(shin, thisData & 0x01);
//  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);
  
  //digitalWrite(gnd, HIGH);  
  digitalWrite(sr_oe, ACTIVE_AL);	  
}

void effetRandom()
{
  if (!isInit)
  {
    randomSeed(analogRead(0));
    isInit = true;
  }
  data = random(256);
}

void effetChaserContinu()
{
  static boolean goingRight = true;
  
  if (!isInit)
  {
    data = B10000000;
    isInit = true;
  }
  else
  {
    if ((data == 0xFF) && goingRight)
    {
      goingRight = false;
      data = B11111110;
    }
    else if ((data == B10000000) && !goingRight)
    {
      goingRight = true;
      data = B11000000;
    }
    else if (goingRight)
      data = data | (data >> 1);
    else
      data = data & (data << 1);
  }
}

void effetChaser()
{
  if (!isInit)
  {
    data = B10000000;
    isInit = true;
  }
  else
  {
    if (data == 0x01)
      data = B10000000;
    else
      data = data >> 1;
  }
}

void effetChaserDouble()
{
  if (!isInit)
  {
    data = B10001000;
    isInit = true;
  }
  else
  {
    data = data >> 1;
    if (data == B00001000)
      data = B10001000;
  }
}

void effetAllUp()
{
  if (!isInit)
  {
    data = B11111111;
    isInit = true;
  }
}

void loop3()
{
}

void loop()
{
  static byte effet = 1;
  static unsigned long elapsed = millis();
  
  if (ALWAYS_ON) effet = 5;

  switch(effet) {
    case 1: 
      effetRandom();
      break;
    case 2:
      effetChaserContinu();
      break;
    case 3:
      effetChaser();
      break;
    case 4:
      effetChaserDouble();
      break;
	case 5:
	  effetAllUp();
	  break;
  }
  
  if (millis() - elapsed >= 3000)
  {
	if (!ALWAYS_ON)
      effet = effet + 1;
    if (effet > NB_MAX_EFFET) effet = 1;
    isInit = false;
    
    digitalWrite(led, HIGH);
    delay(10);
    digitalWrite(led, LOW);
    delay(90);
    digitalWrite(led, HIGH);
    delay(10);
    digitalWrite(led, LOW);
    delay(90);
    digitalWrite(led, HIGH);
    delay(10);
    digitalWrite(led, LOW);

    elapsed = millis();
  }
  
  updateLeds();
  delay(wait);
}

// the loop routine runs over and over again forever:
void loop2() {
//  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);               // wait for a second
//  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
//  delay(1000);               // wait for a second
  
  digitalWrite(shin, HIGH);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);

  digitalWrite(shin, LOW);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  digitalWrite(clock, LOW);   digitalWrite(clock, HIGH);   delay(wait);
  
}

// the loop routine runs over and over again forever:
void loop1() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
 
}
