#pragma once
#include "meshoptim/data.h"
#include <vector>

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

  std::string remove_duplicates(const std::string& subject);

  std::string create_mesh_data(std::size_t element_count);

  std::size_t count_elements(const std::string& subject);

  std::string_view get_element(const std::string& subject, std::size_t element_idx, std::size_t offset = 0, std::size_t data_size = sizes::element_size);

  void set_normal(std::string& mesh, std::size_t element_idx, const xyz& normal);

  std::string_view get_normal(const std::string& mesh, std::size_t element_idx);

  void set_vertex(std::string& mesh, std::size_t element_idx, const xyz& vertex);

  std::string_view get_vertex(const std::string& mesh, std::size_t element_idx);

  string_vector to_xml_parts(const std::string& mesh);
}