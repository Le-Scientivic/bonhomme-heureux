#include "interface_graphique.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

std::bitset<MODULE_BIT_COUNT> uiModules;

void init_modules(){
    uiModules.set(INPUT);
    uiModules.reset(CHAT);
    uiModules.reset(AUTOMATIONS);
    uiModules.reset(VIDEO);
}

void display_modules() {
    bool isInputEnabled = uiModules.test(INPUT);

    // Créer une instance du module input
    InputModule input_module("Vous: ", " > ");
    input_module.set_placeholder("Tapez votre message...");

    // Créer le composant input rendu
    auto input_component = input_module.render();

    // Créer le layout principal avec le module input
    auto main_layout = Renderer(input_component, [&] {
        return vbox({
            text("Bienvenue dans l'interface modulaire") | bold | center,
            separator(),
            text("Module INPUT activé") | center,
            separator(),
            input_component->Render(),
        }) | border;
    });

    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(main_layout);
}