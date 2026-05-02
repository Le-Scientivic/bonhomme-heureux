#include "interface_graphique.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

void InterfaceGraphique::ajouter_module(Module module) {
  _liste_de_modules.push_back(module);
}
    
void InterfaceGraphique::afficher_rendu() {
  std::vector<Element> éléments_visibles;
  for (const auto& module : _liste_de_modules) {
    if (module.estVisible()) {
      éléments_visibles.push_back(module);
    }
  }

  auto rendu = Renderer([&] {
    if (éléments_visibles.empty()) {
      return text("Aucun module d'affiché: appelez la fonction afficher d'un module pour mettre estVisible à True");
    }
    return vbox(éléments_visibles);
  });
  _screen.Loop(rendu);
}