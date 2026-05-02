#pragma once

#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "UI_modules/module.hpp"

//#include "UI_modules/input.hpp"

class InterfaceGraphique {
public:
    void ajouter_module(Module module);
    void afficher_rendu();

public:
    std::vector<Module> _liste_de_modules = {};

    ftxui::ScreenInteractive _screen = ftxui::ScreenInteractive::Fullscreen();
};