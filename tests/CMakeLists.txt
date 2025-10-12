cmake_minimum_required(VERSION 3.22)

# Tests for TrueHUD

find_package(Catch2 3 CONFIG REQUIRED)

add_executable(truehud_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/widgetbase_tests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/hudhandler_process_tests.cpp
)

target_compile_features(truehud_tests PRIVATE cxx_std_20)

# Avoid Windows macro pollution (e.g., min/max) that conflicts with standard library and Catch2
 target_compile_definitions(truehud_tests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN TRUEHUD_TESTING=1)


# Include the project source headers and the generated Plugin.h from the src subdir
# We need ${CMAKE_BINARY_DIR}/src because src/CMakeLists.txt generates Plugin.h there.
# We include ${CMAKE_SOURCE_DIR}/src so we can include PCH.h and TrueHUDAPI.h.

target_include_directories(truehud_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_BINARY_DIR}/src/src
)

# Link Catch2 main; also link CommonLibSSE to satisfy RE/SKSE symbols used by PCH.h/headers (compile-only usage)
# CommonLibSSE is added by src/CMakeLists.txt, which our root CMake adds before this tests subdir.

target_link_libraries(truehud_tests PRIVATE
  Catch2::Catch2WithMain
  CommonLibSSE::CommonLibSSE
)

# Ensure the generated headers from the src subproject exist before building tests
add_dependencies(truehud_tests TrueHUD)

include(CTest)
include(Catch)
catch_discover_tests(truehud_tests)
