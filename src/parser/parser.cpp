#include "parser/parser.h"
#include "meshoptim/process.h"
#include <algorithm>
#include "fmt/format.h"
#include <sstream>

namespace meshoptim
{
  namespace parser
  {

    typedef std::vector<lexer::node_vector::const_iterator> const_iterator_vector;
    typedef std::array<const_iterator_vector, element_type::number_of_element_types> sequence_by_element_array;

    lexer::threaded_lexer create_lexer() {
      lexer::regex_vector regexs(token_type::number_of_tokens);
      regexs[token_type::whitespace] = std::regex("[ \n\t]+", std::regex::extended);
      regexs[token_type::datatype_float] = std::regex("[-+]?[0-9]*[.][0-9]*", std::regex::extended | std::regex::optimize);
      regexs[token_type::datatype_int] = std::regex("[0-9]+", std::regex::extended | std::regex::optimize);
      regexs[token_type::sequence_position] = std::regex("\\[position]", std::regex::extended | std::regex::optimize);
      regexs[token_type::sequence_normal] = std::regex("\\[normal]", std::regex::extended | std::regex::optimize);
      regexs[token_type::sequence_triangle] = std::regex("\\[triangle]", std::regex::extended | std::regex::optimize);
      regexs[token_type::sequence_texture_coord] = std::regex("\\[texturecoord]", std::regex::extended | std::regex::optimize);

      return lexer::threaded_lexer(regexs);
    }

    class ll_1_parser {
    public:
      explicit ll_1_parser(const lexer::threaded_lexer& tokenizer, bool parse_data = false) : tokenizer(tokenizer), node_current(tokenizer.nodes.begin()), node_end(tokenizer.nodes.end()), parse_data(parse_data) {

      }

      bool parse_mesh() {
        if (node_current == node_end) {
          return true;
        }

        switch(node_current->ri) {
          case meshoptim::parser::token_type::datatype_float:
          case meshoptim::parser::token_type::datatype_int: {
            this->result.errors.push_back("Unexpected token in parse_mesh()");
            return false;
          } break;
          default: {
            return this->parse_sequences();
          } break;
        }
        return false;
      }

      bool parse_sequences() {
        while (node_current != node_end) {

          switch(node_current->ri) {
            case token_type::sequence_normal:
            case token_type::sequence_position: {
              auto node = this->consume_token();
              if (!this->parse_vec3_list(node)) {
                return false;
              }
            } break;
            case token_type::sequence_texture_coord: {
              auto node = this->consume_token();
              if (!this->parse_vec2_list(node)) {
                return false;
              }
            } break;
            case token_type::sequence_triangle: {
              this->consume_token();
              if (!parse_triangle_list()) {
                return false;
              }
            } break;
            default: {
              this->result.errors.push_back(this->unexpected_token("parse_sequences()", node_current->ri));
              return false;
            } break;
          }
        }
        return true;
      }

      bool parse_vec3_list(lexer::node_vector::const_iterator sequence_node) {
        element_type type {element_type::position};
        if (sequence_node->ri == token_type::sequence_position) {
          type = element_type::position;
        } else if (sequence_node->ri == token_type::sequence_normal) {
          type = element_type::normal;
        } else if (sequence_node->ri == token_type::sequence_triangle) {
          type = element_type::triangle;
        } else {
          this->result.errors.push_back("Unexpected node type encountered in parse_vec3_list()");
          return false;
        }

        while (node_current != node_end) {
          switch(node_current->ri) {
            case token_type::sequence_normal:
            case token_type::sequence_triangle:
            case token_type::sequence_texture_coord:
            case token_type::sequence_position: {
              return true;
            } break;
            case token_type::datatype_float: {
              if(!parse_vec3(type)) {
                return false;
              }
            } break;
            default: {
              this->result.errors.push_back(this->unexpected_token("parse_vec3_list()", node_current->ri));
              return false;
            };
          }
        }
        return true;
      }

      bool parse_vec2_list(lexer::node_vector::const_iterator sequence_node) {
        element_type type {element_type::position};
        if (sequence_node->ri == token_type::sequence_texture_coord) {
          type = element_type::texturecoord0;
        } else {
          this->result.errors.push_back("Unexpected node type encountered in parse_vec2_list()");
          return false;
        }

        while (node_current != node_end) {
          switch(node_current->ri) {
            case token_type::datatype_int: {
              this->result.errors.push_back(this->unexpected_token("parse_vec2_list()", node_current->ri));
              return false;
            } break;
            case token_type::datatype_float: {
              if(!parse_vec2(type)) {
                return false;
              }
            } break;
            default: {
              return true;
            };
          }
        }
        return true;
      }

