//******************************************************************************
//* Includes
//******************************************************************************

#include "SimpleControl.h"
#include "HardwareSerial.h"

using namespace simpleControl;

//******************************************************************************
//* Static Members
//******************************************************************************

// make one instance for the user to use
SimpleControlClass SimpleControl;

callbackFunction SimpleControlClass::currentCallback = (callbackFunction)NULL;

//******************************************************************************
//* Constructors
//******************************************************************************

SimpleControlClass::SimpleControlClass()
:
  pointer(0),
  escape(false)
{
  allowBufferUpdate = ((char *)NULL == dataBuffer);
}

//******************************************************************************
//* Public Methods
//******************************************************************************

void SimpleControlClass::begin(void)
{
  begin(DEFAULT_BAUD);
}

void SimpleControlClass::begin(long baudrate)
{
  Serial.begin(baudrate);
  begin(Serial);
}

void SimpleControlClass::begin(Stream &s)
{
  SimpleControlStream = &s;
}

// Serial Receive Handling

int SimpleControlClass::available(void)
{
  return SimpleControlStream->available();
}

void SimpleControlClass::processInput(void)
{
  int inputData = SimpleControlStream->read();
  if (inputData != -1)
  {
    parse((char)inputData);
  }
}

void SimpleControlClass::attach(callbackFunction newFunction)
{
  currentCallback = newFunction;
}

// Serial Send Handling

void SimpleControlClass::send(unsigned long address, float data)
{
  char addrBytes[4];
  char dataBytes[4];

  addrBytes[0] = (address >> 24) & 0xFF;
  addrBytes[1] = (address >> 16) & 0xFF;
  addrBytes[2] = (address >>  8) & 0xFF;
  addrBytes[3] = (address >>  0) & 0xFF;

  union {float f; unsigned long l;} u;
  u.f = data;
  
  dataBytes[0] = (u.l >> 24) & 0xFF;
  dataBytes[1] = (u.l >> 16) & 0xFF;
  dataBytes[2] = (u.l >>  8) & 0xFF;
  dataBytes[3] = (u.l >>  0) & 0xFF;

  SimpleControlStream->write(START_BYTE);
  streamBytes(addrBytes, 4);
  streamBytes(dataBytes, 4);
  SimpleControlStream->write(FINISH_BYTE);
}

//******************************************************************************
//* Private Methods
//******************************************************************************

// utility methods

unsigned long SimpleControlClass::decode2Int(const char *bytes) {
  unsigned long data = (unsigned long) (
        (((long)bytes[0] << 24) & 0xFF000000)
      | (((long)bytes[1] << 16) & 0x00FF0000)
      | (((long)bytes[2] <<  8) & 0x0000FF00)
      | (((long)bytes[3] <<  0) & 0x000000FF)
  );
  return data;
}

float SimpleControlClass::decode2Float(const char *bytes) {
  union { float f; long l; } a;
  a.l = (long) (
      (((long)bytes[4] << 24) & 0xFF000000)
    | (((long)bytes[5] << 16) & 0x00FF0000)
    | (((long)bytes[6] <<  8) & 0x0000FF00)
    | (((long)bytes[7] <<  0) & 0x000000FF)
  );
  return a.f;
}

// receive methods
void SimpleControlClass::parse(char inputData)
{
  if (inputData == START_BYTE) {
    pointer = 0;
  } else if ( (inputData == FINISH_BYTE) && (pointer == MAX_DATA_BYTES)) {
    pointer ++;
    unsigned long address = decode2Int(dataBuffer);
    float data = decode2Float(dataBuffer);
    if (currentCallback) (*currentCallback)(address, data);
  } else if ( (pointer < MAX_DATA_BYTES) && (!isEscapeByte(inputData)) ) {
    if (inputData == ESCAPE_BYTE) {
      escape = true;
      return;
    } else {
      if (escape) {
        dataBuffer[pointer] = inputData ^ ESCAPE_MASK;
        escape = false;
      }
      else
        dataBuffer[pointer] = inputData;
      pointer ++;
    }
  } else {
    // TODO: Exception
  }
}

bool SimpleControlClass::isEscapeByte(char value)
{
  return ((value == START_BYTE) || (value == ESCAPE_BYTE) || (value == FINISH_BYTE));
}

// send methods

void SimpleControlClass::streamBytes(const char *bytes, size_t size)
{
  if (SimpleControlStream == (Stream *)NULL) { return; }

  for (int i=0; i<size; i++)
  {
    if (isEscapeByte(bytes[i]))
    {
      SimpleControlStream->write(ESCAPE_BYTE);
      SimpleControlStream->write(bytes[i] ^ ESCAPE_MASK);
    }
    else
    {
      SimpleControlStream->write(bytes[i]);
    }
  }
}
