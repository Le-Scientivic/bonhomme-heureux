#pragma once

#include <bitset>

#include "UI_modules/input.hpp"

// modules de l'ui à activer/désactiver
enum ModuleIndex {
    INPUT,
    CHAT,
    AUTOMATIONS,
    VIDEO,
    MODULE_BIT_COUNT
};
// contient l'état d'activation des modules de l'UI
extern std::bitset<MODULE_BIT_COUNT> uiModules;

void init_modules();
void display_modules();