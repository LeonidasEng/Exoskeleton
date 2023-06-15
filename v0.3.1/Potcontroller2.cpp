#include <Servo.h>
#include <movingAvg.h> 

Servo myservo;
movingAvg avgpos(30);
int potpin = A0;
int val;    
int pos = 0; 

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  
  avgpos.begin();
}

void loop() 
{
  int rawVal = analogRead(potpin);
  int avgVal = avgpos.reading(rawVal);

  // Adjusting mapping function
  if(avgVal <= 512) {
    val = map(avgVal, 0, 512, 90, 0); // maps from 0 to 512 to 90 to 0
  } 
  else {
    val = map(avgVal, 513, 1023, 90, 180); // maps from 513 to 1023 to 90 to 180
  }

  myservo.write(val);  
  Serial.println(val);

  delay(15);  
}
