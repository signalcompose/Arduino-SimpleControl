#ifndef SIMPLECONTROL_H
#define SIMPLECONTROL_H

#include <Boards.h>

namespace simpleControl
{

  // Version numbers for the SimpleControl Library.
  static const int FIRMWARE_MAJOR_VERSION =  0;
  static const int FIRMWARE_MINOR_VERSION =  1;
  static const int FIRMWARE_BUGFIX_VERSION = 0;

  // Version numbers for the protocol.
  static const int PROTOCOL_MAJOR_VERSION =  0;
  static const int PROTOCOL_MINOR_VERSION =  1;
  static const int PROTOCOL_BUGFIX_VERSION = 0;

  // message command bytes
  static const char START_BYTE  = 0x2A;
  static const char FINISH_BYTE = 0x2B;
  static const char ESCAPE_BYTE = 0x2C;
  static const char ESCAPE_MASK = 0x20;

  // others
  static const int MAX_DATA_BYTES = 8;
  static const long DEFAULT_BAUD = 115200;

class SimpleControlClass
{
public:
  typedef void (*callbackFunction)(unsigned long, float);

  SimpleControlClass();

  /* Arduino constractors */
  void begin(void);
  void begin(long baudrate);
  void begin(Stream &s);

  /* Serial receive handling*/
  int available(void);
  void processInput(void);

  void attach(callbackFunction newFunction);

  /* Serial send handling*/
  void send(unsigned long address, float data);

private:
  Stream * SimpleControlStream;

  char dataBuffer[MAX_DATA_BYTES];
  size_t pointer;
  bool allowBufferUpdate;
  bool escape;

  /* utility methods */
  unsigned long decode2Int(const char *bytes);
  float decode2Float(const char *bytes);

  /* receive methods */
  void parse(char inputData);
  bool isEscapeByte(const char value);

  /* send methods */
  void streamBytes(const char *bytes, size_t size);

  /* callback function */
  static callbackFunction currentCallback;
};

} // namespace

extern "C" {
  typedef simpleControl::SimpleControlClass::callbackFunction callbackFunction;
}

extern simpleControl::SimpleControlClass SimpleControl;

#endif
