#include "parser/parser.h"
#include "meshoptim/process.h"
#include <algorithm>
#include "fmt/format.h"

namespace meshoptim
{
  namespace parser
  {

    typedef std::vector<lexer::node_vector::const_iterator> const_iterator_vector;
    typedef std::array<const_iterator_vector, element_type::number_of_element_types> sequence_by_element_array;

    class ll_1_parser {
    public:
      explicit ll_1_parser(const lexer::generic_lexer& tokenizer, bool parse_data = false) : tokenizer(tokenizer), node_current(tokenizer.nodes.begin()), node_end(tokenizer.nodes.end()), parse_data(parse_data) {

      }

      bool parse_mesh() {
        if (node_current == node_end) {
          return true;
        }

        switch(node_current->ri) {
          case meshoptim::parser::token_type::sequence_position:
          case meshoptim::parser::token_type::sequence_normal: {
            return this->parse_sequences();
          } break;
          default: {
            this->result.errors.push_back("Unexpected token in parse_mesh()");
            return false;
          } break;
        }
        return false;
      }

      bool parse_sequences() {
        if (node_current == node_end) {
          return true;
        }

        switch(node_current->ri) {
          case token_type::sequence_normal:
          case token_type::sequence_position: {
            auto node = this->consume_token();
            this->parse_vec3_list(node);
            return this->parse_sequences();
          } break;
          default: {
            this->result.errors.push_back(this->unexpected_token("parse_sequences()", node_current->ri));
            return false;
          } break;
        }
        return false;
      }

      bool parse_vec3_list(lexer::node_vector::const_iterator sequence_node) {
        element_type type {element_type::position};
        if (sequence_node->ri == token_type::sequence_position) {
          type = element_type::position;
        } else if (sequence_node->ri == token_type::sequence_normal) {
          type = element_type::normal;
        } else {
          this->result.errors.push_back("Unexpected node type encountered in parse_vec3_list()");
          return false;
        }

        switch(node_current->ri) {
          case token_type::sequence_normal:
          case token_type::sequence_position: {
            return true;
          } break;
          case token_type::datatype_float: {
            if(!parse_vec3(type)) {
              return false;
            }
            return parse_vec3_list(sequence_node);
          } break;
          default: {
            return false;
          } break;
        }
        return false;
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

      const lexer::generic_lexer& tokenizer;
      lexer::node_vector::const_iterator node_current, node_end;
      sequence_by_element_array parsed_sequences;
      parser_result result;
      bool parse_data;
    };

    parser_result  parse(const std::string& input) {
      mesh_lexer tokenizer;

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
      }
      return result;
    }

  }
}
