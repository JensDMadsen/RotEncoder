

# Why Choose this Rotary Encoder Library

The **RotEncoder** library offers several key advantages over other rotary encoder libraries, making it a great choice for both hobbyists and professionals. Here are the main reasons to choose this library:

## 1. Low Power Consumption

The **RotEncoder** library is optimized for low power consumption, making it ideal for battery-powered applications. Rotary encoders often settle in a position where one of the contacts is closed, even when the knob is not being turned. Without proper management, this can lead to constant static current draw. The **RotEncoder** library avoids this by dynamically controlling the pull-up resistors, ensuring that no static current is drawn unless **both the A and B contacts** of the rotary encoder are closed simultaneously.

-   **Dynamic power management**: By dynamically enabling and disabling the pull-up resistors, the library ensures that only one pin is active at a time, unless both contacts are closed.
-   **Efficient power usage**: This approach minimizes power consumption by only drawing current during **state transitions** or when both contacts are closed, helping your device run longer on a single charge.

## 2. High Accuracy and Responsiveness

With support for hardware interrupts, the library guarantees fast and precise position tracking, even when the rotary encoder is turned at high speeds. The interrupt-driven design ensures that every pulse is captured accurately, preventing missed counts or position errors.

## 3. Easy Customization

While the library works out of the box with default pins (2 and 3 on the Arduino Nano), it also offers flexibility for advanced users who want to use different pin configurations. The **RotEncoderPins** template class makes it easy to specify custom pins during compilation, ensuring seamless integration into a wide range of projects.

## 4. Reliable and Safe Operation

The library is designed with safety and robustness in mind. It actively disables unused pins to reduce static current draw and to ensure that no floating inputs cause unexpected behavior. Furthermore, it ensures that pins are always in a well-defined state, whether in pull-up mode or actively pulled low.

## 5. Modular Design and Extensibility

The library is structured to be modular, making it easy to extend and customize. You can override functions for pin reading or mode control, giving you full flexibility to adapt the library to your specific hardware requirements.

## Summary of Key Features:

