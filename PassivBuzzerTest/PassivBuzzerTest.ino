#include <Arduino.h>

enum biepstate
{
  low = 0,
  equal = 1,
  up1 = 2,
  up2 = 3,
  up3 = 4,
  up4 = 5,
};
class Tonestate{
  public:
    biepstate state;
    float frequency;
    //Achtung duration = modulaCount --> kleiner und es werden mehr Töne pro Sekunde gespielt
    int duration;
    float lowerVelocity;
    float upVelocity;
};
Tonestate ToneArray[6] = {
  {biepstate::low, 120, -1, -999, -2.5f},
  {biepstate::equal, 0, 0, -2.5f, 0},
  {biepstate::up1, 700, 25, 0, 1.5f},
  {biepstate::up2, 850, 18, 1.5f, 3.5f},
  {biepstate::up3, 1000, 10, 3.5f, 10},
  {biepstate::up4, 1600, 3, 10, 999},
};
int indexCurrentToneArray = 1;

float frequencyNow = 0;
int durationNow = 0;
bool start = true;
int deltaTimeFrame = 30;

float velocity = 1.0f;
unsigned int count = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  if(Serial.available() > 0)
  {
    char incomingByte = (char)Serial.read();
    if(incomingByte == 'p')
    {
      start = !start;
      noTone(8);
    }
    if(incomingByte == 'f')
    {
      ToneArray[indexCurrentToneArray].frequency = Serial.parseFloat();
      Serial.println(String(ToneArray[indexCurrentToneArray].frequency) + "  " + String(ToneArray[2].frequency));
      setNewBeep();
    }
    if(incomingByte == 'd')
    {
      ToneArray[indexCurrentToneArray].duration = Serial.parseFloat();
      setNewBeep();
    }
    if(incomingByte == 's')
    {
      int index = Serial.parseInt();
      indexCurrentToneArray = index;
      setNewBeep();
    }
    if(incomingByte == 'v')
    {
      velocity = Serial.parseFloat();
      setNewBeep();
    }
  }
  if(!start)
    return;

  if(ToneArray[indexCurrentToneArray].state != biepstate::low && ToneArray[indexCurrentToneArray].state != biepstate::equal && count%durationNow == 0)
  {
    tone(8, frequencyNow, (deltaTimeFrame*durationNow)/2);
  }
  count++;
  delay(30);
}

void setNewBeep()
{
  int lengthTonestate = sizeof(ToneArray)/sizeof(Tonestate);
  for (size_t i = 0; i < lengthTonestate; i++)
  {
    if(velocity >= ToneArray[i].lowerVelocity && velocity <= ToneArray[i].upVelocity)
    {
      indexCurrentToneArray = i;
      if((lengthTonestate - 1) == i)
      {
        frequencyNow = ToneArray[indexCurrentToneArray].frequency;
        durationNow = ToneArray[indexCurrentToneArray].duration;
      }
      else
      {
        float factor = (velocity - ToneArray[indexCurrentToneArray].lowerVelocity)/(ToneArray[indexCurrentToneArray].upVelocity-ToneArray[indexCurrentToneArray].lowerVelocity);
        frequencyNow = factor * (ToneArray[i+1].frequency - ToneArray[indexCurrentToneArray].frequency) + ToneArray[indexCurrentToneArray].frequency;
        durationNow = factor * (ToneArray[i+1].duration - ToneArray[indexCurrentToneArray].duration) + ToneArray[indexCurrentToneArray].duration;
      }
      Serial.println(String(frequencyNow) + "  " + String(durationNow) + "  " + String(ToneArray[indexCurrentToneArray].state));
    }
    if(ToneArray[indexCurrentToneArray].state == biepstate::low)
      tone(8, ToneArray[indexCurrentToneArray].frequency);
    if(ToneArray[indexCurrentToneArray].state == biepstate::equal)
      noTone(8);
  }
}
