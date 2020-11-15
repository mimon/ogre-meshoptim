#pragma once
#include "meshoptim/data.h"
#include <vector>

namespace meshoptim
{
  typedef std::vector<std::size_t> size_t_vector;

  std::string remove_duplicates(const std::string& subject);

  std::string create_mesh_data(std::size_t element_count);

  std::size_t count_elements(const std::string& subject);

  void set_vertex(std::string& mesh, std::size_t element_idx, const xyz& vertex);
}