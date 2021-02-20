#include "tests/catch2.hpp"
#include "parser/parser.h"
#include "meshoptim/process.h"

SCENARIO("A string read from a file contains some vertices data") {
  GIVEN("A string of data") {
    std::string text(
      "[position]\n"
      "123.123 123.123 123.123\n"
    );

    meshoptim::parser::mesh_lexer tokenizer;
    WHEN("Tokenizing the data") {
      tokenizer.tokenize(text);

      THEN("The input has been tokenized") {
        CHECK(tokenizer.error_nodes.size() == 0);
        CHECK(tokenizer.nodes.size() == 8);

        CHECK(tokenizer.nodes[0].ri == meshoptim::parser::token_type::sequence_position);
        CHECK(tokenizer.nodes[1].ri == meshoptim::parser::token_type::whitespace);
        CHECK(tokenizer.nodes[2].ri == meshoptim::parser::token_type::datatype_float);
        CHECK(tokenizer.nodes[3].ri == meshoptim::parser::token_type::whitespace);
        CHECK(tokenizer.nodes[4].ri == meshoptim::parser::token_type::datatype_float);
        CHECK(tokenizer.nodes[5].ri == meshoptim::parser::token_type::whitespace);
        CHECK(tokenizer.nodes[6].ri == meshoptim::parser::token_type::datatype_float);
        CHECK(tokenizer.nodes[7].ri == meshoptim::parser::token_type::whitespace);
      }
    }

    WHEN("Parsing the data") {
      meshoptim::parser::parser_result result = meshoptim::parser::parse(text);

      THEN("The output should consist of 1 element with 3d coordinates") {
        INFO(result.errors.size());
        CHECK(meshoptim::count_elements(result.parsed_mesh) == 1);
        CHECK(meshoptim::get_vertex(result.parsed_mesh, 0)== "123.1230000000000000000000000000123.1230000000000000000000000000123.1230000000000000000000000000");
      }
    }
  }

  GIVEN("A string of data") {
    std::string text(
      "[position]\n"
      "1.1 1.2 1.3\n"
      "2.1 2.2 2.3\n"
      "[normal]\n"
      "3.1 3.2 3.3\n"
      "4.1 4.2 4.3\n"
      "[triangle]\n"
      "0 1 0\n"
      "1 1 1\n"
    );

    WHEN("Parsing the data") {
      meshoptim::parser::parser_result result = meshoptim::parser::parse(text);

      THEN("The output should consist of 2 elements with position and normals") {
        // INFO(result.errors[0]);
        REQUIRE(meshoptim::count_elements(result.parsed_mesh) == 2);
        CHECK(meshoptim::get_vertex(result.parsed_mesh, 1) == "2.1000000000000000000000000000002.2000000000000000000000000000002.300000000000000000000000000000");
        CHECK(meshoptim::get_normal(result.parsed_mesh, 1) == "4.1000000000000000000000000000004.2000000000000000000000000000004.300000000000000000000000000000");

        REQUIRE(result.parsed_index_buffer.size() == 6);
        CHECK(result.parsed_index_buffer[0] == 0);
        CHECK(result.parsed_index_buffer[1] == 1);
        CHECK(result.parsed_index_buffer[2] == 0);
        CHECK(result.parsed_index_buffer[3] == 1);
        CHECK(result.parsed_index_buffer[4] == 1);
        CHECK(result.parsed_index_buffer[5] == 1);
      }
    }
  }

  GIVEN("A large string of data") {
    std::string text(
      "[position]\n"
    );

    std::ostringstream repeated;
    std::fill_n(std::ostream_iterator<std::string>(repeated), 100000, std::string("1.1 2.2 3.3\n"));
    text += repeated.str();

    WHEN("Parsing the data") {
      meshoptim::parser::parser_result result = meshoptim::parser::parse(text);

      THEN("The output should consist of 2 elements with position and normals") {
        REQUIRE(meshoptim::count_elements(result.parsed_mesh) == 100000);
      }
    }
  }
}