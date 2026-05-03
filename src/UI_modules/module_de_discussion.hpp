#pragma once

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"

class ModuleDeDiscussion : public Module {
public:
    ModuleDeDiscussion();

    void ajoute_mon_message(const std::string& mon_nom, const std::string& mon_message);
    void ajoute_son_message(const std::string& son_nom, const std::string& son_message);
    void réinitialiser_discussion();

    ftxui::Element Render() const override;

private:
    struct Message {
        std::string nom;
        std::string texte;
        bool est_a_moi = false;
    };

    std::vector<Message> _messages;
};