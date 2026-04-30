#pragma once

#include <bitset>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "UI_modules/input.hpp"

// modules de l'ui à activer/désactiver
enum ModuleIndex {
    INPUT,
    CHAT,
    AUTOMATIONS,
    VIDEO,
    MODULE_BIT_COUNT
};

class Interface_graphique {
public:
    Interface_graphique();
    void use_input(ModuleDeCommande input);


};