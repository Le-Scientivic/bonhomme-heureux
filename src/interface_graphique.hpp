#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "UI_modules/module.hpp"

//#include "UI_modules/input.hpp"

/// Orchestrateur de rendu FTXUI regroupant les differents modules de l'interface.
class InterfaceGraphique {
public:
    /// Ajoute un module a la liste des composants rendus.
    void ajouter_module(std::shared_ptr<Module> module);
    /// Compose l'interface complete a partir des modules visibles.
    ftxui::Element Render() const;
    /// Construit une vue encadree avec titre, sous-titre, description et contenu.
    ftxui::Element RenderAvecEnTete(const std::string& titre, const std::string& sous_titre, const std::string& description, const ftxui::Element& corps = ftxui::filler()) const;
    /// Lance la boucle interactive principale de rendu.
    void afficher_rendu();

public:
    /// Liste ordonnee des modules presents dans l'interface.
    std::vector<std::shared_ptr<Module>> _liste_de_modules = {};

    /// Ecran FTXUI plein ecran utilise pour la boucle de rendu.
    ftxui::ScreenInteractive _screen = ftxui::ScreenInteractive::Fullscreen();
};