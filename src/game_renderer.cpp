/**
 * @file game_renderer.cpp
 * @brief Implémentation des fonctions d'affichage (FTXUI + fallback console).
 *
 * Ce fichier contient les renderers et helpers utilises pour afficher
 * l'introduction, les menus, la sequence de boot et la boucle de jeu.
 */

#include "game_renderer.hpp"

#include <ctime>

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include "interface_graphique.hpp"
#include "UI_modules/module_de_commande.hpp"
#include "UI_modules/module_de_discussion.hpp"
#include "UI_modules/module_automatisation.hpp"

using namespace ftxui;

// === Renderers principaux ===

/**
 * @brief Affiche l'ecran d'introduction et attend la validation.
 * @return true si le joueur demande a commencer, false si quitte
 */
bool show_intro(const GameMeta& meta, InterfaceGraphique& interface) {
    if (interface.get_module_commande()) {
        return interface.get_module_commande()->afficher_intro(meta, interface);
    }

    return false;
}

void show_boot_sequence(const GamePhase& phase, GameState& state, InterfaceGraphique& interface) {
    // Delegate to module_de_commande if provided
    if (interface.get_module_commande()) {
        interface.get_module_commande()->afficher_boot_sequence(phase, state, interface);
    }
}

int show_choice_menu(
    const std::vector<GameChoice>& choices,
    InterfaceGraphique& interface,
    const std::string& titre_phase,
    const std::vector<std::string>& description_lignes,
    const std::string& titre_menu
) {
    if (choices.empty()) {
        return -1;
    }

    std::vector<std::string> labels;
    for (const auto& c : choices) {
        labels.push_back(c.label);
    }

    interface.get_module_commande()->definir_choix(titre_menu, labels, 0);
    bool quit = false;

    auto module = interface.get_module_commande();
    auto composant = interface.construire_racine();

    interface.afficher_composant(composant, [&](Event event) -> bool {
        if (event == Event::Character('q') || event == Event::Escape) {
            quit = true;
            interface.fermer_rendu();
            return true;
        }

        return false;
    });

    // Read selection from module after the composed render loop exits
    int selected = module->get_selection_courante();

    if (quit) {
        interface.get_module_commande()->effacer_choix();
        return -1;
    }

    interface.get_module_commande()->effacer_choix();

    return selected;
}


void show_message(const std::string& speaker, const std::string& message, InterfaceGraphique& interface) {
    if (interface.get_module_discussion()) {
        interface.get_module_discussion()->ajoute_son_message(speaker, message);
    }
}

Element render_message_with_effect(const std::string& message) {
    return paragraph("  " + message);
}

void show_phase_end(const std::vector<std::string>& messages, InterfaceGraphique& interface) {
    if (interface.get_module_commande()) {
        interface.get_module_commande()->afficher_phase_end(messages, interface);
    }
}

static void afficher_info(
    InterfaceGraphique& interface,
    const std::string& titre,
    const std::string& sous_titre,
    const std::vector<std::string>& description_lignes
) {

    auto corps = vbox({
        text("[ Entree ] Continuer") | dim | center,
    });

    auto page = Renderer([&] {
        return interface.RenderAvecEnTete(titre, sous_titre, description_lignes, corps);
    });

    interface.afficher_composant(page, [&](Event event) -> bool {
        if (event == Event::Return || event == Event::Escape) {
            interface.fermer_rendu();
            return true;
        }
        return false;
    });
}

