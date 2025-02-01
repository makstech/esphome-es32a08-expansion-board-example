#pragma once
#include "esphome.h"
#include "esphome/components/sn74hc595/sn74hc595.h"

namespace esphome {
namespace es32a08 {

/// This component implements a 4-digit 7-segment multiplexed display
/// driven by a SN74HC595 shift register. It uses a text sensor as the
/// “message” source and automatically multiplexes through the four digits.
/// The wiring is assumed to be fixed:
///   - Outputs 0-7: segments A, B, C, D, E, F, G, and DP respectively
///   - Outputs 8-11: digit select lines for digit 1, 2, 3, and 4.
class ES32A08Display : public PollingComponent {
 public:
  /// The update interval is 25ms (about 40Hz per digit refresh)
  ES32A08Display() : PollingComponent(25), current_digit_(0) {}

  void set_text_sensor(text_sensor::TextSensor *ts) { this->text_sensor = ts; }
  void set_shift_register(sn74hc595::SN74HC595 *sr) { this->shift_register = sr; }

  void setup() override {
    if (text_sensor == nullptr || shift_register == nullptr) {
      ESP_LOGE("es32a08_display", "Both text_sensor and shift_register must be set!");
    }
  }

  /// Called every 25ms; multiplexes one digit at a time.
  void update() override {
    if (text_sensor == nullptr || shift_register == nullptr)
      return;

    // Get the current text from the sensor.
    std::string text = text_sensor->state;
    // If text is shorter than 4 characters, pad with spaces at the beginning.
    while (text.length() < 4) {
      text = " " + text;
    }
    int len = text.length();

    // Clear all outputs (both segments and digit enables).
    clear_display();

    // Determine the character for the current digit.
    char c = text[current_digit_];
    bool dot = false;
    // Check if the next character is a dot. (This simple logic treats a dot
    // after a character as part of that digit.)
    if (current_digit_ < len - 1 && text[current_digit_ + 1] == '.') {
      dot = true;
    }

    // Calculate the mapping index. If the character is below ASCII 32, use 0.
    uint8_t ch_index = (c < 32) ? 0 : (c - 32);
    uint8_t segments = char_map_[ch_index];
    // If a dot is requested, set the DP bit (bit 7).
    if (dot) {
      segments |= 0b10000000;
    }

    // Enable the appropriate digit select output.
    set_digit_select(current_digit_, true);

    // Set the segment outputs.
    // Mapping (bit positions in 'segments'):
    //   Bit 6 -> A (pin 0)
    //   Bit 5 -> B (pin 1)
    //   Bit 4 -> C (pin 2)
    //   Bit 3 -> D (pin 3)
    //   Bit 2 -> E (pin 4)
    //   Bit 1 -> F (pin 5)
    //   Bit 0 -> G (pin 6)
    //   Bit 7 -> DP (pin 7)
    shift_register->set_pin(0, segments & 0b01000000);
    shift_register->set_pin(1, segments & 0b00100000);
    shift_register->set_pin(2, segments & 0b00010000);
    shift_register->set_pin(3, segments & 0b00001000);
    shift_register->set_pin(4, segments & 0b00000100);
    shift_register->set_pin(5, segments & 0b00000010);
    shift_register->set_pin(6, segments & 0b00000001);
    shift_register->set_pin(7, segments & 0b10000000);

    // Tell the shift register to update its outputs.
    shift_register->commit();

    // Cycle to the next digit (0 to 3).
    current_digit_ = (current_digit_ + 1) % 4;
  }

 protected:
  uint8_t current_digit_;
  text_sensor::TextSensor *text_sensor = nullptr;
  sn74hc595::SN74HC595 *shift_register = nullptr;

