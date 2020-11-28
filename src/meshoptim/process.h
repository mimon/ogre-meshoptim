#pragma once
#include "meshoptim/data.h"
#include <vector>

namespace meshoptim
{

  std::string remove_duplicates(const std::string& subject);

  std::string create_mesh_data(std::size_t element_count);

  std::size_t count_elements(const std::string& subject);

  std::string_view get_element(const std::string& subject, std::size_t element_idx);

  void set_normal(std::string& mesh, std::size_t element_idx, const xyz& normal);

  std::string_view get_normal(const std::string& mesh, std::size_t element_idx);

  void set_vertex(std::string& mesh, std::size_t element_idx, const xyz& vertex);

  std::string_view get_vertex(const std::string& mesh, std::size_t element_idx);
}