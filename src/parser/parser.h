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
        sequence_triangle,
        sequence_texture_coord,
        datatype_float,
        datatype_int,
        whitespace,
        number_of_tokens
    };

    enum element_type {
      position,
      normal,
      triangle,
      texturecoord0,
      number_of_element_types
    };

    typedef std::vector<std::size_t> size_t_vector;

    class mesh_lexer : public lexer::generic_lexer {
    public:
      mesh_lexer() :
        lexer::generic_lexer(regexs),
        regexs(token_type::number_of_tokens) {
          this->regexs[token_type::whitespace] = std::regex("\\s+");
          this->regexs[token_type::datatype_float] = std::regex("[-+]?(\\d*[.])(\\d*)", std::regex::ECMAScript);
          this->regexs[token_type::datatype_int] = std::regex("\\d+", std::regex::ECMAScript);
          this->regexs[token_type::sequence_position] = std::regex("\\[position\\]");
          this->regexs[token_type::sequence_normal] = std::regex("\\[normal\\]");
          this->regexs[token_type::sequence_triangle] = std::regex("\\[triangle\\]");
          this->regexs[token_type::sequence_texture_coord] = std::regex("\\[texturecoord\\]");
        }

      lexer::regex_vector regexs;
    };

    struct parser_result
    {
      std::vector<std::string> errors{};
      bool valid {false};

      std::array<int, number_of_element_types> sequence_count { 0 };

      std::string parsed_mesh {""};
      size_t_vector parsed_index_buffer {};
    };

    // Mesh               -> Sequences .
    // Sequences          -> SEQ_POSITION Vec3List Sequences .
    // Sequences          -> SEQ_NORMAL Vec3List Sequences .
    // Sequences          -> SEQ_TEXTURE_COORD Vec2List Sequences .
    // Sequences          -> SEQ_TRIANGLE TriList Sequences .
    // Sequences          -> .
    // Vec2List           -> Vec2 Vec2List .
    // Vec2List           -> .
    // Vec3List           -> Vec3 Vec3List .
    // Vec3List           -> .
    // TriList            -> INT INT INT TriList .
    // TriList            -> .
    // Vec3               -> FLOAT FLOAT FLOAT .
    // Vec2               -> FLOAT FLOAT .
    parser_result  parse(const std::string& input);

  }
}