void run_game(const GameMeta& meta, InterfaceGraphique& interface) {
    GameState state = init_game_state();

    std::shared_ptr<ModuleDeCommande> module_commande = interface.get_module_commande();
    std::shared_ptr<ModuleDeDiscussion> module_discussion = interface.get_module_discussion();
    std::shared_ptr<ModuleDeAutomatisation> module_automatisation = interface.get_module_automatisation();

    module_discussion->cacher();
    module_discussion->réinitialiser_discussion();
    module_automatisation->cacher();

    if (!show_intro(meta, interface)) {
        return;
    }

    // P0
    GamePhase phase0 = load_phase("P0", meta);

    show_boot_sequence(phase0, state, interface);

    module_discussion->afficher();
    module_automatisation->afficher();

    if (phase0.has_player_choice) {
        std::vector<std::string> desc;
        if (!phase0.description.empty()) {
            desc.push_back(phase0.description);
        }
        int choice_idx = show_choice_menu(phase0.player_choice_on_start.options, interface, phase0.label, desc, "Faites votre choix");

        if (choice_idx >= 0) {
            const auto& choice = phase0.player_choice_on_start.options[choice_idx];
            std::string response = choice.label;
            if (!choice.response_variants.empty()) {
                response = pick_text(choice.response_variants);
            }
            module_discussion->ajoute_mon_message(meta.player_name, response);
        }
    }

    // P1 et P3 : tâches + automs
    for (const std::string& phase_id : {"P1", "P3"}) {
        GamePhase phase = load_phase(phase_id, meta);
        std::vector<std::string> phase_desc;
        if (!phase.description.empty()) {
            phase_desc.push_back(phase.description);
        }
        //afficher_info(interface, phase.label, "Phase", phase_desc);

        for (const auto& task : phase.tasks) {
            Autom autom = load_autom(task.autom_ref, meta);

            std::vector<std::string> task_desc;
            task_desc.push_back("Sujet: " + task.topic);
            module_discussion->ajoute_son_message(meta.owner_name, pick_text(autom.owner_request_variants));

            std::vector<GameChoice> choices;

            for (const auto& option : autom.player_response_options) {
                GameChoice choice;
                choice.id = option.id;
                choice.label = option.label;
                choices.push_back(choice);
            }

            int choice_idx = show_choice_menu(choices, interface, phase.label, task_desc, "Reponse a choisir");

            if (choice_idx >= 0 && choice_idx < (int)autom.player_response_options.size()) {
                const auto& option = autom.player_response_options[choice_idx];

                module_discussion->ajoute_mon_message(meta.player_name, pick_text(option.variants));

                if (option.outcome != "no_autom") {
                    state.unlocked_automs.push_back(task.autom_ref);
                    module_automatisation->preparer_menu_depuis_unlocked(state.unlocked_automs, meta);
                    //afficher_info(interface, "Automatisation", "Debloquee", {"Autom debloquee : " + autom.label});
                } else {
                    //afficher_info(interface, "Automatisation", "Non debloquee", {"Autom non debloquee."});
                }
            }
        }

        if (phase.has_phase_end) {
            module_commande->afficher_phase_end(phase.phase_end.transition_messages, interface);
        }
    }

    // P2 : événements temps réel
    GamePhase phase2 = load_phase("P2", meta);

    std::vector<std::string> phase2_desc;
    if (!phase2.description.empty()) {
        phase2_desc.push_back(phase2.description);
    }
    afficher_info(interface, phase2.label, "Phase", phase2_desc);

    for (const auto& event : phase2.events) {
        std::vector<std::string> event_desc;
        event_desc.push_back("Contexte: " + event.context);
        module_discussion->ajoute_son_message(meta.owner_name, pick_text(event.owner_messages));

        std::vector<GameChoice> choices;

        for (const auto& autom_ref : state.unlocked_automs) {
            Autom autom = load_autom(autom_ref, meta);

            GameChoice choice;
            choice.id = autom_ref;
            choice.label = autom.label;
            choices.push_back(choice);
        }

        int choice_idx = show_choice_menu(choices, interface, phase2.label, event_desc, "Choix d'automatisation");

        if (choice_idx >= 0 && choice_idx < (int)choices.size()) {
            std::string selected_autom = choices[choice_idx].id;

            if (selected_autom == event.correct_autom_ref) {
                module_discussion->ajoute_son_message(meta.owner_name, pick_text(event.correct_responses));
            } else {
                module_discussion->ajoute_son_message("Systeme", "Mauvaise autom activee. " + meta.owner_name + " semble mecontent.");
            }
        }
    }

    if (phase2.has_phase_end) {
        show_phase_end(phase2.phase_end.transition_messages, interface);
    }

    // P4 : cambriolage
    GamePhase phase4 = load_phase("P4", meta);

    std::vector<std::string> phase4_desc;
    if (!phase4.description.empty()) {
        phase4_desc.push_back(phase4.description);
    }
    afficher_info(interface, phase4.label, "Phase", phase4_desc);

    if (!phase4.trigger_messages.empty()) {
        afficher_info(interface, phase4.label, "Declencheur", {pick_text(phase4.trigger_messages)});
    }

    int path_idx = show_choice_menu(phase4.path_choices, interface, phase4.label, {"Choisissez un chemin."}, "Decision");

    if (path_idx >= 0 && path_idx < (int)phase4.path_choices.size()) {
        const auto& choice = phase4.path_choices[path_idx];

        afficher_info(interface, phase4.label, "Decision", {"Decision: " + choice.label});

        if (choice.id == "path_1") {
            state.ending_id = "ending_normal";
        } else if (choice.id == "path_2") {
            state.ending_id = "ending_hero";
        } else if (choice.id == "path_3") {
            state.ending_id = "ending_opensaw_dark";
        }
    }

    // P5 : fin
    Ending ending = load_ending(state.ending_id, meta);

    afficher_info(interface, ending.label, "Final", {pick_text(ending.narration_variants)});

    if (!ending.owner_final_messages.empty()) {
        module_discussion->ajoute_son_message(meta.owner_name, pick_text(ending.owner_final_messages));
    }

    if (!ending.player_final_responses.empty()) {
        module_discussion->ajoute_mon_message(meta.player_name, pick_text(ending.player_final_responses));
    }

    afficher_info(interface, "Fin", "", {"Merci d'avoir joue !"});
}