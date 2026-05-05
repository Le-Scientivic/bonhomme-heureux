#include "module_de_discussion.hpp"
#include "../interface_graphique.hpp"

#include <algorithm>

using namespace ftxui;

ModuleDeDiscussion::ModuleDeDiscussion() = default;

void ModuleDeDiscussion::ajoute_mon_message(const std::string& mon_nom, const std::string& mon_message) {
    _messages.push_back(Message{mon_nom, mon_message, true});
    _scroll_offset = 0;
}

void ModuleDeDiscussion::ajoute_son_message(const std::string& son_nom, const std::string& son_message) {
    _messages.push_back(Message{son_nom, son_message, false});
    _scroll_offset = 0;
}

void ModuleDeDiscussion::réinitialiser_discussion() {
    _messages.clear();
    _scroll_offset = 0;
}

Element ModuleDeDiscussion::Render() const {
    // Construire l'affichage statique (utilisé par le layout global)
    std::vector<Element> lignes;
    lignes.reserve(_messages.size());

    for (const auto& message : _messages) {
        auto contenu = vbox({
            text(message.nom) | bold,
            paragraph(message.texte),
        }) | border | size(WIDTH, LESS_THAN, 40);

        if (message.est_a_moi) {
            lignes.push_back(hbox({filler(), contenu}));
        } else {
            lignes.push_back(hbox({contenu, filler()}));
        }
    }

    if (lignes.empty()) {
        lignes.push_back(paragraph("Aucun message pour le moment"));
    }

    float scroll_ratio = 1.0f;
    if (_scroll_offset > 0 && _messages.size() > 1) {
        scroll_ratio = 1.0f - (float)_scroll_offset / (float)(_messages.size() - 1);
    }

    return encadrer_avec_titre("Discussion",
        vbox(std::move(lignes))
        | focusPositionRelative(0.5f, scroll_ratio)
        | vscroll_indicator
        | frame
        | yflex
    ) | color(Color::BlueLight);
}

Component ModuleDeDiscussion::MakeComponent(InterfaceGraphique& interface_graphique) {
    (void)interface_graphique;
    auto renderer = Renderer([this] { return Render(); });
    return CatchEvent(renderer, [this](Event event) -> bool {
        if (!event.is_mouse()) return false;
        if (event.mouse().button == Mouse::WheelUp) {
            int max_offset = std::max(0, (int)_messages.size() - 1);
            _scroll_offset = std::min(_scroll_offset + 1, max_offset);
            return true;
        }
        if (event.mouse().button == Mouse::WheelDown) {
            _scroll_offset = std::max(0, _scroll_offset - 1);
            return true;
        }
        return false;
    });
}