# Changelog

## [1.0.0] - 2024-10-13
### Initial Release
- Introduced the **RotEncoder** library for Arduino platforms.
- Optimized for low power consumption using dynamic control of pull-up resistors to minimize static current draw.
- Supports rotary encoders with interrupt-driven design for precise and high-speed tracking.
- Default configuration supports AVR platforms with pins 2 and 3 for rotary encoder inputs.
- Customizable pin configurations using template classes for flexibility.
- Includes detailed error handling and protection against contact bounce and missed counts.
