#include "interface_graphique.hpp"

using namespace ftxui;


std::bitset<MODULE_BIT_COUNT> init_modules(){
    std::bitset<MODULE_BIT_COUNT> uiModules;
    uiModules.set(INPUT);
    uiModules.reset(CHAT);
    uiModules.reset(AUTOMATIONS);
    uiModules.reset(VIDEO);
    return uiModules;
}

void display_modules(std::bitset<MODULE_BIT_COUNT> uiModules,) {
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

    //affiche les élément ftxui sur le terminal
    auto affichage = ScreenInteractive::Fullscreen();
    affichage.Loop(main_layout);
}