#ifndef _DEBUG_H
#define _DEBUG_H

//#define DEBUG //Enable debug prints

#ifdef DEBUG
  #include <SoftwareSerial.h>
  extern SoftwareSerial gConsole;

  #define DEBUG_PRINT(x) gConsole.print(x)
  #define DEBUG_PRINTLN(x) gConsole.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#endif // _DEBUG_H
