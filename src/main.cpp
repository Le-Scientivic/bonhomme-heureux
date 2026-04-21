#include <string>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

int main() {
    auto screen = ScreenInteractive::TerminalOutput();

    // Choices shown in the menu
    const std::vector<std::string> choices = {
        "A  —  Le chemin de la sagesse",
        "B  —  Le chemin de l'aventure",
        "C  —  Le chemin du mystère",
    };

    // Messages displayed after each choice
    const std::vector<std::string> messages = {
        "Le bonhomme médite sous un arbre et trouve enfin la paix intérieure. Il est heureux. 🌿",
        "Le bonhomme part explorer des contrées inconnues, le sourire aux lèvres. Il est heureux. 🗺️",
        "Le bonhomme disparaît dans la brume... personne ne sait où il est allé. Il est heureux. 🌫️",
    };

    int selected = 0;
    bool confirmed = false;

    auto menu = Menu(&choices, &selected);

    // Wrap the menu to intercept Enter → confirm; 'q' → quit
    auto interactive = CatchEvent(menu, [&](Event event) {
        if (event == Event::Return) {
            confirmed = true;
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    auto renderer = Renderer(interactive, [&] {
        return vbox({
                   text("✨  Bonjour, choisissez votre chemin !  ✨") | bold | center,
                   separator(),
                   interactive->Render() | center,
                   separator(),
                   text("[ Entrée ] Valider   [ q ] Quitter") | dim | center,
               }) |
               border;
    });

    screen.Loop(renderer);

    if (confirmed) {
        auto document = vbox({
                            text("Vous avez choisi : " + choices[selected]) | bold | center,
                            separator(),
                            paragraph(messages[selected]) | center,
                        }) |
                        border;
        auto result_screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
        Render(result_screen, document);
        result_screen.Print();
    }

    return 0;
}
