#include <RotEncoder.h>

// Setup Standard Output for Serial Print
auto& Stdout = Serial;                                                                                  // Uses Serial as Stdout

RotEncoder rotEncoder;                                                                                  // Make an instance of RotEncoder

void setup() {
  Stdout.begin(115200);                                                                                 // Initialize Serial communication
  while (!Stdout);
  Stdout.println("Rotary Encoder Example");
  if (rotEncoder.begin()) {                                                                             // Start rotary encoder
    Serial.println("Interrupt setup successfully!");
    Stdout.println("Rotary Encoder Running");
  } else {
    Serial.println("Interrupt setup failed.");
  }
}

long position = 0;                                                                                      // Local position counter
void loop() {
  long pos = rotEncoder.getPosition();
  if (position != pos) {
    position = pos;
    Stdout.print("Position = ");
    Stdout.println(pos);  // Print new position if changed
  }
}
