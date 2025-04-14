#include "ISR_Encoder.h"
#include "Arduino.h"

ISR_Encoder encoder(PB12, PB13);
void setup()
{
  Serial.begin(250000);
  // put your setup code here, to run once:
  encoder.enableEncoder(true, []{encoder.InterruptRoutine();});
  encoder.SetValue(10.23,0.1);
  encoder.SetRange(0,20);
  //encoder.enabled=true;
  encoder.rotaryAccelerationCoef=50;
  encoder.encoderStepFactor=2;
  encoder.Increament=0.001;
}

void loop()
{
  if(encoder.isValueChanged())Serial.println(encoder.Read(),3);
  // put your main code here, to run repeatedly:
}
