#ifndef __SLEEP_H__
#define __SLEEP_H__
/* This file defines a macro, SLEEP(), that will be implemented differently, depending
 * on whether or not __WINDOWS_MM__ is defined.  This was copied from one of the test
 * programs in the RtMidi package.
 */

// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#endif