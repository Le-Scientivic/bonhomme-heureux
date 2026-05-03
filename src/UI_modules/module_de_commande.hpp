#pragma once

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"

class ModuleDeCommande : public Module {
public:
    ModuleDeCommande();

    ftxui::Element Render() const override;
};