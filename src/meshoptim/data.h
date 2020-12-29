#pragma once
#include <array>
#include <vector>
#include <string>

namespace meshoptim
{
  constexpr  std::size_t string_len = 32;
  using char_array  = std::array<char, string_len>;


  struct fixed_string : public char_array {
    fixed_string() : char_array({ '0' }) {
      this->fill('0');
    }

    explicit fixed_string(const char* chars) :
    fixed_string()  {
      const std::size_t len = strlen(chars);
      memcpy(this->data(), chars, len);
    }

  };

  struct xyz {
    const char* x;
    const char* y;
    const char* z;
  };
}
