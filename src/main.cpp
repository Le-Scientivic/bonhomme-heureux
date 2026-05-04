#include "game_engine.hpp"
#include "game_renderer.hpp"
#include "interface_graphique.hpp"
#include "UI_modules/module_automatisation.hpp"
#include "UI_modules/module_de_commande.hpp"
#include "UI_modules/module_de_discussion.hpp"

int main() {
    InterfaceGraphique interface_graphique;

    auto module_automatisation = std::shared_ptr<ModuleDeAutomatisation>(new ModuleDeAutomatisation());
    auto module_commande = std::shared_ptr<ModuleDeCommande>(new ModuleDeCommande());
    auto module_discussion = std::shared_ptr<ModuleDeDiscussion>(new ModuleDeDiscussion());

    interface_graphique.ajouter_module(module_automatisation);
    interface_graphique.ajouter_module(module_commande);
    interface_graphique.ajouter_module(module_discussion);

    UIRenderContext ui_context{};
    ui_context.interface_graphique = &interface_graphique;
    ui_context.module_automatisation = module_automatisation;
    ui_context.module_commande = module_commande;
    ui_context.module_discussion = module_discussion;

    // Charger les métadonnées du jeu depuis JSON
    GameMeta meta = load_game_meta();
    
    // Exécuter le jeu avec l'histoire
    run_game(meta, &ui_context);
    
    return 0;
}
