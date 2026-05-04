#pragma once

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"

/**
 * @file module_de_discussion.hpp
 * @brief Module d'affichage et d'historique de la discussion entre joueur et interlocuteur.
 */
class ModuleDeDiscussion : public Module {
public:
    /**
     * @brief Constructeur: initialise un historique vide.
     */
    ModuleDeDiscussion();

    /**
     * @brief Ajoute un message emis par le joueur dans l'historique.
     * @param mon_nom Nom du joueur
     * @param mon_message Contenu du message
     */
    void ajoute_mon_message(const std::string& mon_nom, const std::string& mon_message);

    /**
     * @brief Ajoute un message emis par l'interlocuteur.
     * @param son_nom Nom de l'interlocuteur
     * @param son_message Contenu du message
     */
    void ajoute_son_message(const std::string& son_nom, const std::string& son_message);

    /**
     * @brief Reinitialise completement l'historique de discussion.
     */
    void réinitialiser_discussion();

    /**
     * @brief Rendement FTXUI du module de conversation.
     * @return Element representant la vue conversation
     */
    ftxui::Element Render() const override;

    /**
     * @brief Construit le composant interactif (slider).
     */
    ftxui::Component MakeComponent(InterfaceGraphique& interface_graphique) override;

private:
    /**
     * @brief Representation interne d'un message de discussion.
     */
    struct Message {
        /// Nom de l'emetteur.
        std::string nom;
        /// Contenu textuel du message.
        std::string texte;
        /// Indique si le message appartient au joueur.
        bool est_a_moi = false;
    };

    /// Historique ordonne des messages de discussion.
    std::vector<Message> _messages;

    /// Etat du slider d'option d'affichage.
    int _slider_value = 50;
    int _slider_min = 0;
    int _slider_max = 100;
    std::string _slider_label = "Zoom discussion";

    ftxui::Element RenderMessages() const;
};