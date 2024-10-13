//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  Arduino Rotary Encoder Library                                                                                                      //
//  Copyright (C) 2019-2024 Jens Dyekjær Madsen                                                                                         //
//                                                                                                                                      //
//  Filename: RotEncoder.h                                                                                                              //
//                                                                                                                                      //
//  Description:                                                                                                                        //
//  Fast and precise rotary encoder library with low power consumption, optimized for high-speed and battery-powered applications.      //
//                                                                                                                                      //
//  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License    //
//  as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.             //
//                                                                                                                                      //
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of      //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.                   //
//  You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the              //
//  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA                                            //
//                                                                                                                                      //
//  It is not allowed to change any license or copyright statements, but feel free to modify, change, and add your own copyrights       //
//  below this line only !                                                                                                              //
//  ----------------------------------------------------------------------------------------------------------------------------------  //
//                                                                                                                                      //
//  Testet on AVR architecture: Arduino Nano.                                                                                           //
//                                                                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef ROTENCODER_H
#define ROTENCODER_H

#include <Arduino.h>
#include <util/atomic.h>  // Include atomic utility for critical sections. Required for 8/16-bit systems to ensure atomic operations

// Rotary Encoder Pin Connections to Arduino Nano:
//
//   Rotary Encoder       Arduino Nano
//   --------------       -------------
//      [ Pin A ]  ------- Pin 2 (Digital Input)
//      [ Pin B ]  ------- Pin 3 (Digital Input)
//      [ GND   ]  ------- GND
//
//   Note:
//   - Pin A and Pin B are connected to digital pins 2 and 3, which support interrupts.
//   - This setup assumes the encoder uses open-drain/open-collector outputs (actively pulls low, never high).
//   - If your encoder drives pins high, modify or override the code to prevent conflicts.

class RotEncoder {                                                                                      // Interface for RotEncoder
public:                                                                                                 // Default pin numbers, can be overridden
  inline virtual uint8_t getPinA() const __attribute__((always_inline)) { return 2;                   } // Setup default pin number to pin 2 for PinA
  inline virtual uint8_t getPinB() const __attribute__((always_inline)) { return 3;                   } // Setup default pin number to pin 3 for PinB

protected:
  void intr();                                                                                          // Interrupt handler function, called if PinA or PinB changes

// Input/output functions that can be overloaded e.g. to use faster or other I/O routines
  inline virtual bool rdPinA() __attribute__((always_inline))   { return !digitalRead(getPinA());     } // Reads pinA (high when switch closed, low open)
  inline virtual bool rdPinB() __attribute__((always_inline))   { return !digitalRead(getPinB());     } // Reads pinB (high when switch closed, low open)
  inline virtual void enPinA() __attribute__((always_inline))   { pinMode(getPinA(), INPUT_PULLUP);   } // Set to input and activate pull-up for pinA
  inline virtual void enPinB() __attribute__((always_inline))   { pinMode(getPinB(), INPUT_PULLUP);   } // Set to input and activate pull-up for pinB

// These functions are used to disable inputs, to lower static current:                                 // Disable input: Sets pullup inactive and sets pin to output low
  inline virtual void diPinA() __attribute__((always_inline))   { digitalWrite(getPinA(), LOW); pinMode(getPinA(), OUTPUT); } // Input disable for PinA
  inline virtual void diPinB() __attribute__((always_inline))   { digitalWrite(getPinB(), LOW); pinMode(getPinB(), OUTPUT); } // Input disable for PinB

public:
  long getPosition() const;                                                                             // Returns rotary encoder position
  bool begin();                                                                                         // Start rotary encoder, returns true if successful
  bool end();                                                                                           // Stop rotary encoder, returns true if successful
  virtual ~RotEncoder() { end(); }                                                                      // Destructor should call end() to safely detach interrupts

private:
  volatile long position = 0;                                                                           // Position, volatile, updated in interrupts
  bool cntflg = false;                                                                                  // Set true to count up, default is not count
  bool lrflg;                                                                                           // Left or right side last
  bool inA, inB;                                                                                        // Stores inA, inB (to not use of local variables in interrupt)
  typedef RotEncoder* volatile IntHandleT;                                                              // Pointer to this class for interrupts
  static IntHandleT intHandle;                                                                          // Pointer to interrupt handle = this when active
  static void isr() { if (intHandle!=NULL) intHandle->intr(); }                                         // Interrupt service routine executes intr() function in this class
};


template <uint8_t PinA, uint8_t PinB>                                                                   // Template to use other pins than standard pin <2,3>
class RotEncoderPins : public RotEncoder {
public:                                                                                                 // Overridden functions to setup PinA and PinB based on template parameters
  inline uint8_t getPinA() const override __attribute__((always_inline)) { return PinA; }               // Setup getPinA() to return with PinA from template
  inline uint8_t getPinB() const override __attribute__((always_inline)) { return PinB; }               // Setup getPinB() to return with PinB from template
};

#endif  // ROTENCODER_H
