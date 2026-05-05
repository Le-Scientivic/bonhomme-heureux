#include "module_de_discussion.hpp"

#include <algorithm>

using namespace ftxui;

ModuleDeDiscussion::ModuleDeDiscussion() = default;

void ModuleDeDiscussion::ajoute_mon_message(const std::string& mon_nom, const std::string& mon_message) {
    _messages.push_back(Message{mon_nom, mon_message, true});
}

void ModuleDeDiscussion::ajoute_son_message(const std::string& son_nom, const std::string& son_message) {
    _messages.push_back(Message{son_nom, son_message, false});
}

void ModuleDeDiscussion::réinitialiser_discussion() {
    _messages.clear();
}

Element ModuleDeDiscussion::Render() const {
    // Construire l'affichage statique (utilisé par le layout global)
    std::vector<Element> lignes;
    lignes.reserve(_messages.size());

    for (const auto& message : _messages) {
        auto contenu = vbox({
            text(message.nom) | bold,
            paragraph(message.texte),
        }) | border | size(WIDTH, LESS_THAN, 60);

        if (message.est_a_moi) {
            lignes.push_back(hbox({filler(), contenu}));
        } else {
            lignes.push_back(hbox({contenu, filler()}));
        }
    }

    if (lignes.empty()) {
        lignes.push_back(paragraph("Aucun message pour le moment"));
    }

    // Limiter la hauteur pour forcer l'utilisation du scroll
    return encadrer_avec_titre("Discussion", vbox(std::move(lignes)) | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 12)) | color(Color::BlueLight);
}

Component ModuleDeDiscussion::MakeComponent(InterfaceGraphique& interface_graphique) {
    (void)interface_graphique;

    // --- Création du renderer principal ---
    auto renderer = Renderer([this] {
        // --- Création des éléments enfants pour chaque message ---
        std::vector<Element> lignes;
        lignes.reserve(_messages.size());

        for (const auto& message : _messages) {
            auto contenu = vbox({
                text(message.nom) | bold,
                paragraph(message.texte),
            }) | border | size(WIDTH, LESS_THAN, 60);

            if (message.est_a_moi) {
                lignes.push_back(hbox({filler(), contenu}));
            } else {
                lignes.push_back(hbox({contenu, filler()}));
            }
        }

        if (lignes.empty()) {
            lignes.push_back(paragraph("Aucun message pour le moment"));
        }

        // --- Affichage avec vscroll_indicator + frame et hauteur limitée ---
        return encadrer_avec_titre("Discussion",
            vbox(std::move(lignes)) | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 12)) | color(Color::BlueLight);
    });

    return renderer;
}