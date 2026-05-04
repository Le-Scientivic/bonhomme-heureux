#pragma once

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"
#include "../game_engine.hpp"

/// Module de selection des automations debloquees par le joueur.
class ModuleDeAutomatisation : public Module {
public:
	/// Construit le module de selection des automations.
	ModuleDeAutomatisation();

	/// Prepare la liste d'automations affichables a partir des debloquages courants.
	void preparer_menu_depuis_unlocked(const std::vector<std::string>& unlocked_automs, const GameMeta& meta);
	/// Reinitialise le menu et efface la selection courante.
	void reinitialiser_menu();

	/// Indique si aucune automation n'est disponible.
	bool est_menu_vide() const;
	/// Deplace la selection d'un cran vers le haut.
	bool deplacer_selection_haut();
	/// Deplace la selection d'un cran vers le bas.
	bool deplacer_selection_bas();
	/// Retourne l'index actuellement selectionne.
	int index_selection() const;
	/// Retourne l'identifiant de l'automation selectionnee.
	std::string id_selection() const;
	/// Retourne le libelle de l'automation selectionnee.
	std::string label_selection() const;
	/// Retourne les libelles actuellement presents dans le menu.
	std::vector<std::string> labels_courants() const;

	/// Produit le rendu FTXUI du module.
	ftxui::Element Render() const override;

private:
	/// Automations actuellement visibles dans le menu.
	std::vector<GameChoice> _automs_courantes;
	/// Index de la selection courante, ou -1 si vide.
	int _selection_courante = -1;
};