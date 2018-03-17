//Ultrasonic Parking Assistant
//measures distance using ultrasonic sensor
//displays approximate closeness with colours on LED module
//Blue- out of range (3m typically)
//Green- in range, but far away
//Amber- closer
//Red- very close
#define GREENDISTANCE 200
#define AMBERDISTANCE 60
#define REDDISTANCE 30

// Ultrasonic HC-SR04 unit interface
//define pins here
//if using fixed power pins, set to a negative number, and they will be ignored
#define UVCC 10
#define UTRIG 11
#define UECHO 12
#define UGND 13

//define led module pins here, polarity is automatically handled by presence of LEDPLUS or LEDMINUS
//if LED's have common negative, use LEDMINUS
//if LED's have common positive, use LEDPLUS
//#define LEDMINUS 3
#define LEDPLUS 3
#define LEDBLUE 0
#define LEDRED 1
#define LEDGREEN 2

void setup(){
  usonicsetup();  
  ledsetup();
}

void loop(){
  byte r=0;             //for red led
  byte g=0;             //for green led
  byte b=0;             //for blue led
  long d;               //for distance
  d=usonic(17400)/58;   //convert ping time to distance in cm
  if(d==0){d=300;}      //sometimes returns 0 when not in range
  if(d<AMBERDISTANCE){r=1;}   //turn on red LED if red or amber needs to be shown
  if((d>=REDDISTANCE)&&(d<300)){g=1;}   //turn on green LED if amber or green needs to be shown
  if(d>=300){b=1;}            //turn on blue otherwise, so you know it's working
  ledset(r,g,b);              //set the LED's
  delay(200);                 //wait a bit so it won't flicker too much
}

void ledsetup(){      //set up led pins depending on whether they are common + or common -, turn all LED's off
#ifdef LEDPLUS
  pinMode(LEDPLUS, OUTPUT);
  digitalWrite(LEDPLUS, HIGH);    
  pinMode(LEDRED, OUTPUT);
  digitalWrite(LEDRED, HIGH);    
  pinMode(LEDGREEN, OUTPUT);
  digitalWrite(LEDGREEN, HIGH);    
  pinMode(LEDBLUE, OUTPUT);
  digitalWrite(LEDBLUE, HIGH);    
#endif
#ifdef LEDMINUS
  pinMode(LEDMINUS, OUTPUT);
  digitalWrite(LEDMINUS, LOW);    
  pinMode(LEDRED, OUTPUT);
  digitalWrite(LEDRED, LOW);    
  pinMode(LEDGREEN, OUTPUT);
  digitalWrite(LEDGREEN, LOW);    
  pinMode(LEDBLUE, OUTPUT);
  digitalWrite(LEDBLUE, LOW);    
#endif
}

void ledset(byte r, byte g, byte b){
#ifdef LEDPLUS
  r=!r;         //invert if we're using common +
  g=!g;
  b=!b;
#endif  
  digitalWrite(LEDRED, r);    //set outputs
  digitalWrite(LEDGREEN, g);    
  digitalWrite(LEDBLUE, b);    
}

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
