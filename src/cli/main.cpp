#include "docopt.h"
#include "fmt/format.h"
#include <iostream>
#include <string>
#include <fstream>
#include <numeric>
#include <streambuf>
#include "parser/parser.h"
#include "meshoptim/process.h"

static const char USAGE[] =
R"(Mesh exporter and optimizer for Ogre 2.

    Usage:
      ogre-meshoptim [--output-file=path] <input_file>...
      ogre-meshoptim (-h | --help)
      ogre-meshoptim --version

    Options:
      -h --help     Show this screen.
      --version     Show version.
      --speed=<kn>  Speed in knots [default: 10].
)";

std::string open_file(const std::string& file) {
  std::ifstream t(file);
  std::string str;

  t.seekg(0, std::ios::end);
  auto file_size = t.tellg();
  if (file_size <= 0) {
    std::cerr << fmt::format("File {} is empty\n", file);
    exit(1);
  }

  str.reserve();
  t.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)),
              std::istreambuf_iterator<char>());
  return str;
}

int main(int argc, const char** argv)
{
    std::map<std::string, docopt::value> args
        = docopt::docopt(USAGE,
                         { argv + 1, argv + argc },
                         true,
                         "Ogre mesh exporter");

    meshoptim::string_vector xml_parts {
      "<mesh>",
      "<submeshes>",
      "</submeshes>",
      "</mesh>"
    };
    for(auto const& input_file : args["<input_file>"].asStringList()) {
      const std::string str(open_file(input_file));

      meshoptim::parser::parser_result result = meshoptim::parser::parse(str);
      std::string final(meshoptim::remove_duplicates(result.parsed_mesh));

      if (result.errors.size() > 0) {
        std::cerr << "Errors: \n";
        for(const auto& err : result.errors) {
          std::cerr << err << "\n";
        }
      } else {
        auto submesh = meshoptim::to_xml_parts(result.parsed_mesh, result.parsed_index_buffer);
        xml_parts.insert(xml_parts.end() - 2, submesh.begin(), submesh.end());
      }
    }

    std::string out;
    out.reserve(xml_parts.size() * 256);
    for(const std::string& s : xml_parts) {
      out += '\n' + s;
    }
    // const std::string out = std::reduce(xml_parts.begin(), xml_parts.end(), start, [](const std::string& a, const std::string& b) {
    //   return a + '\n' + b;
    // });
    std::cout << out;

    return 0;
}
