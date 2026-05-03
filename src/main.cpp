#include <algorithm>
#include <iostream>

#include "game_engine.hpp"
#include "game_renderer.hpp"

int main() {
    init_modules();
    display_modules();

    /*
    auto scenes  = make_scenes();
    auto screen  = ScreenInteractive::TerminalOutput();
    State state;
    int   current = 0;

    while (true) {
        if (current == -1) {
            show_ending(compute_ending(state), state);
            break;
        }

        auto it = scenes.find(current);
        if (it == scenes.end()) {
            show_ending(compute_ending(state), state);
            break;
        }

        int choice = run_scene(screen, it->second, state);
        if (choice < 0) break;

        const auto& c = it->second.choices[choice];
        state.apply(c.dh, c.de, c.dp);
        current = c.next_scene;
    }*/

    return 0;
}
