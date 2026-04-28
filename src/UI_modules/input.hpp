#pragma once

#include <ftxui/component/component.hpp>
#include <string>

/**
 * Module de saisie utilisateur avec texte configurable à gauche et à droite.
 * Affiche un composant interactif en bas de l'écran pour capturer l'entrée utilisateur.
 * 
 * Exemple d'utilisation :
 *   InputModule input("Prompt: ", " > ");
 *   input.set_placeholder("tapez quelque chose...");
 *   auto component = input.render();
 */
class InputModule {
public:
    /**
     * Constructeur du module input.
     * 
     * @param left_text Texte à afficher à gauche de la zone de saisie
     * @param right_text Texte à afficher à droite de la zone de saisie
     */
    InputModule(const std::string& left_text = "", const std::string& right_text = "");

    /**
     * Retourne le composant ftxui rendu pour l'intégration dans l'interface.
     * 
     * @return ftxui::Component composant interactif de saisie
     */
    ftxui::Component render();

    /**
     * Récupère le texte actuellement saisi par l'utilisateur.
     * 
     * @return std::string& référence au texte saisi
     */
    std::string& get_input();

    /**
     * Définit le texte à afficher à gauche de la zone de saisie.
     * 
     * @param text nouveau texte à gauche
     */
    void set_left_text(const std::string& text);

    /**
     * Définit le texte à afficher à droite de la zone de saisie.
     * 
     * @param text nouveau texte à droite
     */
    void set_right_text(const std::string& text);

    /**
     * Définit le texte d'espace réservé affiché quand la zone est vide.
     * 
     * @param text texte placeholder
     */
    void set_placeholder(const std::string& text);

    /**
     * Récupère le texte à gauche de la zone de saisie.
     * 
     * @return std::string texte à gauche
     */
    std::string get_left_text() const;

    /**
     * Récupère le texte à droite de la zone de saisie.
     * 
     * @return std::string texte à droite
     */
    std::string get_right_text() const;

private:
    std::string left_text_;     ///< Texte à gauche de l'input
    std::string right_text_;    ///< Texte à droite de l'input
    std::string input_text_;    ///< Texte saisi par l'utilisateur
    std::string placeholder_;   ///< Texte d'espace réservé
};
