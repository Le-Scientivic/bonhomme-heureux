#include "module.hpp"

#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Module::Module() : est_visible_(true) {}

Element Module::encadrer_avec_titre(const std::string& titre, Element contenu) {
    return vbox({
        text(titre) | bold | center,
        separator(),
        std::move(contenu),
    }) | border;
}

void Module::afficher() {
    est_visible_ = true;
}

void Module::cacher() {
    est_visible_ = false;
}

bool Module::estVisible() const {
    return est_visible_;
}