#include "module_de_commande.hpp"

using namespace ftxui;

ModuleDeCommande::ModuleDeCommande() = default;

void ModuleDeCommande::definir_choix(const std::string& titre, const std::vector<std::string>& choix, int selection) {
    _titre_choix = titre;
    _choix_courants = choix;
    _selection_courante = selection;
}

void ModuleDeCommande::valider_choix(const std::string& choix) {
    _journal.push_back("Choix valide: " + choix);
}

void ModuleDeCommande::annuler_choix() {
    _journal.push_back("Choix annule");
}

void ModuleDeCommande::effacer_choix() {
    _titre_choix.clear();
    _choix_courants.clear();
    _selection_courante = -1;
}

Element ModuleDeCommande::Render() const {
    Elements lignes;

    if (_choix_courants.empty()) {
        lignes.push_back(text("Aucun choix en cours") | dim);
    } else {
        if (_titre_choix.empty()) {
            lignes.push_back(text("Choisissez une action") | bold);
        } else {
            lignes.push_back(text(_titre_choix) | bold);
        }
        lignes.push_back(separator());

        for (int i = 0; i < (int)_choix_courants.size(); ++i) {
            const bool est_selection = i == _selection_courante;
            const std::string prefixe = est_selection ? "> " : "  ";
            lignes.push_back(text(prefixe + _choix_courants[i]));
        }
    }

    if (!_journal.empty()) {
        lignes.push_back(separator());
        lignes.push_back(text("Historique") | bold);

        const int debut = std::max(0, (int)_journal.size() - 5);
        for (int i = debut; i < (int)_journal.size(); ++i) {
            lignes.push_back(text("- " + _journal[i]) | dim);
        }
    }

    return encadrer_avec_titre("Commande", vbox(std::move(lignes)));
}