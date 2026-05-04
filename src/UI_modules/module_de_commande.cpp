/**
 * @file module_de_commande.cpp
 * @brief Implementation de `ModuleDeCommande` : affichage et gestion des choix.
 */

#include "module_de_commande.hpp"
#include "../interface_graphique.hpp"

#include <chrono>
#include <ctime>
#include <thread>
#include <fstream>

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
 * @brief Valide un choix courant (methode conservee pour compatibilite).
 * @param choix Libelle du choix qui a ete valide
 */
void ModuleDeCommande::valider_choix(const std::string& choix) {
    (void)choix;
}

/**
 * @brief Annule le choix courant (methode conservee pour compatibilite).
 */
void ModuleDeCommande::annuler_choix() {
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
        std::ofstream lg("/tmp/bonhomme-debug.log", std::ios::app);
        lg << "[boot_sequence] animation thread started; messages=" << phase.boot_sequence.system_messages.size() << "\n";
        lg.close();
        while (msg_index < (int)phase.boot_sequence.system_messages.size()) {
            const std::string& message = phase.boot_sequence.system_messages[msg_index];
            current_message.clear();

            for (char c : message) {
                current_message += c;
                interface_graphique.poster_evenement(Event::Custom);
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }

            state.current_message_index = msg_index;
            ++msg_index;
            {
                std::ofstream lg2("/tmp/bonhomme-debug.log", std::ios::app);
                lg2 << "[boot_sequence] completed message index=" << msg_index << "\n";
                lg2.close();
            }
            interface_graphique.poster_evenement(Event::Custom);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    });

    interface_graphique.afficher_composant(renderer, [&](Event event) -> bool {
        std::ofstream lg3("/tmp/bonhomme-debug.log", std::ios::app);
        if (event == Event::Return) lg3 << "[boot_sequence] handler event=Return\n";
        else if (event == Event::Escape) lg3 << "[boot_sequence] handler event=Escape\n";
        else lg3 << "[boot_sequence] handler event=Other\n";
        lg3.close();
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

void ModuleDeCommande::afficher_phase_end(const std::vector<std::string>& messages, InterfaceGraphique& interface_graphique) {
    if (messages.empty()) {
        return;
    }

    srand((unsigned)time(nullptr));
    int idx = rand() % messages.size();
    std::string message = messages[idx];

    std::vector<std::string> description_lignes;
    description_lignes.push_back(message);

    auto corps = vbox({
        text("[ Appuyez sur ENTREE pour continuer ]") | bold | center,
    });

    interface_graphique.afficher_page(
        "Transition",
        "",
        description_lignes,
        corps,
        [&](Event event) -> bool {
            if (event == Event::Return) {
                interface_graphique.fermer_rendu();
                return true;
            }
            return false;
        }
    );
}

/**
 * @brief Produit le rendu FTXUI du module de commande.
 * @return Element FTXUI contenant la liste de choix
 */
Element ModuleDeCommande::Render() const {
    Elements lignes;

    if (_choix_courants.empty()) {
        lignes.push_back(text("Aucun choix en cours") | dim);
    } else {
        if (_titre_choix.empty()) {
            lignes.push_back(text("Choisissez une action") | bold);
        } else {
            lignes.push_back(text(_titre_choix) | bold);
        }
        lignes.push_back(separator());

        for (int i = 0; i < (int)_choix_courants.size(); ++i) {
            const bool est_selection = i == _selection_courante;
            const std::string prefixe = est_selection ? "> " : "  ";
            lignes.push_back(text(prefixe + _choix_courants[i]));
        }
    }

    return encadrer_avec_titre("Commande", vbox(std::move(lignes)));
}

ftxui::Component ModuleDeCommande::MakeComponent() {
    // If choices are present, expose a Menu component bound to internal state
    if (!_choix_courants.empty()) {
        auto menu = Menu(&_choix_courants, &_selection_courante);

        return Renderer(menu, [&] {
            Elements lignes;

            if (_titre_choix.empty()) {
                lignes.push_back(text("Choisissez une action") | bold);
            } else {
                lignes.push_back(text(_titre_choix) | bold);
            }
            lignes.push_back(separator());
            lignes.push_back(menu->Render());

            return encadrer_avec_titre("Commande", vbox(std::move(lignes)));
        });
    }

    // Fallback: render as static element
    return Renderer([&] { return Render(); });
}