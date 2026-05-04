#include "module.hpp"

#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Module::Module() : est_visible_(true) {}

/// Encadre un contenu FTXUI avec un titre partage par les modules derives.
Element Module::encadrer_avec_titre(const std::string& titre, Element contenu) {
    return vbox({
        text(titre) | bold | center,
        separator(),
        std::move(contenu),
    }) | border;
}

/// Rend le module visible lors des prochains passages de rendu.
void Module::afficher() {
    est_visible_ = true;
}

/// Cache le module lors des prochains passages de rendu.
void Module::cacher() {
    est_visible_ = false;
}

/// Indique si le module doit actuellement etre rendu.
bool Module::estVisible() const {
    return est_visible_;
}