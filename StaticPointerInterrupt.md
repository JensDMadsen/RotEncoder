<!-- Markdeep: --><script src="https://casual-effects.com/markdeep/latest/markdeep.min.js"></script>
# Documentation: Handling Interrupts in C++ Using a Static Pointer Method

## Introduction

Interrupts are essential for efficient event-driven programming in embedded systems. In C++, using a static function combined with a pointer to a class instance offers a powerful, reusable, and safe way to handle interrupts. This document outlines two methods for managing interrupts: the traditional method used in libraries like **RotEncoder**, and a more advanced template-based version that can handle multiple interrupts simultaneously.

The template version offers the additional benefit of managing multiple interrupts through a flexible and reusable system, making it ideal for more complex applications where multiple interrupt sources must be handled by the same instance.


## The Static Pointer Method

The method consists of the following key components:

-   **Static ISR**: A static function that acts as the interrupt service routine (ISR), which can be attached to hardware interrupts.
-   **Pointer to Class Instance**: A static pointer that points to the current instance of the class handling the interrupt.
-   **Instance-specific Interrupt Function**: A non-static function within the class that processes interrupts for that specific instance.
-   **Atomic Block**: On 8/16-bit systems, an atomic block is necessary when assigning the class pointer in an interrupt context to avoid race conditions. On 32-bit systems, assignments are typically atomic, so this precaution is not required.


## Advantages of the Method

-   **Flexibility**: The ISR is static and can be reused across multiple instances and projects.
-   **Safety**: No interrupt is processed unless the pointer is valid, reducing the risk of undefined behavior.
-   **Reusability**: The approach is reusable across different embedded projects, making it both scalable and modular.


## When to Use Atomic Blocks

When working with **8/16-bit microcontrollers** (such as AVR-based Arduinos), assigning or modifying a multi-byte variable (such as a pointer) can be interrupted mid-assignment, leading to corrupt or inconsistent data. Therefore, we use **atomic blocks** to ensure that pointer assignments happen without interruption.

However, in **32-bit systems** (such as ARM-based microcontrollers), assignments of pointer values are typically atomic by nature, and thus the use of atomic blocks is not necessary for such operations.


## Example Code

Here's how the interrupt handling method is implemented:

```cpp
#include <Arduino.h>
#include <util/atomic.h>  // Required for 8/16-bit systems to ensure atomic operations

class ISRHandler {
protected:
  // Function that will handle the specific interrupt for the instance
  virtual void intr() = 0;  // Pure virtual function, must be implemented by derived class
public:
  ~ISRHandler() {  // Destructor
    end();  // Ensure that interrupts are properly detached when the object is destroyed
  }

  bool begin() {  // Starts interrupt handling, returns true if successful
    // Method to begin interrupt handling by setting up the static pointer and attaching the ISR
    if (!intHandle) {  // Only start if the handler is not already set
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Use atomic block for 8/16-bit systems. It ensures that interrupts are fully disabled during
        // pointer assignment, providing safety in case previous detachInterrupt() wasn't called or failed.
        intHandle = this;  // Set the static pointer to the current instance
      }
      attachInterrupt(digitalPinToInterrupt(2), isr, CHANGE);  // Attach the static ISR to pin 2 (for example)
      return true;
    }
    return false;  // Return false if intHandle is already set
  }

  bool end() {  // Stops interrupt handling, returns true if successful
    // Method to stop interrupt handling by detaching the ISR and resetting the static pointer
    if (intHandle == this) {  // Only stop if intHandle points to this instance
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Use atomic block for 8/16-bit systems. If detachInterrupt() fails, this atomic block ensures
        // intHandle is safely set to nullptr, preventing any invalid interrupt calls. Since detachInterrupt()
        // doesn't return success/failure code, handling this carefully is important.
        intHandle = nullptr;  // Reset the static pointer
      }
      detachInterrupt(digitalPinToInterrupt(2));  // Detach the ISR
      return true;
    }
    return false;  // Return false if intHandle doesn't match this instance
  }

private:
  typedef ISRHandler* volatile IntHandleT;  // Define a static pointer type for interrupt handling
  static IntHandleT intHandle;  // Static pointer to the current instance for interrupt handling

  // Static interrupt service routine (ISR) executes intr() function in this class
  static void isr() {  // Static interrupt service routine (ISR)
    if (intHandle) {
      intHandle->intr();  // Call the instance-specific interrupt handler
    }
  }
};

// Initialize the static pointer to nullptr.
// This ensures that no interrupts are processed until 'begin()' is called and
// the pointer is set to the current instance of the ISRHandler class.
// Setting it to nullptr provides safety in case the ISR is triggered before
// the interrupt handler is properly initialized.
ISRHandler::IntHandleT ISRHandler::intHandle = nullptr;
```