      bool parse_triangle_list() {
        if (node_current == node_end) {
          return true;
        }

        while(node_current != node_end) {
          switch(node_current->ri) {
            case token_type::sequence_normal:
            case token_type::sequence_triangle:
            case token_type::sequence_texture_coord:
            case token_type::sequence_position: {
              return true;
            } break;
            case token_type::datatype_int: {
              auto v1 = this->consume_token();
              auto v2 = this->consume_token();
              auto v3 = this->consume_token();
              if(v1 == node_end || v2 == node_end || v3 == node_end) {
                return false;
              }
              this->parsed_sequences[element_type::triangle].push_back(v1);
              this->parsed_sequences[element_type::triangle].push_back(v2);
              this->parsed_sequences[element_type::triangle].push_back(v3);
            } break;
            case token_type::datatype_float: {
              this->result.errors.push_back(this->unexpected_token("parse_triangle_list()", node_current->ri));
              return false;
            } break;
            default: {
              this->result.errors.push_back(this->unexpected_token("parse_triangle_list()", node_current->ri));
              return false;
            } break;
          }
        }
        return true;
      }

      bool parse_position_sequence() {
        return false;
      }

      bool parse_vec3(element_type type) {
        switch(this->node_current->ri) {
          case token_type::datatype_float: {
            auto x = this->consume_token();
            auto y = this->consume_token();
            auto z = this->consume_token();
            if(x == node_end || y == node_end || z == node_end) {
              return false;
            }
            this->parsed_sequences[type].push_back(x);
            this->parsed_sequences[type].push_back(y);
            this->parsed_sequences[type].push_back(z);
          } break;

          default: {
            return false;
          }
        }
        return true;
      }

      bool parse_vec2(element_type type) {
        auto x = this->consume_token();
        auto y = this->consume_token();
        if(x->ri != token_type::datatype_float || y->ri != token_type::datatype_float) {
          return false;
        }
        if(x == node_end || y == node_end) {
          return false;
        }
        this->parsed_sequences[type].push_back(x);
        this->parsed_sequences[type].push_back(y);
        return true;
      }

      bool parse_float() {
        return false;
      }

      lexer::node_vector::const_iterator consume_token() {
        if (node_current == node_end) {
          return node_end;
        } else if (node_current->ri == token_type::whitespace) {
          ++node_current;
        }

        return node_current++;
      }

      std::string unexpected_token(const std::string& fn_name, std::size_t token) {
        return fmt::format("Unexpected token index {} in method {}", token, fn_name);
      }

      const lexer::threaded_lexer& tokenizer;
      lexer::node_vector::const_iterator node_current, node_end;
      sequence_by_element_array parsed_sequences;
      parser_result result;
      bool parse_data;
    };

    parser_result  parse(const std::string& input) {
      lexer::threaded_lexer tokenizer = create_lexer();

      tokenizer.tokenize(input);

      parser_result result;
      if (tokenizer.error_nodes.size() > 0) {
        result.errors.push_back("lexer failed.");
        return result;
      }

      auto new_start = std::remove_if(tokenizer.nodes.begin(), tokenizer.nodes.end(), [](const auto& node) {
        return node.ri == token_type::whitespace;
      });
      tokenizer.nodes.erase(new_start, tokenizer.nodes.end());

      auto p = ll_1_parser(tokenizer);

      if(!p.parse_mesh()) {
        result.errors = p.result.errors;
      } else {
        std::size_t element_count = p.parsed_sequences[element_type::position].size() / 3;
        result.parsed_mesh = meshoptim::create_mesh_data(element_count);

        for (int i = 0; i < element_count; ++i)
        {
          auto x = p.parsed_sequences[element_type::position][3 * i];
          auto y = p.parsed_sequences[element_type::position][3 * i + 1];
          auto z = p.parsed_sequences[element_type::position][3 * i + 2];

          meshoptim::xyz vert {
            std::string(input, x->p, x->n).c_str(),
            std::string(input, y->p, y->n).c_str(),
            std::string(input, z->p, z->n).c_str(),
          };

          meshoptim::set_vertex(result.parsed_mesh, i, vert);
        }

        for (int i = 0; i < p.parsed_sequences[element_type::normal].size(); i += 3)
        {
          std::size_t element_idx = i / 3;
          auto x = p.parsed_sequences[element_type::normal][i];
          auto y = p.parsed_sequences[element_type::normal][i + 1];
          auto z = p.parsed_sequences[element_type::normal][i + 2];

          meshoptim::xyz vert {
            std::string(input, x->p, x->n).c_str(),
            std::string(input, y->p, y->n).c_str(),
            std::string(input, z->p, z->n).c_str(),
          };

          meshoptim::set_normal(result.parsed_mesh, element_idx, vert);
        }

        for (int i = 0; i < p.parsed_sequences[element_type::texturecoord0].size(); i += 2)
        {
          std::size_t element_idx = i / 2;
          auto x = p.parsed_sequences[element_type::texturecoord0][i];
          auto y = p.parsed_sequences[element_type::texturecoord0][i + 1];

          meshoptim::xy coord {
            std::string(input, x->p, x->n).c_str(),
            std::string(input, y->p, y->n).c_str(),
          };

          meshoptim::set_texture_coord(result.parsed_mesh, element_idx, coord);
        }

        result.parsed_index_buffer.resize(p.parsed_sequences[element_type::triangle].size());
        for (int i = 0; i < p.parsed_sequences[element_type::triangle].size(); ++i)
        {
          auto x = p.parsed_sequences[element_type::triangle][i];
          std::string x_str(input, x->p, x->n);
          std::istringstream ss (x_str);
          ss >> result.parsed_index_buffer[i];
        }
      }
      return result;
    }
  }
}
