enable_testing() # Enable testing in CMake

find_package(GTest CONFIG REQUIRED)

add_executable(my_tests
        test_renderer.cpp
        test_loader.cc
        # ... other test files
)

# Link necessary libraries to your tests (e.g., your engine library)
target_link_libraries(my_tests braque GTest::gtest_main )

include(GoogleTest)
gtest_discover_tests(my_tests)