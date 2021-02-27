#include "meshoptim/process.h"
#include "meshoptim/data.h"
#include <functional>
#include <numeric>
#include "fmt/format.h"

namespace meshoptim
{

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

  void set_xy(std::string& mesh, std::size_t element_idx, std::size_t element_layout, const xy& xy) {
    const std::array<std::string_view, 2> input { std::string_view(xy.x), std::string_view(xy.y) };
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

  void set_texture_coord(std::string& mesh, std::size_t element_idx, const xy& coord) {
    set_xy(mesh, element_idx, layout::texture_coord0, coord);
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

  std::string_view get_texture_coord(const std::string& subject, std::size_t element_idx) {
    return get_element(subject, element_idx).substr(layout::texture_coord0, sizes::texture_coord);
  }

  string_vector to_xml_parts(const std::string& mesh, const size_t_vector& index_buffer) {
    const std::size_t elm_count = count_elements(mesh);

    string_vector out {
      "<mesh>",
      "<submeshes>",
      "<submesh operationtype=\"triangle_list\" usesharedvertices=\"false\">",
    };
    // out.reserve(9 + 2 * elm_count);

    string_vector out_tail {
      "</submesh>",
      "</submeshes>",
      "</mesh>"
    };

    string_vector vertexbuffer {
      fmt::format("<geometry vertexcount=\"{}\">", elm_count),
      "<vertexbuffer normals=\"true\" positions=\"true\" tangent_dimensions=\"4\" tangents=\"false\" texture_coords=\"0\">"
    };
    for(int i=0; i<elm_count;++i) {
      vertexbuffer.push_back("<vertex>");
      vertexbuffer.push_back(
        fmt::format(
          "<position x=\"{}\" y=\"{}\" z=\"{}\" />",
          get_element(mesh, i, layout::vertex, sizes::x),
          get_element(mesh, i, layout::vertex + sizes::x, sizes::y),
          get_element(mesh, i, layout::vertex + sizes::x + sizes::z, sizes::z)
        )
      );
      vertexbuffer.push_back(
        fmt::format(
          "<normal x=\"{}\" y=\"{}\" z=\"{}\" />",
          get_element(mesh, i, layout::normal, sizes::x),
          get_element(mesh, i, layout::normal + sizes::x, sizes::y),
          get_element(mesh, i, layout::normal + sizes::x + sizes::z, sizes::z)
        )
      );
      vertexbuffer.push_back("</vertex>");
    }
    string_vector vertexbuffer_tail {
      "</vertexbuffer>",
      "</geometry>",
    };


    string_vector triangles {
      fmt::format("<faces count=\"{}\">", index_buffer.size() / 3),
    };
    for(int i=0; i<index_buffer.size() ; i+= 3) {
      triangles.push_back(
        fmt::format(
          "<face v1=\"{}\" v2=\"{}\" v3=\"{}\" />",
          index_buffer[i],
          index_buffer[i + 1],
          index_buffer[i + 2]
        )
      );
    }
    string_vector triangles_tail {
      "</faces>",
    };


    out.insert(out.end(), vertexbuffer.begin(), vertexbuffer.end());
    out.insert(out.end(), vertexbuffer_tail.begin(), vertexbuffer_tail.end());

    out.insert(out.end(), triangles.begin(), triangles.end());
    out.insert(out.end(), triangles_tail.begin(), triangles_tail.end());

    out.insert(out.end(), out_tail.begin(), out_tail.end());
    return out;
  }
}