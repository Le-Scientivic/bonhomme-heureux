#pragma once

#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include "module.hpp"

class ModuleDeCommande : public Module {
public:
    ModuleDeCommande();

    void definir_choix(const std::string& titre, const std::vector<std::string>& choix, int selection);
    void valider_choix(const std::string& choix);
    void annuler_choix();
    void effacer_choix();

    ftxui::Element Render() const override;

private:
    std::string _titre_choix;
    std::vector<std::string> _choix_courants;
    int _selection_courante = -1;
    std::vector<std::string> _journal;
};