#pragma once

#include <string>

#include <ftxui/component/component.hpp>

/**
 * @file theme.hpp
 * @brief Helpers de presentation reutilisables pour l'UI (FTXUI).
 *
 * Petites fonctions inline qui retournent des `ftxui::Element` utilises
 * pour normaliser l'apparence des textes et barres de progression.
 */
namespace theme {
    /**
     * @brief Crée un paragraphe stylé pour les textes d'inscription.
     * @param text Texte a afficher
     * @return Element FTXUI representant le paragraphe
     */
    inline ftxui::Element incription_text(std::string text) { return ftxui::paragraph(text); };

    /**
     * @brief Affiche une barre de progression d'automation avec un titre.
     * @param titre Titre de la progression
     * @param ratio Valeur entre 0.0 et 1.0 representant l'avancement
     * @return Element FTXUI compose du titre et de la jauge
     */
    inline ftxui::Element automation_progression(std::string titre, float ratio) { return ftxui::vbox(ftxui::text(titre), ftxui::gaugeUp(ratio)); };
}