  /// Character to segment mapping for a 7-segment display.
  /// The mapping starts at ASCII 32 (space) and supports 95 characters.
  const uint8_t char_map_[95] = {
    0b00000000, // ' ' (0x20)
    0b10110000, // '!' (0x21)
    0b00100010, // '"' (0x22)
    0b00000000, // '#' (0x23)
    0b00000000, // '$' (0x24)
    0b01001001, // '%' (0x25)
    0b00000000, // '&' (0x26)
    0b00000010, // ''' (0x27)
    0b01001110, // '(' (0x28)
    0b01111000, // ')' (0x29)
    0b01000000, // '*' (0x2A)
    0b00000000, // '+' (0x2B)
    0b00010000, // ',' (0x2C)
    0b00000001, // '-' (0x2D)
    0b10000000, // '.' (0x2E)
    0b00000000, // '/' (0x2F)
    0b01111110, // '0' (0x30)
    0b00110000, // '1' (0x31)
    0b01101101, // '2' (0x32)
    0b01111001, // '3' (0x33)
    0b00110011, // '4' (0x34)
    0b01011011, // '5' (0x35)
    0b01011111, // '6' (0x36)
    0b01110000, // '7' (0x37)
    0b01111111, // '8' (0x38)
    0b01111011, // '9' (0x39)
    0b01001000, // ':' (0x3A)
    0b01011000, // ';' (0x3B)
    0b00000000, // '<' (0x3C)
    0b00001001, // '=' (0x3D)
    0b00000000, // '>' (0x3E)
    0b01100101, // '?' (0x3F)
    0b01101111, // '@' (0x40)
    0b01110111, // 'A' (0x41)
    0b00011111, // 'B' (0x42)
    0b01001110, // 'C' (0x43)
    0b00111101, // 'D' (0x44)
    0b01001111, // 'E' (0x45)
    0b01000111, // 'F' (0x46)
    0b01011110, // 'G' (0x47)
    0b00110111, // 'H' (0x48)
    0b00110000, // 'I' (0x49)
    0b00111100, // 'J' (0x4A)
    0b00000000, // 'K' (0x4B)
    0b00001110, // 'L' (0x4C)
    0b00000000, // 'M' (0x4D)
    0b00010101, // 'N' (0x4E)
    0b01111110, // 'O' (0x4F)
    0b01100111, // 'P' (0x50)
    0b11111110, // 'Q' (0x51)
    0b00000101, // 'R' (0x52)
    0b01011011, // 'S' (0x53)
    0b00000111, // 'T' (0x54)
    0b00111110, // 'U' (0x55)
    0b00111110, // 'V' (0x56)
    0b00111111, // 'W' (0x57)
    0b00000000, // 'X' (0x58)
    0b00100111, // 'Y' (0x59)
    0b01101101, // 'Z' (0x5A)
    0b01001110, // '[' (0x5B)
    0b00000000, // '\' (0x5C)
    0b01111000, // ']' (0x5D)
    0b00000000, // '^' (0x5E)
    0b00001000, // '_' (0x5F)
    0b00100000, // '`' (0x60)
    0b01110111, // 'a' (0x61)
    0b00011111, // 'b' (0x62)
    0b00001101, // 'c' (0x63)
    0b00111101, // 'd' (0x64)
    0b01001111, // 'e' (0x65)
    0b01000111, // 'f' (0x66)
    0b01011110, // 'g' (0x67)
    0b00010111, // 'h' (0x68)
    0b00010000, // 'i' (0x69)
    0b00111100, // 'j' (0x6A)
    0b00000000, // 'k' (0x6B)
    0b00001110, // 'l' (0x6C)
    0b00000000, // 'm' (0x6D)
    0b00010101, // 'n' (0x6E)
    0b00011101, // 'o' (0x6F)
    0b01100111, // 'p' (0x70)
    0b00000000, // 'q' (0x71)
    0b00000101, // 'r' (0x72)
    0b01011011, // 's' (0x73)
    0b00000111, // 't' (0x74)
    0b00011100, // 'u' (0x75)
    0b00011100, // 'v' (0x76)
    0b00000000, // 'w' (0x77)
    0b00000000, // 'x' (0x78)
    0b00100111, // 'y' (0x79)
    0b00000000, // 'z' (0x7A)
    0b00110001, // '{' (0x7B)
    0b00000110, // '|' (0x7C)
    0b00000111, // '}' (0x7D)
    0b01100011  // '~' (0x7E, often used as degree symbol)
  };

  /// Clears (turns off) all segment outputs (pins 0–7) and digit selects (pins 8–11).
  void clear_display() {
    for (uint8_t i = 0; i < 12; i++) {
      shift_register->set_pin(i, false);
    }
    shift_register->commit();
  }

  /// Activates the digit select line corresponding to the given digit.
  /// Digit 0 maps to pin 8, 1 to pin 9, etc.
  void set_digit_select(uint8_t digit, bool state) {
    uint8_t pin = 8 + digit;
    shift_register->set_pin(pin, state);
  }
};

}  // namespace es32a08
}  // namespace esphome
