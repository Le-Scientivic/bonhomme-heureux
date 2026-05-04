#pragma once

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"

/// Module qui affiche les choix courants et l'historique d'actions du joueur.
class ModuleDeCommande : public Module {
public:
    /// Construit le module de commande vide.
    ModuleDeCommande();

    /// Definit le titre du choix courant et les options visibles.
    void definir_choix(const std::string& titre, const std::vector<std::string>& choix, int selection);
    /// Valide un choix et l'ajoute au journal d'actions.
    void valider_choix(const std::string& choix);
    /// Annule le choix courant sans le valider.
    void annuler_choix();
    /// Efface le choix en cours d'affichage.
    void effacer_choix();

    /// Produit le rendu FTXUI du module.
    ftxui::Element Render() const override;

private:
    /// Titre du groupe de choix courant.
    std::string _titre_choix;
    /// Choix affiches au joueur.
    std::vector<std::string> _choix_courants;
    /// Index de la selection courante dans les choix.
    int _selection_courante = -1;
    /// Historique court des actions ou validations recents.
    std::vector<std::string> _journal;
};