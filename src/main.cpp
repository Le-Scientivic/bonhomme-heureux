#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

// ─── État du joueur ──────────────────────────────────────────────────────────

struct State {
    int humeur      = 50;   // 0-100
    int energie     = 50;   // 0-100
    int ponctualite = 100;  // 0-100

    void apply(int dh, int de, int dp) {
        humeur      = std::clamp(humeur      + dh, 0, 100);
        energie     = std::clamp(energie     + de, 0, 100);
        ponctualite = std::clamp(ponctualite + dp, 0, 100);
    }
};

// ─── Structure d'une scène ───────────────────────────────────────────────────

struct Choice {
    std::string label;
    int         next_scene;  // -1 = calculer la fin
    int         dh, de, dp;  // deltas humeur / énergie / ponctualité
};

struct Scene {
    std::string              title;
    std::vector<std::string> desc;
    std::vector<Choice>      choices;
};

// ─── Structure d'une fin ─────────────────────────────────────────────────────

struct Ending {
    std::string              icon;
    std::string              title;
    std::vector<std::string> lines;
};

// ─── Définition des scènes ───────────────────────────────────────────────────

std::map<int, Scene> make_scenes() {
    return {
        // 0 : Réveil
        {0, {
            "🌅  Le réveil de Michel  —  Il est 7h30",
            {
                "DRIIIIING !",
                "Le réveil de Michel hurle à pleins poumons.",
                "Il est 7h30. Le boulot commence à 9h.",
                "",
                "Michel contemple le plafond en clignant des yeux.",
                "Que fait-il ?",
            },
            {
                {"🦸  Se lever d'un bond !",              1, +10,  +5,   0},
                {"😴  Encore 5 petites minutes...",       2,  -5,  -5, -20},
                {"💀  Éteindre le réveil et se rendormir", -1, -40, -20, -80},
            }
        }},

        // 1 : Petit déjeuner (bien réveillé, plein de temps)
        {1, {
            "☀️  Cuisine  —  7h32  (1h28 avant le boulot)",
            {
                "Michel est debout, frais (ou presque).",
                "Il a le temps. Sa cuisine l'attend.",
                "",
                "Que mange Michel ce matin ?",
            },
            {
                {"🍳  Œufs brouillés et café maison", 3, +20, +30,  -5},
                {"🥣  Céréales avec du lait",          3, +10, +15,   0},
                {"☕  Juste un café, pas faim",         4,  -5, -15, +10},
            }
        }},

        // 2 : Petit déjeuner (levé en retard)
        {2, {
            "⏰  Cuisine  —  7h55  (1h05 avant le boulot — vite vite !)",
            {
                "Michel ouvre un œil. Il est 7h55. Merde.",
                "Il a gaspillé 25 précieuses minutes sous la couette.",
                "",
                "Il lui reste peu de temps. Que mange-t-il ?",
            },
            {
                {"🥣  Céréales en moins de 2 minutes",       5,  +5, +10, -10},
                {"☕  Juste un café debout à la cuisine",     5,   0, -10,   0},
                {"🍳  Des œufs (tant pis pour l'heure !)",  -1, +15, +25, -60},
            }
        }},

        // 3 : Transport (bien nourri, bien réveillé)
        {3, {
            "🏠  Départ  —  8h15  (45 min avant le boulot)",
            {
                "Michel, rassasié et de bonne humeur, enfile sa veste.",
                "Il sort dans la rue lumineuse.",
                "",
                "Comment va-t-il au boulot ?",
            },
            {
                {"🚲  À vélo — rapide et vivifiant",  -1, +15,  -5, +10},
                {"🚌  En bus — tranquille et fiable", -1,  +5,  +5,   0},
                {"🚶  À pied — ça fait du bien",      -1, +10, -10, -15},
            }
        }},

        // 4 : Transport (pas nourri, bien réveillé)
        {4, {
            "🏠  Départ  —  8h10  (50 min avant le boulot)",
            {
                "Michel, le ventre creux mais l'esprit clair, sort de chez lui.",
                "Il espère que quelqu'un aura apporté des croissants au bureau.",
                "",
                "Comment va-t-il au boulot ?",
            },
            {
                {"🚲  À vélo — sur un estomac vide...",    -1,  +5, -20, +10},
                {"🚌  En bus — au moins il peut souffler", -1,  +5,   0,  +5},
                {"🚶  À pied — lent et affamé",            -1,   0, -15, -10},
            }
        }},

        // 5 : Transport (levé en retard, en speed)
        {5, {
            "🏠  Départ  —  8h25  (35 min avant le boulot — en mode fusée)",
            {
                "Michel avale son café ou ses céréales en catastrophe.",
                "Il attrape son sac et file.",
                "",
                "Comment y aller vite ?",
            },
            {
                {"🚲  À vélo — c'est le plus rapide !",        -1, +10, -10, +15},
                {"🚌  En bus — s'il est à l'heure...",          -1,   0,  +5, -10},
                {"🚕  Taxi — ça coûte mais ça règle le souci", -1,  -5, +10, +20},
            }
        }},
    };
}

// ─── Calcul de la fin ────────────────────────────────────────────────────────

