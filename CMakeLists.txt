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

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG        v3.11.3
)

add_executable(bonhomme-heureux 
  src/main.cpp 
  src/interface_graphique.hpp 
  src/interface_graphique.cpp 
  src/UI_modules/module.hpp
  src/UI_modules/module.cpp
  src/UI_modules/module_de_commande.hpp 
  src/UI_modules/module_de_commande.cpp
  src/UI_modules/theme.hpp
  src/UI_modules/theme.cpp
  src/UI_modules/module_de_discussion.hpp
  src/UI_modules/module_de_discussion.cpp
  src/UI_modules/module_automatisation.hpp
  src/UI_modules/module_automatisation.cpp
  src/tests/exemples.hpp
  src/tests/exemples.cpp
  src/game_engine.hpp
  src/game_engine.cpp
  src/game_renderer.hpp
  src/game_renderer.cpp
)

FetchContent_MakeAvailable(ftxui nlohmann_json)

target_include_directories(bonhomme-heureux PRIVATE src)

target_link_libraries(bonhomme-heureux
  PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
    nlohmann_json::nlohmann_json
)