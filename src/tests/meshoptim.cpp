#include "tests/catch2.hpp"

#include "tests/catch2.hpp"
#include "meshoptim/process.h"

SCENARIO("Constructing a mesh data structure") {
  GIVEN("A data structure") {
    meshoptim::vertex v1 { "1.1", "2.2", "3.3" };
    meshoptim::vertex v2 { "4.4", "5.5", "6.6" };

    std::string themesh = meshoptim::create_mesh_data(2);

    WHEN("Assigning vertices") {
      const size_t count1 = themesh.size();
      set_vertex(themesh, 0, v1);
      const size_t count2 = themesh.size();
      // set_vertex(themesh, 1, v2);

      // const size_t count3 = themesh.size();

      THEN("The length of the data of the mesh does not change") {
        REQUIRE(count1 == count2);
      }
    }

    WHEN("Not having duplicated") {
      set_vertex(themesh, 0, v1);
      set_vertex(themesh, 1, v2);

      WHEN("Processing it") {
        REQUIRE(meshoptim::count_elements(themesh) == 2);
        std::string newmesh = meshoptim::remove_duplicates(themesh);

        THEN("No duplicates should be found") {
          CHECK(meshoptim::count_elements(newmesh) == 2);
        }
      }
    }

    WHEN("Duplicate vertices exists") {
      set_vertex(themesh, 0, v1);
      set_vertex(themesh, 1, v1);

      WHEN("Processing it") {
        std::string newmesh = meshoptim::remove_duplicates(themesh);

        THEN("The duplicates should be found") {
          CHECK(meshoptim::count_elements(newmesh) == 1);
          // CHECK(meshoptim::get_vertex(0) == v1);
        }
      }
    }
  }
}