/*
  EricLedCube
 */

#define ACTIVE_AL LOW
#define INACTIVE_AL HIGH
#define ACTIVE_AH HIGH
#define INACTIVE_AH LOW

#define ALWAYS_ON true
#define ALWAYS_ON_EFFET 5

#define NB_MAX_EFFET 6

#define USE_74HC595 false
#define USE_IRQ_TIMER1 true

// Pin 13 has an LED connected on most Arduino boards.
int led = 13;  // PORTC7
int clock = 2; // PORTD1
int shin = 3;  // PORTD0

#if USE_74HC595 == true
int sr_ser = 5;    // PORTC6
int sr_oe = 7;     // PORTE6
int sr_rclk = 8;   // PORTB4
int sr_srclk = 10; // PORTB6
#endif

int wait = 500;
boolean isInit = false;

byte currentp = 0;
volatile byte data = 0x00;
volatile byte thisdata[4] = {0x00, 0x00, 0x00, 0x00};


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
  
#if USE_74HC595 == true
  pinMode(sr_oe, OUTPUT);
  pinMode(sr_rclk, OUTPUT);
  pinMode(sr_ser, OUTPUT);
  pinMode(sr_srclk, OUTPUT);
#else
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
#endif

#if USE_74HC595 == true
  digitalWrite(sr_oe, INACTIVE_AL);
  digitalWrite(sr_rclk, LOW);
  shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000000);
  digitalWrite(sr_rclk, HIGH);
  digitalWrite(sr_oe, ACTIVE_AL);
#else
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
#endif

#if USE_IRQ_TIMER1 == true    
    cli();//stop interrupts

    //set timer1 interrupt at 735Hz
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register for 735hz increments
    OCR1A = 399;// = (16*10^6) / (735*8) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS11 bit for 8 prescaler
    TCCR1B |= (1 << CS11);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);

    sei();//allow interrupts
#endif

  Serial.begin(115200);

  delay(1);

  while (!Serial)
  {
    message(3);
    delay(100);
  }
}

// Allume de 0 a 4 LEDs au hasard.
void effetRandom()
{
    if (!isInit)
    {
        randomSeed(analogRead(A4));
        isInit = true;
    }
    data = random(B1111 + 1);
}

// Allume les LEDs suivant un compteur binaire.
void effetBinaryCounter()
{
    if (!isInit)
    {
        data = B0000;
        isInit = true;
    }
    if (data == B1111)
        data = B0000;
    else
        data++;
}

// Allume les LEDs de gauche a droite et les
// eteint de droite a gauche.
void effetChaserContinu()
{
    static boolean goingRight = true;

    if (!isInit)
    {
        data = B1000;
        isInit = true;
    }
    else
    {
        if (data == B1111)
        {
            goingRight = false;
        }
        else if (data == B1000)
        {
            goingRight = true;
        }
        
        if (goingRight)
            data = data | (data >> 1);
        else
            data = data & (data << 1);
    }
}

// Allume un LED un apres l'autre vers la droite et
// recommence a gauche.
void effetChaser()
{
    if (!isInit)
    {
        data = B1000;
        isInit = true;
    }
    else
    {
        if (data == B0001)
            data = B1000;
        else
            data = data >> 1;
    }
}

// Allume les LEDs 3 et 1, ensuite les LEDs 2 et 0.
void effetChaserDouble()
{
    if (!isInit)
    {
        data = B1010;
        isInit = true;
    }
    else
    {
        if (data == B0101)
            data = B1010;
        else
            data = data >> 1;
    }
}

// Allume toutes les LEDs.
void effetAllUp()
{
    if (!isInit)
    {
        data = B1111;
        isInit = true;
    }
}

