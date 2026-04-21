cmake_minimum_required(VERSION 3.14)
project(bonhomme-heureux CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG        v5.0.0
)
FetchContent_MakeAvailable(ftxui)

add_executable(bonhomme-heureux src/main.cpp)
target_link_libraries(bonhomme-heureux
  PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
)
