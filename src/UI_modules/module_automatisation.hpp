#pragma once

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"
#include "../game_engine.hpp"

/**
 * @file module_automatisation.hpp
 * @brief Module de selection et navigation des automations debloquees.
 */
class ModuleDeAutomatisation : public Module {
public:
	/**
	 * @brief Constructeur: initialise un menu vide.
	 */
	ModuleDeAutomatisation();

	/**
	 * @brief Prepare la liste d'automations affichables a partir des debloquages courants.
	 * @param unlocked_automs Identifiants d'automs debloquees
	 * @param meta Metadonnees du jeu utilisées pour resoudre les labels
	 */
	void preparer_menu_depuis_unlocked(const std::vector<std::string>& unlocked_automs, const GameMeta& meta);

	/**
	 * @brief Reinitialise le menu et efface la selection courante.
	 */
	void reinitialiser_menu();

	/**
	 * @brief Indique si aucune automation n'est disponible.
	 * @return true si le menu est vide
	 */
	bool est_menu_vide() const;

	/**
	 * @brief Deplace la selection d'un cran vers le haut.
	 * @return true si la selection a change
	 */
	bool deplacer_selection_haut();

	/**
	 * @brief Deplace la selection d'un cran vers le bas.
	 * @return true si la selection a change
	 */
	bool deplacer_selection_bas();

	/**
	 * @brief Retourne l'index actuellement selectionne.
	 */
	int index_selection() const;

	/**
	 * @brief Retourne l'identifiant de l'automation selectionnee.
	 */
	std::string id_selection() const;

	/**
	 * @brief Retourne le libelle de l'automation selectionnee.
	 */
	std::string label_selection() const;

	/**
	 * @brief Retourne les libelles actuellement presents dans le menu.
	 */
	std::vector<std::string> labels_courants() const;

	/**
	 * @brief Produit le rendu FTXUI du module.
	 */
	ftxui::Element Render() const override;

	/**
	 * @brief Construit le composant interactif du module (menu de navigation).
	 */
	ftxui::Component MakeComponent(InterfaceGraphique& interface_graphique) override;

private:
	ftxui::Element _render_frame(ftxui::Element corps) const;

	/// Automations actuellement visibles dans le menu.
	std::vector<GameChoice> _automs_courantes;
	/// Libelles synchronises avec la liste d'automations pour le menu FTXUI.
	std::vector<std::string> _labels_courants;
	/// Index de la selection courante, ou -1 si vide.
	int _selection_courante = -1;
};