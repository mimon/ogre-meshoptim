#include "tests/catch2.hpp"

#include "tests/catch2.hpp"
#include "meshoptim/process.h"

// Assigning integers without '.' decimals is not supported

SCENARIO("Constructing a mesh data structure") {
  GIVEN("A data structure") {
    meshoptim::xyz v1 { "1.1", "2.2", "3.3" };
    meshoptim::xyz v2 { "4.4", "5.5", "6.6" };
    meshoptim::xyz v3 { "10.0", "11.0", "12.0" };

    std::string themesh = meshoptim::create_mesh_data(2);

    WHEN("Assigning vertices") {
      const size_t count1 = themesh.size();
      set_vertex(themesh, 0, v1);
      const size_t count2 = themesh.size();

      THEN("The length of the data of the mesh does not change") {
        REQUIRE(count1 == count2);
      }

      WHEN("Getting vertex") {
        std::string getv1(meshoptim::get_vertex(themesh, 0));
        THEN("Correct vertex data should be returned") {
          REQUIRE(getv1.substr(0, 3) == "1.1");
        }
      }
    }

    WHEN("Assigning normals") {
      set_vertex(themesh, 0, v1);
      set_normal(themesh, 0, v3);

      WHEN("Getting normal") {
        std::string normal(meshoptim::get_normal(themesh, 0));
        THEN("Correct vertex data should be returned") {
          REQUIRE(normal.substr(0, 4) == "10.0");
        }
      }
    }

    WHEN("Not having duplicated elements") {
      set_vertex(themesh, 0, v1);
      set_vertex(themesh, 1, v1);
      set_normal(themesh, 0, v3);
      set_normal(themesh, 1, v1);

      WHEN("Processing it") {
        REQUIRE(meshoptim::count_elements(themesh) == 2);
        std::string newmesh = meshoptim::remove_duplicates(themesh);

        THEN("No duplicates should be found") {
          CHECK(meshoptim::count_elements(newmesh) == 2);
        }
      }
    }

    WHEN("Duplicate elements exists") {
      set_vertex(themesh, 0, v1);
      set_vertex(themesh, 1, v1);
      set_normal(themesh, 0, v3);
      set_normal(themesh, 1, v3);

      WHEN("Processing it") {
        std::string newmesh = meshoptim::remove_duplicates(themesh);

        THEN("The duplicates should be found") {
          CHECK(meshoptim::count_elements(newmesh) == 1);
          // CHECK(meshoptim::get_vertex(0) == v1);
        }
      }
    }

    WHEN("Exporting to XML") {
      set_vertex(themesh, 0, v1);
      set_vertex(themesh, 1, v1);
      set_normal(themesh, 0, v3);
      set_normal(themesh, 1, v1);

      // meshoptim::string_vector xml = meshoptim::to_xml_parts(themesh);

      THEN("It exports valid XML") {
        // <mesh>
        //   <submeshes>
        //     <submesh>
        //       <geometry>
          //       <vertexbuffer>
          //         <vertex></vertex>
          //       </vertexbuffer>
        //       </geometry>
        //     </submesh>
        //   </submeshes>
        // </mesh>
        // constexpr int vertices = 2;
        // CHECK(xml.size() == 10 + vertices * (2 + 2));
      }
    }
  }
}