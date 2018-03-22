//Ultrasonic Parking Assistant
//measures distance using ultrasonic sensor
//displays distance to serial

#include "display.h"



// Ultrasonic HC-SR04 unit interface
//define pins here
//if using fixed power pins, set to a negative number, and they will be ignored
#define UVCC 10
#define UTRIG 11
#define UECHO 12
#define UGND 13

//LED 16x16 Matrix interface
//define pins here
#define MATRIXLAT 2     // Latch
#define MATRIXCLK 3     // Clock
#define MATRIXDI 4      // Serial data
#define MATRIXG 5       // Blank
#define MATRIXA 6       // DCBA column bits
#define MATRIXB 7       
#define MATRIXC 8
#define MATRIXD 9
#define MATRIXFREQ 1500L  // trigger draw every Hz/1500 cycles per second.

byte matrixTopData[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};    //for display data. top row.
byte matrixBottomData[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};    //for display data. bottom row.


byte matrixscan=0;                                                //column being scanned


void setup(){
  usonicsetup();  
  matrixsetup();
  debugsetup();
}

void loop(){
  long d;               //for distance
  d=usonic(17400)/58;   //convert ping time to distance in cm
  if(d==0){d=300;}      //sometimes returns 0 when not in range
  debugdistance(d);
  updatedisplay(d);
  delay(500);
}



// -----------------------------------------------------------------------------------------
// Matrix
// -----------------------------------------------------------------------------------------

void matrixsetup(){
  //set up pins
  pinMode( MATRIXLAT, OUTPUT);  
  pinMode( MATRIXCLK, OUTPUT); 
  pinMode( MATRIXDI, OUTPUT);
  pinMode( MATRIXG, OUTPUT);
  pinMode( MATRIXA, OUTPUT);
  pinMode( MATRIXB, OUTPUT);
  pinMode( MATRIXC, OUTPUT);
  pinMode( MATRIXD, OUTPUT);
  
  // Timer 1 set up as a FREQ Hz sample interrupt, only common thing this affects is servo library
  TCCR1A = 0;                       // 
  TCCR1B = _BV(WGM12) | _BV(CS10);  // WGM12:CTC mode (clear timer on complete), CS10:no clock scaling.
  TCNT1 = 0;                        // actual timer value init 0
  OCR1A = F_CPU / MATRIXFREQ;       // F_CPU is Hz (cycles/sec)
  TIMSK1 = _BV(OCIE1A);             // enable TIMER1_COMPA_vect interrupt
}

ISR(TIMER1_COMPA_vect) {    //gets triggered FREQ times/second
  matrixscan++;                         //next column on matrix
  digitalWrite(MATRIXG,HIGH);           //blank for data shuffle
  digitalWrite(MATRIXA, (matrixscan&1));//set row
  digitalWrite(MATRIXB, (matrixscan&2));
  digitalWrite(MATRIXC, (matrixscan&4));
  digitalWrite(MATRIXD, (matrixscan&8));
  digitalWrite(MATRIXLAT, LOW);
  shiftOut(MATRIXDI, MATRIXCLK, LSBFIRST, 255-(matrixTopData[matrixscan&15]&255));    //output row data
  shiftOut(MATRIXDI, MATRIXCLK, LSBFIRST, 255-(matrixBottomData[matrixscan&15]&255));
  digitalWrite(MATRIXLAT, HIGH); //latch data
  digitalWrite(MATRIXG,LOW); //unblank
}


void updatedisplay(long d)
{
  if (d>999)
  {
    memcpy(matrixTopData,&LCD8x5_F,5);
    memcpy(matrixTopData+5,&LCD8x5_A,5);
    memcpy(matrixTopData+10,&LCD8x5_R,5);
  }
  else
  {
    int ones = d % 10;
    int hundreds = (d / 100);
    int tens = (d - (100 * hundreds)) / 10;

    if (hundreds == 0)
      memcpy(matrixTopData,&LCD8x5_BLANK,5);
    else
      setChar8x5(matrixTopData,hundreds);
    
    if (hundreds == 0 && tens == 0)
      memcpy(matrixTopData+5,&LCD8x5_BLANK,5);
    else
      setChar8x5(matrixTopData+5,tens);

    setChar8x5(matrixTopData+10,ones);
    
  }
    
}

//
// set the 
//
void setChar8x5(byte* data,int digit)
{
  
  switch (digit)
  {
    case -1:
      memcpy(data,&LCD8x5_BLANK,5);
      break;
    case 0:
      memcpy(data,&LCD8x5_0,5);
      break;
    case 1:
      memcpy(data,&LCD8x5_1,5);
      break;
    case 2:
      memcpy(data,&LCD8x5_2,5);
      break;
    case 3:
      memcpy(data,&LCD8x5_3,5);
      break;
    case 4:
      memcpy(data,&LCD8x5_4,5);
      break;
    case 5:
      memcpy(data,&LCD8x5_5,5);
      break;
    case 6:
      memcpy(data,&LCD8x5_6,5);
      break;
    case 7:
      memcpy(data,&LCD8x5_7,5);
      break;
    case 8:
      memcpy(data,&LCD8x5_8,5);
      break;
    case 9:
      memcpy(data,&LCD8x5_9,5);
      break;
    default:
      memcpy(data,&LCD8x5_SAD,5);
      break;
  }
  
  
}

// -----------------------------------------------------------------------------------------
// Display
// -----------------------------------------------------------------------------------------

void debugsetup(){
  Serial.begin(9600);
}

void debugdistance(long d){
  Serial.println(d);  
}



// -----------------------------------------------------------------------------------------
// Ultrasonic Sensor
// -----------------------------------------------------------------------------------------
void usonicsetup(void){
  if(UGND>-1){                  //set up ground pin if defined
    pinMode(UGND, OUTPUT);
    digitalWrite(UGND, LOW);    
  }
  if(UVCC>-1){                  //set up VCC pin if defined
    pinMode(UVCC, OUTPUT);
    digitalWrite(UVCC, HIGH);    
  }
  pinMode(UECHO, INPUT);        //ECHO pin is input
  pinMode(UTRIG, OUTPUT);       //TRIG pin is output
  digitalWrite(UTRIG, LOW);     
}

long usonic(long utimeout){    //utimeout is maximum time to wait for return in us
  long b;
  if(digitalRead(UECHO)==HIGH){return 0;}    //if UECHO line is still low from last result, return 0;
  digitalWrite(UTRIG, HIGH);  //send trigger pulse
  delay(1);
  digitalWrite(UTRIG, LOW);
  long utimer=micros();
  while((digitalRead(UECHO)==LOW)&&((micros()-utimer)<1000)){}  //wait for pin state to change- return starts after 460us typically
  utimer=micros();
  while((digitalRead(UECHO)==HIGH)&&((micros()-utimer)<utimeout)){}  //wait for pin state to change
  b=micros()-utimer;
  return b;
}

