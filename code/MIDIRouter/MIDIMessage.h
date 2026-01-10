#ifndef __MIDIMESSAGE_H__
#define __MIDIMESSAGE_H__
/* This file declares the MIDIMessage struct, which will contain a
 * MIDI message that will be sent over the network.  A MIDI message
 * contains between 1 and 3 bytes.  The MIDIMessage struct contains
 * an array of unsigned char (3 bytes) and a member to specify how
 * many of those bytes are actually used.
 */

#include <cstring> // For memset()

struct MIDIMessage {
   // Constructor
   MIDIMessage()
      : numBytes(0)
   {
      memset(message, 0, sizeof(message));
   }

   unsigned char message[3]; // Contains the MIDI message data
   unsigned char numBytes;   // The actual number of bytes used (will be an integer value)
};

#endif