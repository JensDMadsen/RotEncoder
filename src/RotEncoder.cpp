//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  Arduino Rotary Encoder Library                                                                                                      //
//  Copyright (C) 2019-2024 Jens Dyekjær Madsen                                                                                         //
//                                                                                                                                      //
//  Filename: RotEncoder.cpp                                                                                                            //
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


#include "RotEncoder.h"

RotEncoder::IntHandleT RotEncoder::intHandle = NULL;                                                    // Initialize interrupt handle pointer to indicate no connection to class


bool RotEncoder::begin() {                                                                              // Start rotary encoder, returns true if successful
  // Method to begin interrupt handling by setting up the static pointer and attaching the ISR
  if (intHandle == nullptr) {                                                                           // Only start if the handler is not already set
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      // Use atomic block for 8/16-bit systems. It ensures that interrupts are fully disabled during
      // pointer assignment, providing safety in case previous detachInterrupt() wasn't called or failed.
      intHandle = this;                                                                                 // Set the static pointer to the current instance
    }
    enPinA();                                                                                           // Enable PinA and PinB inputs with pull-up
    enPinB();
    attachInterrupt(digitalPinToInterrupt(getPinA()), isr, CHANGE);                                     // Attach interrupt for PinA, set to change pin
    attachInterrupt(digitalPinToInterrupt(getPinB()), isr, CHANGE);                                     // Attach interrupt for PinB, set to change pin
    return true;                                                                                        // Return true if ok
  }
  return false;                                                                                         // Return false if intHandle is already set
}

bool RotEncoder::end() {                                                                                // Stops interrupt handling, returns true if successful
  // Method to stop interrupt handling by detaching the ISR and resetting the static pointer
  if (intHandle == this) {  // Only stop if intHandle points to this instance
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      // Use atomic block for 8/16-bit systems. If detachInterrupt() fails, this atomic block ensures
      // intHandle is safely set to nullptr, preventing any invalid interrupt calls. Since detachInterrupt()
      // doesn't return success/failure code, handling this carefully is important.
      intHandle = nullptr;                                                                              // Remove handle to this class, set to nullptr
    }
    detachInterrupt(digitalPinToInterrupt(getPinA()));                                                  // Detach interrupt for PinA
    detachInterrupt(digitalPinToInterrupt(getPinB()));                                                  // Detach interrupt for PinB
    return true;                                                                                        // Return true if ok
  }
  return false;                                                                                         // Return false if intHandle doesn't match this instance
}

long RotEncoder::getPosition() const {                                                                  // Returns actual value from the rotary encoder
  long pos;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                                   // Read position atomic
    pos = position;
  }
  return pos;                                                                                           // and return it
}

void RotEncoder::intr() {                                                                               // Immplementation of interrupt handler for rotary encoder
  cli();                                                                                                // No interrupts allowed during read
  enPinA(); enPinB();                                                                                   // Uses built-in pull-ups
  do {                                                                                                  // Read inputs, ensure stable valid readings
    inA = rdPinA(); inB = rdPinB();                                                                     //   Read inputs
  } while((inA!=rdPinA())||(inB!=rdPinB()));                                                            // Retry until stable

  // This is code for rotary switch:
  if (inA) {                                                                                            // Switch(inA,inB) case:
    if (inB) {                                                                                          //   InA on and InB on:
      cntflg = true;                                                                                    //     Set count flag
    } else {                                                                                            //   InA on and InB off:
      diPinA();                                                                                         //     Turn off pull-up current for inA
      if ((!lrflg)&&cntflg) position++;                                                                 //     lrflg indicates if oposite position, cntflg to count up/dn
      lrflg = true;                                                                                     //     Set lrflag to true for this position (SW1 on, SW0 off)
      cntflg = false;                                                                                   //     Clear count flag
    }
  } else {
    if (inB) {                                                                                          //   InA off and InB on:
      diPinB();                                                                                         //     Turn off pull-up current for inA
      if (lrflg&&cntflg) position--;                                                                    //     lrflg indicates if oposite position, cntflg to count up/dn
      lrflg = false;                                                                                    //     Set lrflag to false for this position (SW1 off, SW0 on)
      cntflg = false;                                                                                   //     Clear count flag
    } else {                                                                                            //   InA off and InB off:
    }                                                                                                   //     Do nothing. cntflg is false, unless bounce on common pin
  }                                                                                                     //       (Do nothing: Bounceing on common pin is handled)
  sei();
}
