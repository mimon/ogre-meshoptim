#include "meshoptim/process.h"
#include "meshoptim/data.h"
#include <functional>
#include <numeric>
#include "fmt/format.h"

namespace meshoptim
{
  typedef std::vector<std::size_t> size_t_vector;

  size_t_vector hash_elements(const std::string& subject) {
    const std::size_t element_count = count_elements(subject);
    size_t_vector hashes(element_count, 0);
    auto hash_fn = std::hash<std::string_view>();

    for (int i = 0; i < element_count; ++i)
    {
      std::string_view element = get_element(subject, i);
      hashes[i] = hash_fn(element);
    }

    return hashes;
  }

  std::string remove_duplicates(const std::string& subject) {
    const size_t_vector hashes = hash_elements(subject);
    size_t_vector indexes(hashes.size());
    std::iota(indexes.begin(), indexes.end(), 0);

    std::sort(indexes.begin(), indexes.end(), [&hashes](std::size_t a, std::size_t b) {
      return (hashes[a] < hashes[b]);
    });

    auto dups_start = std::unique(indexes.begin(), indexes.end(), [&hashes](std::size_t a, std::size_t b) {
      return (hashes[a] == hashes[b]);
    });

    std::size_t new_element_count = std::distance(indexes.begin(), dups_start);
    std::string new_mesh = create_mesh_data(new_element_count);
    for(std::size_t new_index = 0; new_index < new_element_count; ++new_index) {
      std::size_t old_index = indexes[new_index];
      auto old_pos = subject.begin() + old_index * sizes::element_size;
      auto new_pos = new_mesh.begin() + new_index * sizes::element_size;
      new_mesh.replace(new_pos, new_pos + sizes::element_size, old_pos, old_pos + sizes::element_size);
    }

    return new_mesh;
  }

  size_t count_elements(const std::string& subject) {
    return subject.size() / sizes::element_size;
  }

  std::string create_mesh_data(std::size_t element_count) {
    const std::size_t str_size = sizes::element_size * element_count;
    return std::string(str_size, '0');
  }

  void set_xyz(std::string& mesh, std::size_t element_idx, std::size_t element_layout, const xyz& xyz) {
    const std::array<std::string_view, 3> input { std::string_view(xyz.x), std::string_view(xyz.y), std::string_view(xyz.z) };
    for (int i = 0; i < input.size(); ++i)
    {
      auto begin = mesh.begin() + element_idx * sizes::element_size + element_layout + i * sizes::size_of_coordinate;
      auto end = begin + std::min(input[i].size(), sizes::size_of_coordinate);
      mesh.replace(begin, end, input[i]);
    }
  }

  std::string_view get_element(const std::string& subject, std::size_t element_idx, std::size_t offset, std::size_t data_size) {
    auto elm_start = subject.begin() + element_idx * sizes::element_size + offset;
    std::string_view element(&(*elm_start), data_size);
    return element;
  }

  void set_vertex(std::string& mesh, std::size_t element_idx, const xyz& vertex) {
    set_xyz(mesh, element_idx, layout::vertex, vertex);
  }

  std::string_view get_vertex(const std::string& subject, std::size_t element_idx) {
    return get_element(subject, element_idx).substr(layout::vertex, sizes::vertex);
  }

  void set_normal(std::string& mesh, std::size_t element_idx, const xyz& normal) {
    set_xyz(mesh, element_idx, layout::normal, normal);
  }

  std::string_view get_normal(const std::string& subject, std::size_t element_idx) {
    return get_element(subject, element_idx).substr(layout::normal, sizes::normal);
  }

  string_vector to_xml_parts(const std::string& mesh) {
    const std::size_t elm_count = count_elements(mesh);

    string_vector out {
      "<mesh>",
      "<submeshes>",
      "<submesh>",
      fmt::format("<geometry vertexcount=\"{}\">", elm_count),
      "<vertexbuffer>"
    };
    out.reserve(9 + 2 * elm_count);

    for(int i=0; i<elm_count;++i) {
      out.push_back("<vertex>");
      out.push_back(
        fmt::format(
          "<position x=\"{}\" y=\"{}\" z=\"{}\" />",
          get_element(mesh, i, layout::vertex, sizes::x),
          get_element(mesh, i, layout::vertex + sizes::x, sizes::y),
          get_element(mesh, i, layout::vertex + sizes::x + sizes::z, sizes::z)
        )
      );
      out.push_back(
        fmt::format(
          "<normal x=\"{}\" y=\"{}\" z=\"{}\" />",
          get_element(mesh, i, layout::normal, sizes::x),
          get_element(mesh, i, layout::normal + sizes::x, sizes::y),
          get_element(mesh, i, layout::normal + sizes::x + sizes::z, sizes::z)
        )
      );
      out.push_back("</vertex>");
    }
    string_vector tail {
      "</vertexbuffer>",
      "</geometry>",
      "</submesh>",
      "</submeshes>",
      "</mesh>"
    };
    out.insert(out.end(), tail.begin(), tail.end());
    return out;
  }
}