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
  typedef std::vector<fixed_string> fixed_string_vector;

  struct xyz {
    const char* x;
    const char* y;
    const char* z;
  };

  typedef xyz vertex;
  typedef std::vector<vertex> vertex_vector;

  // struct accessor {
  //   const std::string& data;
  //   const std::size_t element_size = 0;
  //   const std::size_t offset = 0;

  //   void set(std::size_t, );
  // };

  struct mesh {
    mesh(std::size_t element_count = 1);

    std::string mesh_data;

    // accessor vertices { mesh_data, sizeof(xyz) };
  };
}