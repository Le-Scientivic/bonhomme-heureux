#include "game_engine.hpp"
#include "game_renderer.hpp"
#include "interface_graphique.hpp"
#include "UI_modules/module_automatisation.hpp"
#include "UI_modules/module_de_commande.hpp"
#include "UI_modules/module_de_discussion.hpp"

int main() {
    auto module_automatisation = std::make_shared<ModuleDeAutomatisation>();
    auto module_commande = std::make_shared<ModuleDeCommande>();
    auto module_discussion = std::make_shared<ModuleDeDiscussion>();

    auto interface = std::make_shared<InterfaceGraphique>(
        module_automatisation,
        module_commande,
        module_discussion
    );

    // Charger les métadonnées du jeu depuis JSON
    GameMeta meta = load_game_meta();
    
    // Exécuter le jeu avec l'histoire
    run_game(meta, *interface);
    
    return 0;
}
