/*
  EricLedCube
 */

#define ACTIVE_AL LOW
#define INACTIVE_AL HIGH
#define ACTIVE_AH HIGH
#define INACTIVE_AH LOW

#define ALWAYS_ON true
#define ALWAYS_ON_EFFET 2

#define NB_MAX_EFFET 5

// Pin 13 has an LED connected on most Arduino boards.
int led = 13;
int clock = 2;
int shin = 3;

int sr_ser = 5;
int sr_oe = 7;
int sr_rclk = 8;
int sr_srclk = 10;

int wait = 500;

boolean isInit = false;
volatile byte data = 0x00;
byte currentp = 0;

boolean toggle1 = 0;

void message(int nb)
{
  for (int i=0; i<nb; i++)
  {
    digitalWrite(led, HIGH);
    delay(10);
    digitalWrite(led, LOW);
    delay(90);
  }
}

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


  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 20;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts


  Serial.begin(115200);

  digitalWrite(sr_oe, INACTIVE_AL);
  digitalWrite(sr_rclk, LOW);
  shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000000);
  digitalWrite(sr_rclk, HIGH);
  digitalWrite(sr_oe, ACTIVE_AL);

  delay(1);

  while (!Serial) {
    ;
  }
}

void updateLeds()
{
  digitalWrite(sr_oe, INACTIVE_AL);
  shiftOut(shin, clock, LSBFIRST, data);
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

inline void inttimer1(void)
{//timer1 interrupt 1Hz toggles pin 13 (LED)
  //generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  /*if (toggle1){
    digitalWrite(13,HIGH);
    toggle1 = 0;
  }
  else{
    digitalWrite(13,LOW);
    toggle1 = 1;
  }*/
    /*digitalWrite(sr_oe, INACTIVE_AL);
    shiftOut(shin, clock, LSBFIRST, data << 4);

    digitalWrite(sr_rclk, LOW);
    shiftOut(sr_ser, sr_srclk, MSBFIRST, B1 << currentp++);
    digitalWrite(sr_rclk, HIGH);
    digitalWrite(sr_oe, ACTIVE_AL);

    if (currentp > 3) currentp = 0;*/
}

ISR(TIMER1_COMPA_vect)
{
	inttimer1();
}

#define TEST true
#define TIMEON 450

void loop5()
{
  //data++;
  //if (data >= B00010000) data = 0;
  data=255;
  delay(333);
}

void loop()
{
  static int i = 2;
  int timeon[4] = {1, 450, 1000, 2500};

  static unsigned long t = 0;
  static unsigned long n = 0;

  static unsigned long ton = 0;
  static unsigned long ton_n = 0;

  static unsigned long toff = 0;
  static unsigned long toff_n = 0;

  if (n < 5000)
  {
    unsigned long t1 = millis();
    unsigned long t1off;
    unsigned long t1on;

    static byte thisdata = B00001111;
    static unsigned long thistime = millis();

    digitalWrite(sr_oe, INACTIVE_AL);
    t1off = t1;
    shiftOut(shin, clock, LSBFIRST, thisdata << 4);
    digitalWrite(sr_oe, ACTIVE_AL);
    t1on = millis();
    delayMicroseconds(timeon[i]);
    toff += t1on - t1off;

    digitalWrite(sr_rclk, LOW);
    shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000001);
    digitalWrite(sr_rclk, HIGH);

    digitalWrite(sr_oe, INACTIVE_AL);
    t1off = millis();
    ton += t1off - t1on;
    shiftOut(shin, clock, LSBFIRST, thisdata << 4);
    digitalWrite(sr_oe, ACTIVE_AL); 
    t1on = millis();
    delayMicroseconds(timeon[i]);
    toff += t1on - t1off;

    digitalWrite(sr_rclk, LOW);
    shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000010);
    digitalWrite(sr_rclk, HIGH);

    digitalWrite(sr_oe, INACTIVE_AL);
    t1off = millis();
    ton += t1off - t1on;
    shiftOut(shin, clock, LSBFIRST, thisdata << 4);
    digitalWrite(sr_oe, ACTIVE_AL); 
    t1on = millis();
    delayMicroseconds(timeon[i]);
    toff += t1on - t1off;

    digitalWrite(sr_rclk, LOW);
    shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000100);
    digitalWrite(sr_rclk, HIGH);

    digitalWrite(sr_oe, INACTIVE_AL);
    t1off = millis();
    ton += t1off - t1on;
    shiftOut(shin, clock, LSBFIRST, thisdata << 4);
    digitalWrite(sr_oe, ACTIVE_AL); 
    t1on = millis();
    delayMicroseconds(timeon[i]);
    toff += t1on - t1off;

    digitalWrite(sr_rclk, LOW);
    shiftOut(sr_ser, sr_srclk, MSBFIRST, B00001000);
    digitalWrite(sr_rclk, HIGH);

    if ((millis() - thistime) >= 1000)
    {
      //thisdata++;
      if (thisdata >= B00010000) thisdata = 0;
      thistime = millis();
      //i++;
      //if (i == 4) i = 0;
      //Serial.print(timeon[i]);
      //Serial.print("\n");
    }

    unsigned long t2 = millis();
    ton += t2 - t1on;
    t += t2 - t1;
    n++;
  }
  else
  {
    static boolean done = false;
    if (!done)
    {
      shiftOut(shin, clock, LSBFIRST, B00010000);
      digitalWrite(sr_rclk, LOW);
      shiftOut(sr_ser, sr_srclk, MSBFIRST, B00001000);
      digitalWrite(sr_rclk, HIGH);

      digitalWrite(sr_oe, INACTIVE_AL);
      
      Serial.print("\n");
      Serial.print("\n");
      Serial.print("\n");
      Serial.print("LED CUBE STATISTICS\n");
      Serial.print("\n");
      //Serial.print("time on delay="); Serial.print(timeon[i]); Serial.print("us\n");
      //Serial.print("t="); Serial.print(t); Serial.print("ms\n");
      //Serial.print("n="); Serial.print(n); Serial.print(" times\n");
      Serial.print("loop="); Serial.print((double)t / (double)n); Serial.print("ms\n");
      //Serial.print("toff="); Serial.print(toff); Serial.print("ms\n");
      Serial.print("toff/loop="); Serial.print((double)toff / (double)n); Serial.print("ms\n");
      //Serial.print("ton="); Serial.print(ton); Serial.print("ms\n");
      Serial.print("ton/loop="); Serial.print((double)ton / (double)n); Serial.print("ms\n");
      Serial.print("duty cycle="); Serial.print(100.0 * ((double)ton / (double)n) / ((double)t / (double)n)); Serial.print("%\n");
      Serial.print("refresh rate="); Serial.print(1000.0 / ((double)t / (double)n)); Serial.print("Hz\n");
    }
    done = true;
  }
}

void loop3()
{
  static byte effet = 1;
  static unsigned long elapsed = millis();
  
  if (ALWAYS_ON) effet = ALWAYS_ON_EFFET;

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
    
    message(3);

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
