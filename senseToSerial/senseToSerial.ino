//Ultrasonic Parking Assistant
//measures distance using ultrasonic sensor
//displays distance to serial

// Ultrasonic HC-SR04 unit interface
//define pins here
//if using fixed power pins, set to a negative number, and they will be ignored
#define UVCC 10
#define UTRIG 11
#define UECHO 12
#define UGND 13


void setup(){
  usonicsetup();  
  displaysetup();
}

void loop(){
  long d;               //for distance
  d=usonic(17400)/58;   //convert ping time to distance in cm
  if(d==0){d=300;}      //sometimes returns 0 when not in range
  displaydistance(d);  
  delay(500);
}


// -----------------------------------------------------------------------------------------
// Display
// -----------------------------------------------------------------------------------------

void displaysetup(){
  Serial.begin(9600);
}

void displaydistance(long d){
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

