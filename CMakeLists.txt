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

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(ftxui nlohmann_json)

add_executable(bonhomme-heureux
  src/main.cpp
  src/interface_graphique.hpp
  src/interface_graphique.cpp
  src/UI_modules/input.hpp
  src/UI_modules/input.cpp
  src/game_engine.hpp
  src/game_engine.cpp
  src/game_renderer.hpp
  src/game_renderer.cpp
)

target_include_directories(bonhomme-heureux PRIVATE src)

target_link_libraries(bonhomme-heureux
  PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
    nlohmann_json::nlohmann_json
)