#if USE_IRQ_TIMER1 == true
inline void inttimer1(void)
{

#if USE_74HC595 == true
    PORTE |= (1 << PORTE6);                           // digitalWrite(sr_oe, INACTIVE_AL);
#else
    PORTF &= B00001111;
#endif

    for(int i=0; i<8; i++)
    {
        PORTD &= ~(1 << PORTD1);
        PORTD = (PORTD & ~(1 << PORTD0)) | (((thisdata[currentp] >> i) & 1) << PORTD0);
        PORTD |= (1 << PORTD1);
    }                                                 // shiftOut(shin, clock, LSBFIRST, thisdata << 4);

#if USE_74HC595 == true
    PORTB &= ~(1 << PORTB4);                          // digitalWrite(sr_rclk, LOW);
    for(int i=7; i>=0; i--)
    {
        PORTB &= ~(1 << PORTB6);
        PORTC = (PORTC & ~(1 << PORTC6)) | ((((1 << currentp) >> i) & 1) << PORTC6);
        PORTB |= (1 << PORTB6);
    }                                                 // shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000001);
    PORTB |= (1 << PORTB4);                           // digitalWrite(sr_rclk, HIGH);
    PORTE &= ~(1 << PORTE6);                          // digitalWrite(sr_oe, ACTIVE_AL);
#else
    PORTF |= (1 << currentp + 4);
#endif

    if (currentp >= 3) 
        currentp = 0;
    else
        currentp++;
}

ISR(TIMER1_COMPA_vect) { inttimer1(); }
#endif

#if USE_IRQ_TIMER1 == true
void loop()
{
    updateAnimation();
    thisdata[0] = data;
    thisdata[1] = data;
    thisdata[2] = data;
    thisdata[3] = data;
    delay(wait);
}
#endif

#if USE_IRQ_TIMER1 == false
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

        static unsigned long thistime = millis();

        for(int plane=0; plane<4; plane++)
        {
#if USE_74HC595 == true
            PORTE |= (1 << PORTE6);                           // digitalWrite(sr_oe, INACTIVE_AL);
#else
            PORTF &= B00001111; 
#endif
            t1off = t1;
            for(int i=0; i<8; i++)
            {
                PORTD &= ~(1 << PORTD1);
                PORTD = (PORTD & ~(1 << PORTD0)) | (((thisdata[plane] >> i) & 1) << PORTD0);
                PORTD |= (1 << PORTD1);
            }                                                 // shiftOut(shin, clock, LSBFIRST, thisdata << 4);
#if USE_74HC595 == true
            PORTB &= ~(1 << PORTB4);                          // digitalWrite(sr_rclk, LOW);
            for(int i=7; i>=0; i--)
            {
                PORTB &= ~(1 << PORTB6);
                PORTC = (PORTC & ~(1 << PORTC6)) | ((((1 << plane) >> i) & 1) << PORTC6);
                PORTB |= (1 << PORTB6);
            }                                                 // shiftOut(sr_ser, sr_srclk, MSBFIRST, B00000001);
            PORTB |= (1 << PORTB4);                           // digitalWrite(sr_rclk, HIGH);
            PORTE &= ~(1 << PORTE6);                          // digitalWrite(sr_oe, ACTIVE_AL);
#else
            PORTF |= (1 << PORTF7);
#endif
            t1on = micros();
            toff += t1on - t1off;

            delayMicroseconds(timeon[i]);
        }

#if USE_74HC595 == true
        PORTE |= (1 << PORTE6);                           // digitalWrite(sr_oe, INACTIVE_AL);
#else
        PORTF &= B00001111;
#endif

        unsigned long t2 = micros();
        ton += t2 - t1on;
        t += t2 - t1;
        n++;

        if ((millis() - thistime) >= wait)
        {
            updateAnimation();
            thisdata[0] = data;
            thisdata[1] = data;
            thisdata[2] = data;
            thisdata[3] = data;
            thistime = millis();
        }
    }
    else
    {
        static boolean done = false;
        if (!done)
        {
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
#endif

void updateAnimation()
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
    case 6:
        effetBinaryCounter();
        break;
    }

    if (millis() - elapsed >= 3000)
    {
        isInit = false;

        if (effet >= NB_MAX_EFFET) 
            effet = 1;
        else
            effet = effet + 1;

        message(3);

        elapsed = millis();
    }
}
