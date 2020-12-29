#pragma once
#include "lexer/lexer.h"
#include <regex>
#include <array>

namespace meshoptim
{
  namespace parser
  {
    enum token_type {
        sequence_position,
        sequence_normal,
        datatype_float,
        whitespace,
        number_of_tokens
    };

    enum element_type {
      position,
      normal,
      number_of_element_types
    };

    class mesh_lexer : public lexer::generic_lexer {
    public:
      mesh_lexer() :
        lexer::generic_lexer(regexs),
        regexs(token_type::number_of_tokens) {
          this->regexs[token_type::whitespace] = std::regex("\\s+");
          this->regexs[token_type::datatype_float] = std::regex("[-+]?(\\d*[.])(\\d*)", std::regex::ECMAScript);
          this->regexs[token_type::sequence_position] = std::regex("\\[position\\]");
          this->regexs[token_type::sequence_normal] = std::regex("\\[normal\\]");
        }

      lexer::regex_vector regexs;
    };

    struct parser_result
    {
      std::vector<std::string> errors{};
      bool valid {false};

      std::array<int, number_of_element_types> sequence_count { 0 };

      std::string parsed_mesh {""};
    };

    // Mesh               -> Sequences .
    // Sequences          -> SEQ_POSITION Vec3List Sequences .
    // Sequences          -> SEQ_NORMAL Vec3List Sequences .
    // Sequences          -> .
    // Vec3List           -> Vec3 Vec3List .
    // Vec3List           -> .
    // Vec3               -> FLOAT FLOAT FLOAT .
    parser_result  parse(const std::string& input);

  }
}