-   **Low power consumption** with dynamic control of pull-up resistors.
-   **Interrupt-driven** for fast and accurate position tracking.
-   **Customizable** pin configurations via template classes.
-   **Safe operation** by avoiding static current draw and ensuring pins are in defined states.
-   **Modular and extensible** design, allowing easy adaptation to different use cases.).
-   **You get a rotary encoder that works** without the typical [pitfalls](#common-mistakes-when-using-rotary-encoders) seen in most implementations.

---

# How to Use the RotEncoder Library

The **RotEncoder** library is designed to efficiently handle rotary encoders while minimizing power consumption. Below are detailed steps and instructions for using the library with the default pin configuration or customizing it for specific pin setups.

## 1. Installing the Library

To begin using the **RotEncoder** library, download and install the library in your Arduino IDE. You can place the library in the `libraries` folder in your Arduino installation directory, or use the **Library Manager** in the IDE to import the library.

## 2. Including the Library in Your Sketch

After installation, include the library at the top of your Arduino sketch:

```cpp
#include <RotEncoder.h>
```
## 3. Default Pin Configuration and Power Saving

By default, the **RotEncoder** class uses digital pins 2 and 3 on the Arduino Nano (or similar boards) for rotary encoder inputs. These pins support interrupts, which are used by the library to track the encoder's movement.

### Rotary Encoder Pin Connections to Arduino Nano:
```
    Rotary Encoder       Arduino Nano
    --------------       -------------
       [ Pin A ]  ------- Pin 2 (Digital Input)
       [ Pin B ]  ------- Pin 3 (Digital Input)
       [ GND   ]  ------- GND
```
### Important Notes:

-   **Pin A** and **Pin B** are configured as **input pins with pull-up resistors** when the encoder is being read.
-   The library **dynamically manages the pull-up resistors** to minimize power consumption. **No static current is drawn** except when both the A and B contacts of the rotary encoder are active simultaneously.
-   Depending on the state of the encoder, one pin may be actively pulled low, while the other is in **input_pullup** mode. This dynamic control ensures the encoder can be read while minimizing unnecessary power consumption.
-   **Do not connect encoders that actively drive the pins high**, as this could conflict with the library's power-saving features, leading to increased power consumption or improper operation.
-   If your encoder drives the pins high, you can modify or override the relevant functions to handle these cases safely.

### Example Using Default Pins:

```cpp
#include <RotEncoder.h>

RotEncoder encoder;  // Uses default pins 2 (PinA) and 3 (PinB)

void setup() {
    Serial.begin(115200);
    encoder.begin();  // Start the encoder
}

void loop() {
    long position = encoder.getPosition();  // Get the current encoder position

    // Print the position if it changes
    static long lastPosition = 0;
    if (position != lastPosition) {
        lastPosition = position;
        Serial.print("Position: ");
        Serial.println(position);
    }
}
```
## 4. Customizing Pin Configuration

If you need to use pins other than the default pins 2 and 3, the library provides a template class, `RotEncoderPins`, which allows you to specify the pin numbers during compilation.

### Custom Pin Example:

```cpp
#include <RotEncoder.h>

// Use pins 5 and 6 instead of 2 and 3
RotEncoderPins<5, 6> encoder;

void setup() {
    Serial.begin(115200);
    encoder.begin();  // Start the encoder
}

void loop() {
    long position = encoder.getPosition();  // Get the current encoder position
    Serial.println(position);
}
```
In this example, the rotary encoder is connected to pins 5 and 6, but the rest of the code remains the same.

## 5. Stopping the Encoder

When you're done using the encoder, or if you want to turn it off temporarily, you can stop it by calling the `end()` method:
```cpp
encoder.end();  // Stops the encoder and disables interrupts
```
The `end()` method is also automatically called in the destructor, so if the object goes out of scope, it will stop safely.

## 6. Interrupts and Advanced Usage

The **RotEncoder** library uses interrupts to ensure high accuracy and responsiveness, even at high speeds. When setting up the encoder, ensure that the pins used for **Pin A** and **Pin B** support hardware interrupts on your microcontroller.

### Important Notes for AVR Microcontrollers:

-   On **AVR microcontrollers** (such as the one used in Arduino boards like the Nano and Uno), **port change interrupts** can have issues if **Pin A** and **Pin B** are on the same port and share the same interrupt.
-   When both pins are on the same port, using the same port change interrupt can result in **missed interrupts**, which may cause the encoder to lose track of its position.
-   To avoid this issue, it's recommended to use **pins on different ports** if you are relying on port change interrupts, as each port will have its own interrupt handler.
-   If port change interrupts are not necessary, using dedicated **external interrupts** for Pin A and Pin B (on pins such as 2 and 3 on the Arduino Nano) will avoid this problem entirely.

You can further customize the behavior of the library by overriding functions for reading the pins or controlling the pin modes if your setup requires different handling.


---

# Reading and Decoding of Rotary Encoders

Rotary encoders are widely used for input in various devices, including measurement equipment, microwaves, and even development kits like Xilinx starter kits. However, many descriptions and code examples found online often oversimplify how rotary encoders function, leading to incorrect or unreliable results. This guide aims to clarify how rotary encoders work and how to implement a robust reading algorithm.

## Understanding the Basics

A rotary encoder is a mechanical device with two output signals (A and B) that are activated in sequence as the knob rotates. The relative timing between these two signals (also known as quadrature output) allows us to determine the direction of rotation. Additionally, many rotary encoders include a spring that pulls the knob into a detented position where both contacts are open.

### Simplified Schematic:
```
       +5V ─┬───┐
           ┌┴┐ ┌┴┐
 ROT-ENC   │ │ │ │ 2x10K       Gray / Quadrature output    Up→
┌───────┐  └┬┘ └┬┘              |           |           |
│      A├───┴───│───> _OutA    ─|──┐   ┌────|──┐   ┌────|──┐   ┌────
│   O   │       │               |  └───┘    |  └───┘    |  └───┘
│  com B├───────┴───> _OutB    ─|────┐   ┌──|────┐   ┌──|────┐   ┌──
└───┬───┘                       |    └───┘  |    └───┘  |    └───┘
    │                           |           |           |  ←Dn
   Gnd
```
The encoder generates a specific sequence depending on the direction of rotation. For example:

| State | _OutA | _OutB |
|-------|-------|-------|
| 1     | 0     | 0     |
| 2     | 1     | 0     |
| 3     | 1     | 1     |
| 4     | 0     | 1     |

By interpreting these transitions correctly, we can determine whether the knob is being rotated clockwise or counterclockwise. Here is a visual representation of how the A and B signals interact during rotation:

### Signal Timing Diagram:
```
                   Gray / Quadrature output    Up→
        |           |           |           |           |           |
_OutA: ─|──┐   ┌────|──┐   ┌────|──┐   ┌────|──┐   ┌────|──┐   ┌────|─
        |  └───┘    |  └───┘    |  └───┘    |  └───┘    |  └───┘    | 
_OutB: ─|────┐   ┌──|────┐   ┌──|────┐   ┌──|────┐   ┌──|────┐   ┌──|─
        |    └───┘  |    └───┘  |    └───┘  |    └───┘  |    └───┘  | 
        |           |           |           |  ←Dn      |           |
```
When the knob is not in motion, the spring typically pulls the knob to a position where both contacts are open (_OutA = 1, _OutB = 1).

## Spring Actuation Force and Rotary Encoder Behavior

A rotary encoder typically contains two contacts (A and B), which are alternately activated depending on the direction of rotation. These contacts generate a quadrature signal that allows the determination of both the direction and count of rotation.

Inside the rotary encoder, a spring mechanism creates a tactile "click" sensation as the knob is rotated. This spring helps the knob settle into defined, stable positions, known as detents, which correspond to specific electrical states of the encoder. As you turn the knob, the spring provides resistance, increasing and decreasing in strength depending on the position of the contacts. This mechanism ensures that the knob does not accidentally drift between positions and gives the user clear feedback with each incremental movement.

The spring is designed to pull the knob to a neutral position, where both contacts are open (_OutA = 1, _OutB = 1). However, as the knob is turned, resistance is felt, which varies depending on how the contacts are engaged.

### Spring Actuation Force Diagram:
```
        |                                     |                                     |                                     |                                     |
_OutA: ─|───────┐              ┌──────────────|───────┐              ┌──────────────|───────┐              ┌──────────────|───────┐              ┌──────────────|─
        |       └──────────────┘              |       └──────────────┘              |       └──────────────┘              |       └──────────────┘              |
_OutB: ─|──────────────┐              ┌───────|──────────────┐              ┌───────|──────────────┐              ┌───────|──────────────┐              ┌───────|─
        |              └──────────────┘       |              └──────────────┘       |              └──────────────┘       |              └──────────────┘       |
        |                                     |                                     |                                     |                                     |
Force:  | Loose | Weak | Tight | Weak | Loose | Loose | Weak | Tight | Weak | Loose | Loose | Weak | Tight | Weak | Loose | Loose | Weak | Tight | Weak | Loose |
```

### Explaining the Spring Force

-   **Loose (Low Resistance)**: When the knob is in a position where both contacts are open, the spring exerts minimal force, and there is little resistance. This is the default rest position of the rotary encoder.
    
-   **Weak (Moderate Resistance)**: As the knob starts to turn, one contact closes (either A or B). The spring force increases slightly, providing a moderate level of resistance.
    
-   **Tight (High Resistance)**: When both contacts are closed simultaneously, the spring force is at its maximum. This is where the user feels the most resistance during rotation. This 'tight' position helps prevent accidental movement and ensures that the encoder snaps back to the next position.

----------

## Considerations for Low-Power Designs

Rotary encoders designed for low-power electronics need to account for the mechanical behavior of the spring. In some cases, especially with encoders that have weaker springs (low actuation force), the knob might get stuck in a position where only one contact is closed, leading to a small but continuous current draw. Over time, this can significantly drain a battery in portable or battery-powered devices.

To prevent this, designers can implement strategies such as dynamically controlling pull-up resistors, ensuring that static current draw is minimized when the rotary encoder is not in motion.

For low-power designs, the key is to minimize static current consumption. Here’s an example of a circuit that avoids battery drain when the rotary encoder is idle:

```
Schematic:
                ┌───────┐
               ┌┴┐      │
 ROT-ENC       │ │100K  │
┌───────┐      └┬┘      │
│      A├─────┬─│───┤>O─┴─────> OutA
│   O   │     │ │
│  com B├─────│─┴───┤>O─┬─────> OutB
└───┬───┘    ┌┴┐        │
    │        │ │100K    │
   Gnd       └┬┘        │
              └─────────┘
```

In this design, when both contacts are open, there is no static current, as the resistors are in series with the high-impedance CMOS inputs, resulting in no current flow. Even if one contact remains closed, the corresponding CMOS inverter input is pulled low, and its output goes high. Since the other contact remains open, there is no current flow through the associated resistor.

When one contact is closed, the CMOS inverter on the opposite side drives the other input low, ensuring that there is no voltage difference across the resistors. This means no current flows through the resistors, even when one contact is closed. Only when both contacts are closed simultaneously will current flow, and this current is inversely proportional to the resistance values, meaning higher resistance results in lower current.

In summary, this design ensures that no static current is drawn unless both contacts are closed, and even then, the current is minimal due to the high resistance values. The circuit takes advantage of the high input impedance of CMOS inverters and their rail-to-rail outputs, ensuring that the static current remains negligible during normal operation.


----------

## Important Observations for Rotary Encoder Algorithms

When designing algorithms for reading rotary encoders, it’s essential to account for contact bounce and avoid errors in state transitions. While some designs attempt to use software delays for debouncing, this is not effective for rotary encoders. Instead, ensure that state changes only occur when both contacts (A and B) have valid transitions, and no change should be registered if only one contact bounces.

Additionally, the common contact for the A and B switches in a rotary encoder can cause simultaneous bounce on both A and B signals, leading to problems. This can be mitigated by reading the signals multiple times in quick succession to verify whether a simultaneous change is valid or just noise. If both contacts appear to change state almost at the same time, this is often due to bounce on the common terminal and should be ignored by the state machine.

Some designs fail to account for the scenario where bounce on the common terminal causes a transition from state 00 (both contacts closed) to state 11 (both contacts open) and back to 00. During this transition, intermediate states such as 01 or 10 can appear due to slight delays in the microcontroller’s I/O circuitry, not because of bounce. If a transition from 00 to 11 (and back to 00) briefly passes through 01 or 10, it should not lead to counting, as this is the result of signal propagation delays rather than a valid transition. Bounce on contact A, contact B, or the common terminal should never result in an increment or decrement of the counter, as it is not a valid transition but a result of bounce noise.


----------

## Common Mistakes When Using Rotary Encoders

Many issues with rotary encoders arise from misunderstanding how they work. Here are some of the most common pitfalls:

1.  **Adding Capacitors for Debouncing:** Some implementations suggest adding capacitors to the output to reduce bouncing. While this may reduce electrical noise, it often leads to inaccurate results, especially in sensitive applications. The real problem typically lies in the algorithm, not in the hardware. Capacitors introduce delays, which can cause the encoder to miss or misread transitions during fast rotations.
    
2.  **Using Faulty Algorithms:** Many software implementations count incorrectly when the knob is moved back and forth slightly. For example, on a microwave or measurement device, you may see the counter increment or decrement even if you return to the original position. This happens because the state machine misinterprets partial movements as full steps. For example, if transitions like 1, 2, 3, 2, 1 instead of completing a full sequence (1, 2, 3, 4), the algorithm may still increment or decrement the counter. A well-designed algorithm should only count full transitions, preventing incorrect counting during small back-and-forth movements.
    
3.  **Slack in Counting:** Some algorithms allow a small error range (+1 or -1), which may not be noticeable in some applications but can lead to incorrect readings in precision devices. A proper implementation should eliminate this slack by accurately tracking each quadrature phase.
    
4.  **Bounce on All Contacts:** Rotary encoders typically have three terminals: A, B, and common. Not only do A and B bounce, but the common terminal can also cause simultaneous bouncing on both outputs. The reading algorithm should account for this and only count valid transitions.
    
5.  **Spring Issues and Battery Drain:**  In battery-powered devices, the spring in the rotary encoder may fail to return the knob to the position where both contacts are open. If one contact remains closed, it can result in continuous current draw, draining the battery. To avoid this, it is crucial to dynamically enable or disable pull-up or pull-down resistors based on the state of the rotary encoder, allowing efficient low-power operation.

6.  **Potential Missed Interrupts:** Missed interrupts could lead to errors in detecting the quadrature signals, especially on AVR microcontrollers. For more details, refer to the section [Important Notes for AVR Microcontrollers](#important-notes-for-avr-microcontrollers).

----------

## Best Practices for Rotary Encoder Algorithms

To create a robust algorithm for reading rotary encoders, consider the following:

1.  **Track Full Transitions:** Ensure your code counts only full transitions (from one phase to the next). This means going through all four possible quadrature states before counting a valid movement.
    
2.  **Debouncing in Software:** Avoid using hardware debouncing techniques (like capacitors). Instead, implement a software-based solution that ignores fast, transient changes in the signal.
    
3.  **Handling Bounce on Common Terminal pin:** Design your algorithm to handle potential bounces on both A and B signals, as well as the common terminal. One way to achieve this is by sampling the signals multiple times to detect bounce on common terminal, and to handle transitions right.
    
4.  **Low Power Handling:** If your design involves battery-operated devices, dynamically control pull-up resistors to prevent static current consumption when the encoder is idle.
    
5.  **Quality Components:** Lastly, use rotary encoders with a high cycle life (e.g., 100,000 cycles or more). Lower-quality encoders can wear out quickly and lead to unreliable readings, especially in high-precision or frequently used devices.


----------

## Conclusion

Understanding the mechanics and electrical behavior of rotary encoders is crucial for creating reliable and efficient designs. Whether using Arduino Nano or FPGA, robust algorithms and proper hardware design can eliminate the common pitfalls associated with rotary encoders. This guide, along with the accompanying examples, provides a framework for correctly implementing rotary encoder reading, even in low-power applications.


## Additional Documentation

- [Avoiding initialization lists for pin configuration, which minimizes runtime memory usage and allows for compile-time optimization.](./OverridesAndInitLists.md)
- [Handling interrupts safely and efficiently using a static pointer method.](./StaticPointerInterrupt.md)


## Contact

If you have any questions, feedback, or need further assistance, feel free to [Contact Me](https://www.arduino.one/contact.html) through my online form.
