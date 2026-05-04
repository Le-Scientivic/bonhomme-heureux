#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "UI_modules/module.hpp"

//#include "UI_modules/input.hpp"

class InterfaceGraphique {
public:
    void ajouter_module(std::shared_ptr<Module> module);
    ftxui::Element Render() const;
    ftxui::Element RenderAvecEnTete(const std::string& titre, const std::string& sous_titre, const std::string& description, const ftxui::Element& corps = ftxui::filler()) const;
    void afficher_rendu();

public:
    std::vector<std::shared_ptr<Module>> _liste_de_modules = {};

    ftxui::ScreenInteractive _screen = ftxui::ScreenInteractive::Fullscreen();
};