Ending compute_ending(const State& s) {
    if (s.ponctualite < 20) {
        return {
            "💀", "FIN : La catastrophe absolue",
            {
                "Michel ne se lèvera pas ce matin.",
                "Son chef a appelé. Deux fois. Puis trois fois.",
                "Michel dort encore, quelque part sous sa couette.",
                "C'est le genre de matinée dont on ne se remet pas.",
            }
        };
    }
    if (s.ponctualite >= 70 && s.energie >= 65 && s.humeur >= 65) {
        return {
            "🌟", "FIN PARFAITE : Une matinée au top",
            {
                "Michel arrive au bureau le sourire aux lèvres.",
                "Bien nourri, à l'heure, plein d'énergie.",
                "Ses collègues se demandent ce qui lui arrive.",
                "Il est... heureux. Vraiment heureux.",
            }
        };
    }
    if (s.ponctualite >= 55 && s.energie >= 50) {
        return {
            "✅", "BONNE FIN : Dans les temps",
            {
                "Michel arrive au bureau à peu près à l'heure,",
                "le ventre raisonnablement plein.",
                "La journée s'annonce correcte.",
                "Pas extraordinaire, mais correcte.",
            }
        };
    }
    if (s.ponctualite >= 55 && s.energie < 50) {
        return {
            "😐", "FIN MOYENNE : À l'heure mais le ventre vide",
            {
                "Michel arrive à l'heure — ouf.",
                "Mais le ventre creux.",
                "Il fixe l'horloge en attendant la pause déjeuner.",
                "Il reste 4 heures. Ça va être long.",
            }
        };
    }
    if (s.ponctualite < 55 && s.energie >= 50) {
        return {
            "😬", "FIN MOYENNE : En retard mais bien nourri",
            {
                "Michel arrive avec 10 minutes de retard.",
                "Son chef lève un sourcil.",
                "Michel sourit. Au moins, il a bien mangé.",
                "C'est déjà ça.",
            }
        };
    }
    return {
        "😢", "MAUVAISE FIN : La double peine",
        {
            "Michel arrive en retard ET le ventre vide.",
            "Son chef l'attend. Ses collègues le regardent.",
            "Il n'a même pas de café pour se consoler.",
            "C'est une mauvaise matinée.",
            "Mais demain sera peut-être meilleur.",
        }
    };
}

// ─── Panneau de statistiques ─────────────────────────────────────────────────

Element stat_bar(const std::string& label, int val, Color c) {
    std::string s = std::to_string(val);
    while (s.size() < 3) s = " " + s;
    return hbox({
        text(label) | size(WIDTH, EQUAL, 17),
        gauge(val / 100.0f) | color(c) | flex,
        text(" " + s + "%") | size(WIDTH, EQUAL, 6),
    });
}

Element stats_panel(const State& s) {
    return vbox({
        text("  Michel  ") | bold | center,
        separator(),
        stat_bar(" Humeur      ", s.humeur,      Color::Yellow),
        stat_bar(" Énergie     ", s.energie,     Color::Green),
        stat_bar(" Ponctualité ", s.ponctualite, Color::Cyan),
    }) | border | size(WIDTH, EQUAL, 36);
}

// ─── Afficher une scène interactive ──────────────────────────────────────────
// Retourne l'index du choix sélectionné, ou -1 (quitter / abandonner).

int run_scene(ScreenInteractive& screen, const Scene& scene, const State& state) {
    std::vector<std::string> labels;
    for (const auto& c : scene.choices) labels.push_back(c.label);

    int  selected  = 0;
    bool confirmed = false;
    bool quit      = false;

    auto menu   = Menu(&labels, &selected);
    auto caught = CatchEvent(menu, [&](Event ev) -> bool {
        if (ev == Event::Return) {
            confirmed = true;
            screen.ExitLoopClosure()();
            return true;
        }
        if (ev == Event::Character('q') || ev == Event::Character('Q')) {
            quit = true;
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    auto renderer = Renderer(caught, [&] {
        Elements left;
        left.push_back(text("  " + scene.title + "  ") | bold | center);
        left.push_back(separator());
        for (const auto& line : scene.desc) {
            if (line.empty())
                left.push_back(text(""));
            else
                left.push_back(paragraph("  " + line));
        }
        left.push_back(separator());
        left.push_back(caught->Render());
        left.push_back(separator());
        left.push_back(
            text("[ haut/bas ] Naviguer   [ Entree ] Choisir   [ q ] Quitter") |
            dim | center);

        return hbox({
            vbox(std::move(left)) | border | flex,
            stats_panel(state),
        });
    });

    screen.Loop(renderer);

    if (quit || !confirmed) return -1;
    return selected;
}

// ─── Afficher la fin ─────────────────────────────────────────────────────────

void show_ending(const Ending& ending, const State& s) {
    Elements desc;
    for (const auto& l : ending.lines)
        desc.push_back(paragraph("  " + l));

    auto doc = vbox({
        text("  " + ending.icon + "  " + ending.title + "  " + ending.icon) |
            bold | center,
        separator(),
        vbox(std::move(desc)),
        separator(),
        text("  Stats finales de Michel :") | bold,
        separator(),
        hbox({text("  Humeur      "), gauge(s.humeur      / 100.0f) | flex,
              text(" " + std::to_string(s.humeur)      + "%")}),
        hbox({text("  Énergie     "), gauge(s.energie     / 100.0f) | flex,
              text(" " + std::to_string(s.energie)     + "%")}),
        hbox({text("  Ponctualité "), gauge(s.ponctualite / 100.0f) | flex,
              text(" " + std::to_string(s.ponctualite) + "%")}),
        separator(),
        text("  — Fin du Niveau 1 : La vie de Michel —  ") | dim | center,
    }) | border;

    auto final_screen = Screen::Create(Dimension::Full(), Dimension::Fit(doc));
    Render(final_screen, doc);
    final_screen.Print();
    std::cout << "\n";
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main() {
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
    }

    return 0;
}
