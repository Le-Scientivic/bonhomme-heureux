#pragma once

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"

/// Module qui gere l'affichage de la discussion entre le joueur et l'autre partie.
class ModuleDeDiscussion : public Module {
public:
    /// Construit le module de discussion avec un historique vide.
    ModuleDeDiscussion();

    /// Ajoute un message emis par le joueur.
    void ajoute_mon_message(const std::string& mon_nom, const std::string& mon_message);
    /// Ajoute un message emis par l'interlocuteur.
    void ajoute_son_message(const std::string& son_nom, const std::string& son_message);
	/// Reinitialise completement l'historique de discussion.
    void réinitialiser_discussion();

    /// Produit le rendu FTXUI de la conversation.
    ftxui::Element Render() const override;

private:
    /// Representation interne d'un message de discussion.
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
};