#pragma once

#include <string>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"

class ModuleDeAutomatisation : public Module {
public:
	ModuleDeAutomatisation();

	ftxui::Element Render() const override;
};