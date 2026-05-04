#pragma once

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"
#include "../game_engine.hpp"

class ModuleDeAutomatisation : public Module {
public:
	ModuleDeAutomatisation();

	void preparer_menu_depuis_unlocked(const std::vector<std::string>& unlocked_automs, const GameMeta& meta);
	void reinitialiser_menu();

	bool est_menu_vide() const;
	bool deplacer_selection_haut();
	bool deplacer_selection_bas();
	int index_selection() const;
	std::string id_selection() const;
	std::string label_selection() const;
	std::vector<std::string> labels_courants() const;

	ftxui::Element Render() const override;

private:
	std::vector<GameChoice> _automs_courantes;
	int _selection_courante = -1;
};