#include "module_de_commande.hpp"

using namespace ftxui;

ModuleDeCommande::ModuleDeCommande() = default;

Element ModuleDeCommande::Render() const {
    return encadrer_avec_titre("Commande", text("Aucun contenu"));
}