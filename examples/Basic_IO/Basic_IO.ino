#include <SimpleControl.h>

// Simple tutorial an how to receive and send SimpleControl messages.

int pins[] = {3, 5, 6, 9};

void callback(const unsigned long addr, float data)
{
  pinMode(addr, OUTPUT);
  analogWrite(addr, (data + 1) * 127);
}


void setup()
{
  SimpleControl.attach(callback);
  SimpleControl.begin();
}

void loop()
{
  while (SimpleControl.available())
  {
    SimpleControl.processInput();
  }

  for (int i=0; i<4; i++) {
    float sine = sin((float) millis() / 100 + (PI/2*i));
    SimpleControl.send(pins[i], sine);
  }
  delay(1);
}
