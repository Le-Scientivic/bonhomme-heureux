#include "module_automatisation.hpp"

#include <unordered_set>

using namespace ftxui;

ModuleDeAutomatisation::ModuleDeAutomatisation() = default;

void ModuleDeAutomatisation::preparer_menu_depuis_unlocked(const std::vector<std::string>& unlocked_automs, const GameMeta& meta) {
    _automs_courantes.clear();
    _labels_courants.clear();

    std::unordered_set<std::string> ids_vus;
    ids_vus.reserve(unlocked_automs.size());

    for (const auto& autom_ref : unlocked_automs) {
        if (!ids_vus.insert(autom_ref).second) {
            continue;
        }

        const Autom autom = load_autom(autom_ref, meta);

        GameChoice choix;
        choix.id = autom_ref;
        choix.label = autom.label.empty() ? autom_ref : autom.label;
        _automs_courantes.push_back(std::move(choix));
        _labels_courants.push_back(_automs_courantes.back().label);
    }

    _selection_courante = _automs_courantes.empty() ? -1 : 0;
}

void ModuleDeAutomatisation::reinitialiser_menu() {
    _automs_courantes.clear();
    _labels_courants.clear();
    _selection_courante = -1;
}

bool ModuleDeAutomatisation::est_menu_vide() const {
    return _automs_courantes.empty();
}

bool ModuleDeAutomatisation::deplacer_selection_haut() {
    if (_automs_courantes.empty()) {
        return false;
    }

    if (_selection_courante > 0) {
        --_selection_courante;
        return true;
    }

    return false;
}

bool ModuleDeAutomatisation::deplacer_selection_bas() {
    if (_automs_courantes.empty()) {
        return false;
    }

    if (_selection_courante < (int)_automs_courantes.size() - 1) {
        ++_selection_courante;
        return true;
    }

    return false;
}

int ModuleDeAutomatisation::index_selection() const {
    return _selection_courante;
}

std::string ModuleDeAutomatisation::id_selection() const {
    if (_selection_courante < 0 || _selection_courante >= (int)_automs_courantes.size()) {
        return "";
    }

    return _automs_courantes[_selection_courante].id;
}

std::string ModuleDeAutomatisation::label_selection() const {
    if (_selection_courante < 0 || _selection_courante >= (int)_automs_courantes.size()) {
        return "";
    }

    return _automs_courantes[_selection_courante].label;
}

std::vector<std::string> ModuleDeAutomatisation::labels_courants() const {
    return _labels_courants;
}

Element ModuleDeAutomatisation::_render_frame(Element corps) const {
    return encadrer_avec_titre("Automatisation", vbox({
        text("Automatisations activées") | bold,
        separator(),
        std::move(corps),
        separator(),
        text("[ haut/bas ] Naviguer") | dim,
        text("[ Entree ] Activer") | dim,
        text("[ q/Echap ] Annuler") | dim,
    })) | color(Color::RGB(255, 180, 0));
}

Element ModuleDeAutomatisation::Render() const {
    if (_automs_courantes.empty()) {
        return encadrer_avec_titre("Automatisation", vbox({
            text("Aucune automation debloquee.") | color(Color::RedLight),
            separator(),
            text("Impossible de traiter cet evenement sans automation active.") | dim,
            text("[ Entree ] Continuer") | dim,
        })) | color(Color::RGB(255, 180, 0));
    }

    Elements items;
    for (int i = 0; i < (int)_automs_courantes.size(); ++i) {
        const bool selectionne = i == _selection_courante;
        const std::string prefixe = selectionne ? "> " : "  ";
        Element ligne = text(prefixe + _automs_courantes[i].label);
        if (selectionne) {
            ligne = ligne | color(Color::Cyan) | bold;
        }
        items.push_back(std::move(ligne));
    }
    return _render_frame(vbox(std::move(items)));
}

Component ModuleDeAutomatisation::MakeComponent(InterfaceGraphique& interface_graphique) {
    (void)interface_graphique;

    if (_labels_courants.empty()) {
        return Renderer([this] { return Render(); });
    }

    auto menu = Menu(&_labels_courants, &_selection_courante);
    return Renderer(menu, [this, menu] {
        return _render_frame(menu->Render());
    });
}