### Code Explanation

-   **`static IntHandleT intHandle`**: This is a static pointer to the current class instance that handles the interrupt. It allows the static ISR to call the instance-specific interrupt handler.

-   **`virtual void intr()`**: This is a pure virtual function that must be implemented in any class that inherits from **ISRHandler**. This function contains the logic specific to the interrupt event (e.g., updating a position counter).

-   **`begin()` and `end()`**: These methods attach and detach the ISR from a specific interrupt pin. In **8/16-bit systems**, the atomic block is used to ensure the assignment of the interrupt handler is not interrupted.

-   **`static void isr()`**: This is the static interrupt service routine that is triggered by the interrupt. It checks if the static pointer is valid before calling the instance-specific interrupt handler.


## Template-Based Interrupt Handler

The template-based version of the `ISRHandler` class allows you to handle multiple interrupts through the use of _parameter packs_. Parameter packs in C++ are a feature of templates that allow functions or classes to accept an arbitrary number of template parameters. In this case, the class can accept and manage multiple interrupt numbers and modes at once.

Here's how the template-based version works:

-   You provide a list of interrupt numbers and mode (e.g., `RISING`, `FALLING`, etc.)
-   The class uses recursive template functions to attach or detach interrupts for each number in the list, ensuring that all specified interrupts are managed by the same instance of the class.

The key advantage of this approach is flexibility. You can attach and manage multiple interrupts in one instance without duplicating code, which is especially useful in complex systems with multiple interrupt sources.


### Template Code

