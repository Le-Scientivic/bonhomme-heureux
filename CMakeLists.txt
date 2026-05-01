cmake_minimum_required(VERSION 3.14)
project(bonhomme-heureux CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v6.1.9
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(ftxui)


add_executable(bonhomme-heureux 
  src/main.cpp 
  src/interface_graphique.hpp 
  src/interface_graphique.cpp 
  src/UI_modules/input.hpp 
  src/UI_modules/input.cpp
  src/UI_modules/theme.hpp
  src/UI_modules/theme.cpp
  src/tests/exemples.hpp
  src/tests/exemples.cpp
)

target_link_libraries(bonhomme-heureux
  PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
)
