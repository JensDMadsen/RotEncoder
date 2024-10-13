# Documentation: The Use of Templates and Overridden Functions Instead of Initialization Lists

## Background
In this library, **templates** and **overridden functions** are used to specify pin configurations for the **RotEncoder** instead of using **initialization lists**. This design choice has been made to improve both **flexibility** and **efficiency**, and to ensure that pin configurations can be optimized at compile-time.

## Benefits of Using Templates and Overridden Functions:

### 1. Compile-Time Evaluation and Optimization
- When you use **templates** to specify pin numbers, as in **`RotEncoderPins<PinA, PinB>`**, the pin values are determined at compile-time. This means that the compiler can optimize the code and eliminate any potential runtime overhead that could arise from using runtime configurations, such as **initialization lists**.

    **Example**:
    ```cpp
    RotEncoderPins<5, 6> encoder; // PinA is 5, PinB is 6
    ```
    Here, the pin numbers (5 and 6) will be inserted directly into the code during compilation, reducing the need to store these values in RAM and improving execution speed.

### 2. Reduced Memory Usage
- **Initialization lists** typically store values in RAM as runtime variables, which leads to increased memory consumption. By using **templates** and **overridden functions**, the pin values are handled at compile-time, avoiding the need to store them in RAM. This is especially crucial in embedded systems like Arduino, where memory is limited.

    **Code Example**:
    ```cpp
    template <uint8_t PinA, uint8_t PinB>
    class RotEncoderPins : public RotEncoder {
    public:
      inline uint8_t getPinA() const override __attribute__((always_inline)) { return PinA; }
      inline uint8_t getPinB() const override __attribute__((always_inline)) { return PinB; }
    };
    ```
    In this example, the `PinA` and `PinB` values are handled at compile-time and returned by the overridden `getPinA()` and `getPinB()` functions, meaning they do not consume additional memory during runtime.

### 3. Increased Flexibility
- With templates, you can create multiple instances of the **RotEncoder** class, each with its own custom pin configurations, without altering the base code. This allows for great flexibility when working with various hardware setups.

    **Example**:
    ```cpp
    RotEncoderPins<2, 3> encoder1; // Uses pin 2 and 3
    RotEncoderPins<5, 6> encoder2; // Uses pin 5 and 6
    ```
    Each encoder instance can have its own pin configuration, making it easy to work with different hardware devices in a single project.

### 4. Fewer Runtime Errors
- By relying on **templates** and **overridden functions**, you can avoid common runtime errors caused by incorrect pin initialization. Since pin values are checked at compile-time, potential errors are caught early, making the code more reliable and easier to debug.

## Why Not Use Initialization Lists?

### 1. Runtime Overhead
- Initialization lists evaluate and assign values at runtime, which introduces overhead when the program starts. For performance-critical applications, such as embedded systems, this overhead can be avoided by using compile-time constants.

### 2. Higher RAM Usage
- Initialization lists store values like pin numbers in RAM, even if those values are constant throughout the execution. This can unnecessarily consume memory in resource-constrained systems like Arduino.

### 3. Limits Compile-Time Optimization
- By using initialization lists, you limit the compiler's ability to optimize the code because pin numbers are treated as runtime variables. With templates, these constants are evaluated at compile-time, allowing for greater optimization.

## Comparison Between the Two Methods

| Method                   | Compile-Time Optimization | Memory Usage    | Flexibility | Risk of Runtime Errors |
|--------------------------|---------------------------|-----------------|-------------|------------------------|
| **Templates**             | Yes                       | Low             | High        | Low                    |
| **Initialization Lists**  | No                        | High            | Medium      | High                   |

## Examples from the Code

### Example 1: Using `RotEncoder` with Default Pin Configurations

```cpp
RotEncoder encoder; // Uses default pins (PinA = 2, PinB = 3)
```
In this example, the encoder uses the default pin values for PinA (2) and PinB (3). These values are hardcoded into the `RotEncoder` class.

### Example 2: Using `RotEncoderPins` with Custom Pin Configurations

```cpp
RotEncoderPins<5, 6> customEncoder; // Uses pin 5 and 6
```
In this example, the pin numbers are passed via the template parameters and resolved at compile-time. This reduces the memory footprint and ensures efficient execution. The virtual functions `getPinA()` and `getPinB()` are overridden to return the values from the template.

## Conclusion

By using **templates** and **overridden functions** instead of **initialization lists**, the code is optimized for both speed and memory usage. This approach provides a more flexible structure where users can easily change pin configurations via the **`RotEncoderPins`** class, without managing runtime initialization or risking runtime errors.