```cpp
// ISRHandler.h

#ifndef ISRHANDLER_H
#define ISRHANDLER_H

#include <Arduino.h>
#include <util/atomic.h>  // Required for 8/16-bit systems to ensure atomic operations

// USE:
//   // intr(uint8_t interruptNum) triggered on INT=2 RISING and INT=3 FALLING //
//   class MyInterruptHandler : public ISRHandler<2,RISING, 3,FALLING> {
//   protected:
//     void intr(uint8_t interruptNum) override {  // Handle interrupts here, both INT2 and INT3 calls same intr()
//       // code for interrupts here //
//     }
//   };
//
//   MyInterruptHandler handler;  // Interrupt handler
//
//   void setup() {
//     handler.begin();  // Start handling interrupts on pins 2 rising and 3 falling
//   }

template <uint8_t... InterruptAndModePairs>  // Use template with interrupt and mode pairs for each interrupt
class ISRHandler {
protected:  // Function that will handle the specific interrupt for the instance
  virtual void intr(uint8_t interruptNum) = 0;  // Pure virtual function, must be implemented by derived class

public:
  ~ISRHandler() {  // Destructor ensures that interrupts are properly stopped and detached
    end();  // uses end() to stop and detach all interrupts
  }

  // Begin interrupt handling by setting up the static handle pointer and attaching the ISR
  bool begin() {  // Starts interrupt handling for all interrupts in the template parameter pack
    if (!intHandle) {  // Only start if the handler is not already set
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Use atomic block for 8/16-bit systems. It ensures that interrupts are fully disabled during
        // pointer assignment, providing safety in case previous detachInterrupt() wasn't called or failed.
        intHandle = this;  // Set the static pointer to the current instance
      }
      // Attach ISR for each interrupt and mode pair in the parameter pack
      attachInterrupts<InterruptAndModePairs...>();  // Recursively attach interrupts to static ISR
      return true;  // Return true if success
    }
    return false;  // Return false if intHandle is already set
  }

  // Ending interrupt handling by detach all interrupts and resetting the handler to nullptr
  bool end() {  // Stops interrupt handling for all interrupts in the template parameter pack
    if (intHandle == this) {  // Only stop if intHandle points to this instance
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Use atomic block for 8/16-bit systems. If detachInterrupt() fails, this atomic block ensures
        // intHandle is safely set to nullptr, preventing any invalid interrupt calls. Since detachInterrupt()
        // doesn't return success/failure code, handling this carefully is important.
        intHandle = nullptr;  // Reset the static pointer
      }  // Detach ISR for each interrupt and mode pair in the parameter pack
      detachInterrupts<InterruptAndModePairs...>();  // Recursively detach interrupts
      return true;  // Return true if success
    }
    return false;  // Return false if intHandle doesn't match this instance
  }

private:
  typedef ISRHandler* volatile IntHandleT;  // Define a static pointer type for interrupt handling
  static IntHandleT intHandle;  // Static pointer to the current instance for interrupt handling

  // Helper function to attach interrupts recursively
  template <uint8_t InterruptNum, int Mode, uint8_t... Rest>
  void attachInterrupts() {
    attachInterrupt(  // Attach ISR for the current interrupt and mode
      digitalPinToInterrupt(InterruptNum),  // Attach current InterruptNum
      []() {  // Lambda function as static interrupt service routine (ISR)
        if (intHandle) {  // Make sure instance handle exists
          intHandle->intr(InterruptNum);  // Call the instance interrupt trough handler if exists
        }
      },
      Mode);  // Attach Mode
    if constexpr (sizeof...(Rest) > 0) {  // Check if there are more interrupt and mode pairs in the parameter pack
      attachInterrupts<Rest...>();  // Recursively attach the remaining interrupts and modes
    }
  }

  // Helper function to detach interrupts recursively
  template <uint8_t InterruptNum, int Mode, uint8_t... Rest>
  void detachInterrupts() {
    detachInterrupt(digitalPinToInterrupt(InterruptNum));  // Detach ISR for the current interrupt
    if constexpr (sizeof...(Rest) > 0) {  // Check if there are more interrupt and mode pairs in the parameter pack
      detachInterrupts<Rest...>();  // Recursively detach the remaining interrupts
    }
  }
};

// Initialize the static pointer to nullptr (must be in the header file)
template<uint8_t... InterruptAndModePairs>
typename ISRHandler<InterruptAndModePairs...>::IntHandleT ISRHandler<InterruptAndModePairs...>::intHandle = nullptr;

#endif  // ISRHANDLER_H
```


### Example: Using ISRHandler with Multiple Interrupts

Here is an example of how to use the template-based version of `ISRHandler` to manage two interrupts (pins 2 and 3) simultaneously:

```cpp
#include "ISRHandler.h"  // Include headers

auto& Stdout = Serial;  // Uses Serial as Stdout

// intr() triggered on INT=2 RISING and INT=3 FALLING
class MyInterruptHandler : public ISRHandler<2,RISING, 3,FALLING> {
protected:
  // Handle interrupts here, both INT2 and INT3 calls same intr()
  void intr() override {
    Stdout.println("Interrupt triggered!");
  }
};

MyInterruptHandler handler;  // Interrupt handler

void setup() {
  Stdout.begin(115200);  // Setup 115200 baud
  while (!Serial);  // Wait on Serial
  Stdout.println("Interrupt handler test");  // Check Serial print ok

  // Set the pins as input with pull-up resistors
  pinMode(2, INPUT_PULLUP);  // Set pin 2 as input with pull-up
  pinMode(3, INPUT_PULLUP);  // Set pin 3 as input with pull-up

  handler.begin();  // Start handling interrupts on pins 2 and 3
}

void loop() {
  // Main code logic
}
```


## Conclusion

The static pointer method, both in its traditional and template-based versions, provides a robust, flexible way to handle interrupts in C++. The template version further allows for the handling of multiple interrupts with minimal duplication of code, ensuring efficient and safe interrupt management across different hardware platforms.


## Contact

If you have any questions, feedback, or need further assistance, feel free to [Contact Me](https://www.arduino.one/contact.html) through my online form.
