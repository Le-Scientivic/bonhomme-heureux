#include "input.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

ModuleDeCommande::ModuleDeCommande(const std::string& left_text, const std::string& right_text)
    : left_text_(left_text), right_text_(right_text), input_text_(""), placeholder_("") {}

Component ModuleDeCommande::génère_élément_ftxui() {
    // Créer un composant Input pour capturer la saisie utilisateur
    auto input_comp = Input(&input_text_, placeholder_);

    // Composer avec un Renderer qui ajoute le texte gauche et droit
    return Renderer(input_comp, [this, input_comp]() {
        return hbox({
            text(left_text_),
            input_comp->Render() | flex,
            text(right_text_),
        });
    });
}

std::string& ModuleDeCommande::get_input() {
    return input_text_;
}

void ModuleDeCommande::set_left_text(const std::string& text) {
    left_text_ = text;
}

void ModuleDeCommande::set_right_text(const std::string& text) {
    right_text_ = text;
}

void ModuleDeCommande::set_placeholder(const std::string& text) {
    placeholder_ = text;
}

std::string ModuleDeCommande::get_left_text() const {
    return left_text_;
}

std::string ModuleDeCommande::get_right_text() const {
    return right_text_;
}