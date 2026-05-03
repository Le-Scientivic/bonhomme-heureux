#include "game_renderer.hpp"
#include "UI_modules/input.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>

using namespace ftxui;

// Helper: Effet typewriter 

void print_with_delay(const std::string& text, int delay_ms = 30) {
    for (char c : text) {
        std::cout << c;
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    std::cout << std::endl;
}

// Implémentation du renderer

bool show_intro(const GameMeta& meta) {
    auto screen = ScreenInteractive::TerminalOutput();

    bool start_game = false;
    bool quit = false;

    auto title = Renderer([&meta]() {
        return vbox({
            text("  ╔═══════════════════════════════════════╗") | center,
            text("  ║                                       ║") | center,
            text("  ║         " + meta.title + "                ║") | bold | center,
            text("  ║         v" + meta.version + "                     ║") | center,
            text("  ║                                       ║") | center,
            text("  ╚═══════════════════════════════════════╝") | center,
        }) | border;
    });

    auto description = Renderer([&meta]() {
        Elements lines;
        lines.push_back(text("") | center);
        lines.push_back(text(meta.intro_text_subtitle) | center);
        lines.push_back(text(""));

        std::string body = meta.intro_text_body;
        std::string line;

        for (size_t i = 0; i < body.size(); i++) {
            if (body[i] == '\n' || line.size() > 60) {
                lines.push_back(text("  " + line));
                line.clear();
            } else {
                line += body[i];
            }
        }

        if (!line.empty()) {
            lines.push_back(text("  " + line));
        }

        lines.push_back(text(""));
        lines.push_back(text("  Appuyez sur [ENTREE] pour commencer") | dim | center);

        return vbox(lines) | border;
    });

    auto menu = Container::Vertical({
        title,
        description,
    });

    auto caught = CatchEvent(menu, [&](Event event) -> bool {
        if (event == Event::Return) {
            start_game = true;
            screen.ExitLoopClosure()();
            return true;
        }

        if (event == Event::Character('q') || event == Event::Escape) {
            quit = true;
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });

    auto renderer = Renderer(caught, [&] {
        return vbox({
            title->Render(),
            description->Render(),
        }) | center;
    });

    screen.Loop(renderer);

    return start_game && !quit;
}

void show_boot_sequence(const GamePhase& phase, GameState& state) {
    if (!phase.has_boot || phase.boot_sequence.system_messages.empty()) {
        return;
    }

    auto screen = ScreenInteractive::TerminalOutput();

    std::string current_message = "";
    int msg_index = 0;

    auto renderer = Renderer([&]() {
        Elements lines;
        lines.push_back(text("  ════════════════════════════════════════") | dim | center);

        for (int i = 0; i < msg_index && i < (int)phase.boot_sequence.system_messages.size(); i++) {
            lines.push_back(text("  > " + phase.boot_sequence.system_messages[i]) | dim);
        }

        if (msg_index < (int)phase.boot_sequence.system_messages.size()) {
            lines.push_back(text("  > " + current_message) | ftxui::color(Color::Cyan));
            lines.push_back(text("  █") | blink | ftxui::color(Color::Cyan));
        } else {
            lines.push_back(text(""));
            lines.push_back(text("  [ Appuyez sur ENTREE pour continuer ]") | bold | center);
        }

        lines.push_back(text("  ════════════════════════════════════════") | dim | center);

        return vbox(lines) | border;
    });

    auto component = CatchEvent(renderer, [&](Event event) -> bool {
        if (event == Event::Return && msg_index >= (int)phase.boot_sequence.system_messages.size()) {
            screen.ExitLoopClosure()();
            return true;
        }

        if (event == Event::Escape) {
            msg_index = (int)phase.boot_sequence.system_messages.size();
            current_message.clear();
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });

    std::thread animation([&]() {
        while (msg_index < (int)phase.boot_sequence.system_messages.size()) {
            const std::string& msg = phase.boot_sequence.system_messages[msg_index];
            current_message.clear();

            for (char c : msg) {
                current_message += c;
                screen.PostEvent(Event::Custom);
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }

            state.current_message_index = msg_index;
            msg_index++;
            screen.PostEvent(Event::Custom);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    });

    screen.Loop(component);

    if (animation.joinable()) {
        animation.join();
    }
}

int show_choice_menu(const std::vector<GameChoice>& choices) {
    if (choices.empty()) {
        return -1;
    }

    auto screen = ScreenInteractive::TerminalOutput();

    std::vector<std::string> labels;
    for (const auto& c : choices) {
        labels.push_back(c.label);
    }

    int selected = 0;
    bool confirmed = false;
    bool quit = false;

    auto menu = Menu(&labels, &selected);

    auto caught = CatchEvent(menu, [&](Event event) -> bool {
        if (event == Event::Return) {
            confirmed = true;
            screen.ExitLoopClosure()();
            return true;
        }

        if (event == Event::Character('q') || event == Event::Escape) {
            quit = true;
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });

    auto renderer = Renderer(caught, [&] {
        return vbox({
            text("  Faites votre choix:  ") | bold | center,
            separator(),
            caught->Render(),
            separator(),
            text("  [ haut/bas ] Naviguer   [ Entree ] Choisir   [ q ] Quitter") | dim | center,
        }) | border;
    });

    screen.Loop(renderer);

    if (quit || !confirmed) {
        return -1;
    }

    return selected;
}

std::string show_input(const std::string& prompt) {
    InputModule input(prompt, " > ");
    input.set_placeholder("Tapez votre message...");

    auto component = input.render();
    auto screen = ScreenInteractive::TerminalOutput();

    std::string result;

    auto caught = CatchEvent(component, [&](Event event) -> bool {
        if (event == Event::Return) {
            result = input.get_input();
            screen.ExitLoopClosure()();
            return true;
        }

        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });

    auto renderer = Renderer(caught, [&] {
        return vbox({
            text("  Tapez un message:  ") | bold | center,
            separator(),
            component->Render(),
            separator(),
            text("  [ Entree ] Envoyer   [ Echap ] Annuler") | dim | center,
        }) | border;
    });

    screen.Loop(renderer);

    return result;
}

void show_message(const std::string& speaker, const std::string& message) {
    std::cout << "\n  " << speaker << ": ";
    print_with_delay(message, 20);
}

Element render_message_with_effect(const std::string& message) {
    return paragraph("  " + message);
}

void show_phase_end(const std::vector<std::string>& messages) {
    if (messages.empty()) {
        return;
    }

    auto screen = ScreenInteractive::TerminalOutput();

    srand((unsigned)time(nullptr));
    int idx = rand() % messages.size();
    std::string message = messages[idx];

    auto renderer = Renderer([&message]() {
        return vbox({
            text("  ════════════════════════════════════════") | dim | center,
            text(""),
            text("  " + message) | ftxui::color(Color::Cyan) | center,
            text(""),
            text("  [ Appuyez sur ENTREE pour continuer ]") | bold | center,
            text("  ════════════════════════════════════════") | dim | center,
        }) | border;
    });

    auto caught = CatchEvent(renderer, [&](Event event) -> bool {
        if (event == Event::Return) {
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });

    screen.Loop(caught);
}

void run_game(const GameMeta& meta) {
    GameState state = init_game_state();

    if (!show_intro(meta)) {
        std::cout << "\n  Jeu quitté.\n";
        return;
    }

    // P0
    GamePhase phase0 = load_phase("P0", meta);
    show_boot_sequence(phase0, state);

    if (phase0.has_player_choice) {
        int choice_idx = show_choice_menu(phase0.player_choice_on_start.options);

        if (choice_idx >= 0) {
            std::cout << "\n  [Vous]: "
                      << phase0.player_choice_on_start.options[choice_idx].label
                      << std::endl;
        }
    }

    // P1 et P3 : tâches + automs
    for (const std::string& phase_id : {"P1", "P3"}) {
        GamePhase phase = load_phase(phase_id, meta);

        std::cout << "\n  === " << phase.label << " ===\n";
        std::cout << "  " << phase.description << "\n\n";

        for (const auto& task : phase.tasks) {
            Autom autom = load_autom(task.autom_ref, meta);

            std::cout << "\n  [" << task.topic << "]\n";
            std::cout << "  " << pick_text(autom.owner_request_variants) << "\n\n";

            std::vector<GameChoice> choices;

            for (const auto& option : autom.player_response_options) {
                GameChoice choice;
                choice.id = option.id;
                choice.label = option.label;
                choices.push_back(choice);
            }

            int choice_idx = show_choice_menu(choices);

            if (choice_idx >= 0 && choice_idx < (int)autom.player_response_options.size()) {
                const auto& option = autom.player_response_options[choice_idx];

                std::cout << "\n  [Vous]: " << pick_text(option.variants) << "\n";

                if (option.outcome != "no_autom") {
                    state.unlocked_automs.push_back(task.autom_ref);
                    std::cout << "  Autom débloquée : " << autom.label << "\n";
                } else {
                    std::cout << "  Autom non débloquée.\n";
                }
            }
        }

        if (phase.has_phase_end) {
            show_phase_end(phase.phase_end.transition_messages);
        }
    }

    // P2 : événements temps réel
    GamePhase phase2 = load_phase("P2", meta);

    std::cout << "\n  === " << phase2.label << " ===\n";
    std::cout << "  " << phase2.description << "\n\n";

    for (const auto& event : phase2.events) {
        std::cout << "\n  Contexte : " << event.context << "\n";
        std::cout << "  " << pick_text(event.owner_messages) << "\n\n";

        std::vector<GameChoice> choices;

        for (const auto& autom_ref : state.unlocked_automs) {
            Autom autom = load_autom(autom_ref, meta);

            GameChoice choice;
            choice.id = autom_ref;
            choice.label = autom.label;
            choices.push_back(choice);
        }

        int choice_idx = show_choice_menu(choices);

        if (choice_idx >= 0 && choice_idx < (int)choices.size()) {
            std::string selected_autom = choices[choice_idx].id;

            if (selected_autom == event.correct_autom_ref) {
                std::cout << "\n  " << pick_text(event.correct_responses) << "\n";
            } else {
                std::cout << "\n  Mauvaise autom activée. "
                          << meta.owner_name
                          << " semble mécontent.\n";
            }
        }
    }

    if (phase2.has_phase_end) {
        show_phase_end(phase2.phase_end.transition_messages);
    }

    // P4 : cambriolage
    GamePhase phase4 = load_phase("P4", meta);

    std::cout << "\n  === " << phase4.label << " ===\n";
    std::cout << "  " << phase4.description << "\n\n";

    if (!phase4.trigger_messages.empty()) {
        std::cout << "  " << pick_text(phase4.trigger_messages) << "\n\n";
    }

    int path_idx = show_choice_menu(phase4.path_choices);

    if (path_idx >= 0 && path_idx < (int)phase4.path_choices.size()) {
        const auto& choice = phase4.path_choices[path_idx];

        std::cout << "\n  [Décision]: " << choice.label << "\n";

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

    std::cout << "\n  === " << ending.label << " ===\n\n";
    std::cout << "  " << pick_text(ending.narration_variants) << "\n\n";

    if (!ending.owner_final_messages.empty()) {
        std::cout << "  [" << meta.owner_name << "]: "
                  << pick_text(ending.owner_final_messages) << "\n";
    }

    if (!ending.player_final_responses.empty()) {
        std::cout << "  [" << meta.player_name << "]: "
                  << pick_text(ending.player_final_responses) << "\n";
    }

    std::cout << "\n  Merci d'avoir joué !\n";
}