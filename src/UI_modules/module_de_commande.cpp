/**
 * @file module_de_commande.cpp
 * @brief Implementation de `ModuleDeCommande` : affichage et gestion des choix.
 */

#include "module_de_commande.hpp"
#include "../interface_graphique.hpp"

#include <thread>

using namespace ftxui;

/**
 * @brief Constructeur par defaut.
 */
ModuleDeCommande::ModuleDeCommande() = default;

/**
 * @brief Definit le titre et la liste de choix affiches au joueur.
 * @param titre Titre du groupe de choix
 * @param choix Liste de libelles affichables
 * @param selection Index de la selection initiale (-1 si aucune)
 */
void ModuleDeCommande::definir_choix(const std::string& titre, const std::vector<std::string>& choix, int selection) {
    _titre_choix = titre;
    _choix_courants = choix;
    _selection_courante = selection;
}

/**
 * @brief Efface l'etat du choix courant (titre, options, selection).
 */
void ModuleDeCommande::effacer_choix() {
    _titre_choix.clear();
    _choix_courants.clear();
    _selection_courante = -1;
}

int ModuleDeCommande::get_selection_courante() const {
    return _selection_courante;
}

void ModuleDeCommande::afficher_boot_sequence(const GamePhase& phase, GameState& state, InterfaceGraphique& interface_graphique) {
    if (!phase.has_boot || phase.boot_sequence.system_messages.empty()) {
        return;
    }

    std::string current_message;
    int msg_index = 0;

    auto renderer = Renderer([&]() {
        Elements lignes;
        lignes.push_back(text("Boot sequence") | bold | center);
        lignes.push_back(separator());

        for (int i = 0; i < msg_index && i < (int)phase.boot_sequence.system_messages.size(); ++i) {
            lignes.push_back(text("> " + phase.boot_sequence.system_messages[i]) | dim);
        }

        if (msg_index < (int)phase.boot_sequence.system_messages.size()) {
            lignes.push_back(text("> " + current_message) | color(Color::Cyan));
            lignes.push_back(text("█") | blink | color(Color::Cyan));
        } else {
            lignes.push_back(text(""));
            lignes.push_back(text("[ Appuyez sur ENTREE pour continuer ]") | bold | center);
        }

        std::vector<std::string> description_lignes;
        if (!phase.description.empty()) {
            description_lignes.push_back(phase.description);
        }

        return interface_graphique.RenderAvecEnTete(
            phase.label,
            "Initialisation",
            description_lignes,
            vbox(std::move(lignes))
        );
    });

    std::thread animation([&]() {
        while (msg_index < (int)phase.boot_sequence.system_messages.size()) {
            const std::string& message = phase.boot_sequence.system_messages[msg_index];
            current_message.clear();

            for (char c : message) {
                current_message += c;
                interface_graphique.poster_evenement(Event::Custom);
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }

            ++msg_index;
            interface_graphique.poster_evenement(Event::Custom);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    });

    interface_graphique.afficher_composant(renderer, [&](Event event) -> bool {
        if (event == Event::Return && msg_index >= (int)phase.boot_sequence.system_messages.size()) {
            interface_graphique.fermer_rendu();
            return true;
        }

        if (event == Event::Escape) {
            msg_index = (int)phase.boot_sequence.system_messages.size();
            current_message.clear();
            interface_graphique.fermer_rendu();
            return true;
        }

        return false;
    });

    if (animation.joinable()) {
        animation.join();
    }
}

bool ModuleDeCommande::afficher_intro(const GameMeta& meta, InterfaceGraphique& interface_graphique) {
    bool start_game = false;
    bool quit = false;

    std::vector<std::string> description_lignes;
    if (!meta.intro_text_body.empty()) {
        description_lignes.push_back(meta.intro_text_body);
    }

    auto renderer = Renderer([&]() {
        return interface_graphique.RenderAvecEnTete(
            meta.title,
            meta.version,
            description_lignes,
            vbox({
                text(meta.intro_text_subtitle) | center,
                separator(),
                text("Appuyez sur [ENTREE] pour commencer") | dim | center,
            })
        );
    });

    interface_graphique.afficher_composant(renderer, [&](Event event) -> bool {
        if (event == Event::Return) {
            start_game = true;
            interface_graphique.fermer_rendu();
            return true;
        }

        if (event == Event::Character('q') || event == Event::Escape) {
            quit = true;
            interface_graphique.fermer_rendu();
            return true;
        }

        return false;
    });

    return start_game && !quit;
}

Element ModuleDeCommande::_render_frame(Element corps) const {
    const std::string titre = _titre_choix.empty() ? "Choisissez une action" : _titre_choix;
    return encadrer_avec_titre("Commande", vbox({
        text(titre) | bold,
        separator(),
        std::move(corps),
    })) | color(Color::GreenLight);
}

Element ModuleDeCommande::Render() const {
    if (_choix_courants.empty()) {
        return encadrer_avec_titre("Commande",
            vbox({text("Aucun choix en cours") | dim})) | color(Color::GreenLight);
    }

    Elements items;
    for (int i = 0; i < (int)_choix_courants.size(); ++i) {
        const bool est_selection = i == _selection_courante;
        const std::string prefixe = est_selection ? "> " : "  ";
        items.push_back(text(prefixe + _choix_courants[i]));
    }
    return _render_frame(vbox(std::move(items)));
}

ftxui::Component ModuleDeCommande::MakeComponent(InterfaceGraphique& interface_graphique) {
    if (_choix_courants.empty()) {
        return Renderer([this] { return Render(); });
    }

    MenuOption option;
    option.on_enter = [&interface_graphique] {
        interface_graphique.fermer_rendu();
    };
    auto menu = Menu(&_choix_courants, &_selection_courante, option);
    return Renderer(menu, [this, menu] {
        return _render_frame(menu->Render());
    });
}