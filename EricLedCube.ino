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
int led = 13;  // PORTC7
int clock = 2; // PORTD1
int shin = 3;  // PORTD0

int sr_ser = 5;    // PORTC6
int sr_oe = 7;     // PORTE6
int sr_rclk = 8;   // PORTB4
int sr_srclk = 10; // PORTB6

int wait = 500;

boolean isInit = false;
volatile byte data = 0x00;
byte currentp = 0;

boolean toggle1 = 0;

volatile byte data2[4] = {0, 0, 0, 0};

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

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 4;// = (16*10^6) / (1*1024) - 1 (must be <65536)
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

  while (!Serial)
  {
    message(3);
    delay(100);
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
{
  /*
    PORTE |= (1 << PORTE6);                           // digitalWrite(sr_oe, INACTIVE_AL);
    for(int i=0; i<8; i++)
    {
      PORTD &= ~(1 << PORTD1);
      PORTD = (PORTD & ~(1 << PORTD0)) | ((((data2[currentp] << 4) >> i) & 1) << PORTD0);
      PORTD |= (1 << PORTD1);
    }                                                 // shiftOut(shin, clock, LSBFIRST, thisdata << 4);
    PORTB &= ~(1 << PORTB4);                          // digitalWrite(sr_rclk, LOW);
    for(int i=7; i>=0; i--)
    {
      PORTB &= ~(1 << PORTB6);
      PORTC = (PORTC & ~(1 << PORTC6)) | ((((1 << currentp) >> i) & 1) << PORTC6);
      PORTB |= (1 << PORTB6);
    }                                                 // shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000001);
    PORTB |= (1 << PORTB4);                           // digitalWrite(sr_rclk, HIGH);
    PORTE &= ~(1 << PORTE6);                          // digitalWrite(sr_oe, ACTIVE_AL);

    currentp++;
    if (currentp > 3) currentp = 0;
    */
}

ISR(TIMER1_COMPA_vect)
{
	inttimer1();
}

#define TEST true
#define TIMEON 450

void loop5()
{
  boolean count = false;
  if (count)
  {
    data++;
    if (data >= B00010000) data = 0;
    data2[0] = data;
    data2[1] = data;
    data2[2] = data;
    data2[3] = data;
  }
  else
  {
    data=B00001111;
    data2[0] = B1111;
    data2[1] = B1111;
    data2[2] = B1111;
    data2[3] = B1111;
  }
  delay(333);
}

void loop()
{
  static int i = 0;
  int timeon[4] = {76, 450, 1000, 2500};

  static unsigned long t = 0;
  static unsigned long n = 0;

  static unsigned long ton = 0;
  static unsigned long ton_n = 0;

  static unsigned long toff = 0;
  static unsigned long toff_n = 0;

  if (n < 10000)
  {
    unsigned long t1 = micros();
    unsigned long t1off;
    unsigned long t1on;

    static byte thisdata = B00001111;
    static unsigned long thistime = millis();

    PORTF &= B00001111; // PORTE |= (1 << PORTE6);                           // digitalWrite(sr_oe, INACTIVE_AL);
    t1off = t1;
    PORTD |= 0x00;
    for(int i=0; i<8; i++)
    {
      PORTD &= ~(1 << PORTD1);
      PORTD = (PORTD & ~(1 << PORTD0)) | ((((thisdata << 4) >> i) & 1) << PORTD0);
      PORTD |= (1 << PORTD1);
    }                                                 // shiftOut(shin, clock, LSBFIRST, thisdata << 4);
    //PORTB &= ~(1 << PORTB4);                          // digitalWrite(sr_rclk, LOW);
    //for(int i=7; i>=0; i--)
    //{
    //  PORTB &= ~(1 << PORTB6);
    //  PORTC = (PORTC & ~(1 << PORTC6)) | (((B00000001 >> i) & 1) << PORTC6);
    //  PORTB |= (1 << PORTB6);
    //}                                                 // shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000001);
    //PORTB |= (1 << PORTB4);                           // digitalWrite(sr_rclk, HIGH);
    //PORTE &= ~(1 << PORTE6);                          // digitalWrite(sr_oe, ACTIVE_AL);
    PORTF |= (1 << PORTF7);
    t1on = micros();
    toff += t1on - t1off;
    
    delayMicroseconds(timeon[i]);

    PORTF &= B00001111; // PORTE |= (1 << PORTE6);                           // digitalWrite(sr_oe, INACTIVE_AL);
    t1off = micros();
    ton += t1off - t1on;
    PORTD |= 0x00;
    for(int i=0; i<8; i++)
    {
      PORTD &= ~(1 << PORTD1);
      PORTD = (PORTD & ~(1 << PORTD0)) | ((((thisdata << 4) >> i) & 1) << PORTD0);
      PORTD |= (1 << PORTD1);
    }                                                 // shiftOut(shin, clock, LSBFIRST, thisdata << 4);
    //PORTB &= ~(1 << PORTB4);                          // digitalWrite(sr_rclk, LOW);
    //for(int i=7; i>=0; i--)
    //{
    //  PORTB &= ~(1 << PORTB6);
    //  PORTC = (PORTC & ~(1 << PORTC6)) | (((B00000010 >> i) & 1) << PORTC6);
    //  PORTB |= (1 << PORTB6);
    //}                                                 // shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000010);
    //PORTB |= (1 << PORTB4);                           // digitalWrite(sr_rclk, HIGH);
    //PORTE &= ~(1 << PORTE6);                          // digitalWrite(sr_oe, ACTIVE_AL); 
    PORTF |= (1 << PORTF6);
    t1on = micros();
    toff += t1on - t1off;

    delayMicroseconds(timeon[i]);
    
    PORTF &= B00001111; // PORTE |= (1 << PORTE6);                           // digitalWrite(sr_oe, INACTIVE_AL);
    t1off = micros();
    ton += t1off - t1on;
    PORTD |= 0x00;
    for(int i=0; i<8; i++)
    {
      PORTD &= ~(1 << PORTD1);
      PORTD = (PORTD & ~(1 << PORTD0)) | ((((thisdata << 4) >> i) & 1) << PORTD0);
      PORTD |= (1 << PORTD1);
    }                                                 // shiftOut(shin, clock, LSBFIRST, thisdata << 4);
    //PORTB &= ~(1 << PORTB4);                          // digitalWrite(sr_rclk, LOW);
    //for(int i=7; i>=0; i--)
    //{
    //  PORTB &= ~(1 << PORTB6);
    //  PORTC = (PORTC & ~(1 << PORTC6)) | (((B00000100 >> i) & 1) << PORTC6);
    //  PORTB |= (1 << PORTB6);
    //}                                                 // shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000100);
    //PORTB |= (1 << PORTB4);                           // digitalWrite(sr_rclk, HIGH);
    //PORTE &= ~(1 << PORTE6);                          // digitalWrite(sr_oe, ACTIVE_AL); 
    PORTF |= (1 << PORTF5);
    t1on = micros();
    toff += t1on - t1off;

    delayMicroseconds(timeon[i]);

    PORTF &= B00001111; // PORTE |= (1 << PORTE6);                           // digitalWrite(sr_oe, INACTIVE_AL);
    t1off = micros();
    ton += t1off - t1on;
    PORTD |= 0x00;
    for(int i=0; i<8; i++)
    {
      PORTD &= ~(1 << PORTD1);
      PORTD = (PORTD & ~(1 << PORTD0)) | ((((thisdata << 4) >> i) & 1) << PORTD0);
      PORTD |= (1 << PORTD1);
    }                                                 // shiftOut(shin, clock, LSBFIRST, thisdata << 4);
    //PORTB &= ~(1 << PORTB4);                          // digitalWrite(sr_rclk, LOW);
    //for(int i=7; i>=0; i--)
    //{
    //  PORTB &= ~(1 << PORTB6);
    //  PORTC = (PORTC & ~(1 << PORTC6)) | (((B00001000 >> i) & 1) << PORTC6);
    //  PORTB |= (1 << PORTB6);
    //}                                                 // shiftOut(sr_ser, sr_srclk, MSBFIRST, B00001000);
    //PORTB |= (1 << PORTB4);                           // digitalWrite(sr_rclk, HIGH);
    //PORTE &= ~(1 << PORTE6);                          // digitalWrite(sr_oe, ACTIVE_AL); 
    PORTF |= (1 << PORTF4);
    t1on = micros();
    toff += t1on - t1off;

    delayMicroseconds(timeon[i]);

    unsigned long t2 = micros();
    ton += t2 - t1on;
    t += t2 - t1;
    n++;

    if ((millis() - thistime) >= 333)
    {
      thisdata++;
      if (thisdata >= B00010000) thisdata = 0;
      thistime = millis();
      //i++;
      //if (i == 4) i = 0;
      //Serial.print(timeon[i]);
      //Serial.print("\n");
    }

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
      Serial.print("LED CUBE STATISTICS\n");
      Serial.print("\n");
      Serial.print("time on delay="); Serial.print(timeon[i]); Serial.print("us\n");
      //Serial.print("t="); Serial.print(t); Serial.print("ms\n");
      //Serial.print("n="); Serial.print(n); Serial.print(" times\n");
      Serial.print("loop="); Serial.print((double)t / (double)n); Serial.print("us\n");
      //Serial.print("toff="); Serial.print(toff); Serial.print("ms\n");
      Serial.print("toff/loop="); Serial.print((double)toff / (double)n); Serial.print("us\n");
      //Serial.print("ton="); Serial.print(ton); Serial.print("ms\n");
      Serial.print("ton/loop="); Serial.print((double)ton / (double)n); Serial.print("us\n");
      Serial.print("duty cycle="); Serial.print(100.0 * ((double)ton / (double)n) / ((double)t / (double)n)); Serial.print("%\n");
      Serial.print("refresh rate="); Serial.print(1000000.0 / ((double)t / (double)n)); Serial.print("Hz\n");
      Serial.print("\n");
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
