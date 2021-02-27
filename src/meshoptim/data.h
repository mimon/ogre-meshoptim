#pragma once
#include <array>
#include <vector>
#include <string>

namespace meshoptim
{
  struct xyz {
    std::string_view x;
    std::string_view y;
    std::string_view z;
  };

  struct xy {
    std::string_view x;
    std::string_view y;
    std::string_view z;
  };

  typedef std::vector<std::string> string_vector;
  typedef std::vector<std::string::iterator> string_iter_vector;
}
