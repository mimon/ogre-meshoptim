#include "meshoptim/process.h"
#include "meshoptim/data.h"
#include <functional>
#include <numeric>

namespace meshoptim
{
  namespace sizes {
  constexpr std::size_t size_of_coordinate = 32;
  constexpr std::size_t x = size_of_coordinate;
  constexpr std::size_t y = size_of_coordinate;
  constexpr std::size_t z = size_of_coordinate;
  constexpr std::size_t xyz = x + y + z;
  constexpr std::size_t vertex = xyz;
  constexpr std::size_t normal = xyz;
  constexpr std::size_t element_size = vertex + normal;
  }

  namespace layout {
  constexpr std::size_t vertex = 0;
  constexpr std::size_t normal = sizes::vertex;
  // constexpr std::size_t tangent = sizes::normal - 1;
  }

  struct fixed_xyz {
    fixed_string x;
    fixed_string y;
    fixed_string z;
  };

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
    const std::array<fixed_string, 3> input { fixed_string(xyz.x), fixed_string(xyz.y), fixed_string(xyz.z) };
    for (int i = 0; i < input.size(); ++i)
    {
      auto pos = mesh.begin() + element_idx * sizes::element_size + element_layout + i * sizes::size_of_coordinate;
      auto replace_count = pos + sizes::size_of_coordinate;
      mesh.replace(pos, replace_count, input[i].data(), sizes::size_of_coordinate);
    }
  }

  std::string_view get_element(const std::string& subject, std::size_t element_idx) {
    auto elm_start = subject.begin() + element_idx * sizes::element_size;
    std::string_view element(&(*elm_start), sizes::element_size);
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
}