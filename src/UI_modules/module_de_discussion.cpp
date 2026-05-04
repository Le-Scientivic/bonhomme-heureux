#include "module_de_discussion.hpp"

using namespace ftxui;

ModuleDeDiscussion::ModuleDeDiscussion() = default;

void ModuleDeDiscussion::ajoute_mon_message(const std::string& mon_nom, const std::string& mon_message) {
    _messages.push_back(Message{mon_nom, mon_message, true});
}

void ModuleDeDiscussion::ajoute_son_message(const std::string& son_nom, const std::string& son_message) {
    _messages.push_back(Message{son_nom, son_message, false});
}

void ModuleDeDiscussion::réinitialiser_discussion() {
    _messages.clear();
}

Element ModuleDeDiscussion::RenderMessages() const {
    std::vector<Element> lignes;
    lignes.reserve(_messages.size());

    for (const auto& message : _messages) {
        auto contenu = vbox({
            text(message.nom) | bold,
            paragraph(message.texte),
        }) | border | size(WIDTH, LESS_THAN, 60);

        if (message.est_a_moi) {
            lignes.push_back(hbox({filler(), contenu}));
        } else {
            lignes.push_back(hbox({contenu, filler()}));
        }
    }

    if (lignes.empty()) {
        lignes.push_back(paragraph("Aucun message pour le moment"));
    }

    return vbox(std::move(lignes));
}

Element ModuleDeDiscussion::Render() const {
    auto corps = vbox({
        RenderMessages(),
        separator(),
        text(_slider_label + ": " + std::to_string(_slider_value)) | dim,
    });

    return encadrer_avec_titre("Discussion", std::move(corps));
}

Component ModuleDeDiscussion::MakeComponent(InterfaceGraphique& interface_graphique) {
    (void)interface_graphique;

    auto slider = Slider(&_slider_label, &_slider_value, _slider_min, _slider_max, 1);

    return Renderer(slider, [&] {
        auto corps = vbox({
            RenderMessages(),
            separator(),
            slider->Render(),
        });

        return encadrer_avec_titre("Discussion", std::move(corps));
    });
}