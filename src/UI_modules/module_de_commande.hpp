#pragma once

#pragma once

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"
#include "../game_engine.hpp"

// Forward declaration pour éviter la dépendance circulaire
class InterfaceGraphique;

/**
 * @file module_de_commande.hpp
 * @brief Module qui affiche les choix courants du joueur.
 */
class ModuleDeCommande : public Module {
public:
    /**
     * @brief Construit le module de commande vide.
     */
    ModuleDeCommande();

    /**
     * @brief Definit le titre du choix courant et les options visibles.
     * @param titre Titre a afficher
     * @param choix Liste des options affichables
     * @param selection Index de selection initiale (-1 si aucune)
     */
    void definir_choix(const std::string& titre, const std::vector<std::string>& choix, int selection);

    /**
     * @brief Efface le choix en cours d'affichage.
     */
    void effacer_choix();

    /**
     * @brief Retourne l'index de la selection courante (-1 si aucune).
     */
    int get_selection_courante() const;

    /**
     * @brief Affiche une sequence de boot composee de messages systeme.
     * 
    * La methode lance sa propre boucle FTXUI et met a jour `state` avec
    * l'indice courant des messages affiches. La vue est composee via
    * `RenderAvecEnTete` avec l'interface fournie.
     */
    void afficher_boot_sequence(const GamePhase& phase, GameState& state, InterfaceGraphique& interface_graphique);

    /**
     * @brief Affiche l'ecran d'introduction du jeu et retourne si le
     * joueur souhaite commencer.
     */
    bool afficher_intro(const GameMeta& meta, InterfaceGraphique& interface_graphique);

    /**
     * @brief Affiche un ecran de fin de phase contenant un message aleatoire.
     */
    void afficher_phase_end(const std::vector<std::string>& messages, InterfaceGraphique& interface_graphique);

    /**
     * @brief Produit le rendu FTXUI du module.
     */
    ftxui::Element Render() const override;

    /**
     * @brief Construit le composant interactif du module (menu/selection).
     */
    ftxui::Component MakeComponent(InterfaceGraphique& interface_graphique) override;

private:
    /// Titre du groupe de choix courant.
    std::string _titre_choix;
    /// Choix affiches au joueur.
    std::vector<std::string> _choix_courants;
    /// Index de la selection courante dans les choix.
    int _selection_courante = -1;
};