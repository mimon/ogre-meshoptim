#include "meshoptim/process.h"
#include "meshoptim/data.h"
#include <functional>
#include <numeric>

namespace meshoptim
{
  struct fixed_xyz {
    fixed_string x;
    fixed_string y;
    fixed_string z;
  };

  struct element {
    fixed_xyz vertex;
  };

  std::vector<std::size_t> hash_elements(const std::string& subject) {
    const std::size_t element_count = count_elements(subject);
    std::vector<std::size_t> hashes(element_count, 0);
    auto hash_fn = std::hash<std::string_view>();

    for (int i = 0; i < element_count; ++i)
    {
      auto elm_start = subject.begin() + i * sizeof(element);
      auto elm_end = elm_start + sizeof(element);
      std::string_view element (&(*elm_start), sizeof(element));
      ;
      hashes[i] = hash_fn(element);
    }

    return hashes;
  }

  std::string remove_duplicates(const std::string& subject) {
    const std::vector<std::size_t> hashes = hash_elements(subject);
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
      auto old_pos = subject.begin() + old_index * sizeof(element);
      auto new_pos = new_mesh.begin() + new_index * sizeof(element);
      new_mesh.replace(new_pos, new_pos + sizeof(element), old_pos, old_pos + sizeof(element));
    }

    return new_mesh;
  }

  size_t count_elements(const std::string& subject) {
    return subject.size() / sizeof(element);
  }

  std::string create_mesh_data(std::size_t element_count) {
    const std::size_t str_size = sizeof(element) * element_count;
    return std::string(str_size, '0');
  }

  void set_vertex(std::string& mesh, std::size_t element_idx, const xyz& vertex) {
    const std::array<fixed_string, 3> input { fixed_string(vertex.x), fixed_string(vertex.y), fixed_string(vertex.z) };
    const auto size_of_coordinate = sizeof(decltype(input)::value_type);
    for (int i = 0; i < input.size(); ++i)
    {
      auto pos = mesh.begin() + element_idx * sizeof(element) + i * size_of_coordinate;
      auto replace_count = pos + size_of_coordinate;
      mesh.replace(pos, replace_count, input[i].data(), size_of_coordinate);
    }
  }
}