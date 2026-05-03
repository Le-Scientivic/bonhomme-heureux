#pragma once

#include <ftxui/dom/elements.hpp>
#include <string>

#include <ftxui/component/component.hpp>

#include "module.hpp"

class ModuleDeCommande : Module {
public:
    ModuleDeCommande (ftxui::Element contenu);

    void changer_contenu(ftxui::Element nouveau_contenu);

private:
    ftxui::Element _contenu;
};