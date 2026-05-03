#include "interface_graphique.hpp"
#include <ftxui/dom/elements.hpp>

#include "UI_modules/module_de_commande.hpp"
#include "UI_modules/module_de_discussion.hpp"
#include "UI_modules/module_automatisation.hpp"

using namespace ftxui;

void InterfaceGraphique::ajouter_module(std::shared_ptr<Module> module) {
  _liste_de_modules.push_back(module);
}
    
void InterfaceGraphique::afficher_rendu() {
  std::shared_ptr<ModuleDeDiscussion> module_de_discussion;
  std::shared_ptr<ModuleDeAutomatisation> module_d_automatisation;
  std::shared_ptr<ModuleDeCommande> module_de_commande;

  for (const auto& module : _liste_de_modules) {
    if (!module || !module->estVisible()) {
      continue;
    }

    if (!module_de_discussion) {
      module_de_discussion = std::dynamic_pointer_cast<ModuleDeDiscussion>(module);
    }

    if (!module_d_automatisation) {
      module_d_automatisation = std::dynamic_pointer_cast<ModuleDeAutomatisation>(module);
    }

    if (!module_de_commande) {
      module_de_commande = std::dynamic_pointer_cast<ModuleDeCommande>(module);
    }
  }

  auto render_slot = [](const std::shared_ptr<Module>& module) -> Element {
    if (!module || !module->estVisible()) {
      return filler();
    }

    return module->Render();
  };

  auto render_left_slot = [&](const std::shared_ptr<Module>& module) -> Element {
    if (!module || !module->estVisible()) {
      return filler() | flex;
    }

    return module->Render() | flex;
  };

  auto render_right_slot = [&](const std::shared_ptr<Module>& module) -> Element {
    if (!module || !module->estVisible()) {
      return filler() | flex;
    }

    return hbox({filler(), module->Render() | flex}) | flex;
  };

  auto top_row = hbox({
    render_left_slot(module_d_automatisation),
    render_right_slot(module_de_discussion),
  });

  auto bottom_row = render_slot(module_de_commande);

  auto rendu = Renderer([&] {
    if (!module_de_discussion && !module_d_automatisation && !module_de_commande) {
      return text("Aucun module d'affiché: appelez la fonction afficher d'un module pour mettre estVisible à True");
    }

    return vbox({
      top_row,
      filler(),
      bottom_row,
    });
  });
  _screen.Loop(rendu);
}