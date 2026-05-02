#include "input.hpp"

using namespace ftxui;

ModuleDeCommande::ModuleDeCommande(Element contenu) {
    _contenu = contenu;
}

void ModuleDeCommande::changer_contenu(ftxui::Element nouveau_contenu) {
    _contenu = nouveau_contenu;
}