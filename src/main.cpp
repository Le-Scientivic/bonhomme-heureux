#include <iostream>

#include "game_engine.hpp"
#include "game_renderer.hpp"

int main() {
    // Charger les métadonnées du jeu depuis JSON
    GameMeta meta = load_game_meta();
    
    // Exécuter le jeu avec l'histoire
    run_game(meta);
    
    return 0;
}
