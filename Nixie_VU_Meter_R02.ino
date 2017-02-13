#include <math.h>

#define IN_L A3
#define IN_R A1
#define OUT_L 0
#define OUT_R 1
#define TUBE_EN 4

#define SILENCE_THRESHOLD 0.036 //LMC660 was using 0.012
#define SILENCE_TIME 17000 //30000 was ~53 sec so 17000 should be ~30 sec
#define SILENCE_RETURN 250 //~0.5 sec?
void setup() {
  // put your setup code here, to run once:
  pinMode(OUT_L, OUTPUT);
  pinMode(OUT_R, OUTPUT);
  pinMode(TUBE_EN, OUTPUT);
  pinMode(IN_L, INPUT);
  pinMode(IN_R, INPUT);
  //Serial.begin(9600);
  
  
  digitalWrite(TUBE_EN, HIGH);
  //initial ramp to show off tubes!
  tubeTest();
  
}

void tubeTest()
{
  for(int i = 0; i < 250; i+=8)
  {
    analogWrite(OUT_L, i);
    analogWrite(OUT_R, i);
    delay(30);
  }  
  delay(150);
}
unsigned long silentCount = 0;
bool silent = false;
void loop() {
  // put your main code here, to run repeatedly:

  //get value
  int lValue = analogRead(IN_L);
  int rValue = analogRead(IN_R);

//  Serial.print("Sensor read: ");
//  Serial.print(lValue);
  
  //to volts
  float lVoltage= lValue * (5.0 / 1023.0);
  float rVoltage= rValue * (5.0 / 1023.0);  

//  Serial.print("\tVolts: ");
//  Serial.print(lVoltage);
  
  //silence check
  if(lVoltage < SILENCE_THRESHOLD && rVoltage < SILENCE_THRESHOLD)
  {
    if(!silent) //DONT'T COMBINE WITH PREVIOUS CHECK BECAUSE WE WANT A DO NOTHING CONDITION
    {
      silentCount++;
      if(silentCount > SILENCE_TIME)
      {
        silent = true;
        silentCount = 0;
        digitalWrite(TUBE_EN, LOW);
        analogWrite(OUT_L, 0);
        analogWrite(OUT_R, 0);
        delay(500);
      }
    }
  }
  else if(silent)
  {
    silentCount++;
    if(silentCount > SILENCE_RETURN)
    {
      silent = false;
      silentCount = 0;
      digitalWrite(TUBE_EN, HIGH);
    }
  }
  else
  {
    silentCount = 0;
  }

  //to dbV
  lVoltage = 6.2 * log10(lVoltage+1);
  rVoltage = 6.2 * log10(rVoltage+1);
  
//  Serial.print("\tdB: ");
//  Serial.print(lVoltage);

  //offset for analog system
  lVoltage = lVoltage + 0.02;
  rVoltage = rVoltage + 0.02;

  lVoltage = constrain(lVoltage, 0, 5);
  rVoltage = constrain(rVoltage, 0, 5);
  
  //to output range 0-5 -> 0-255
  lValue = (int) (lVoltage * (250.0/5.0));
  rValue = (int) (rVoltage * (250.0/5.0));
  
//  Serial.print("\tSensor write: ");
//  Serial.println(lValue);
  
  analogWrite(OUT_L, lValue);
  analogWrite(OUT_R, rValue);
  delayMicroseconds(10);//100
}
