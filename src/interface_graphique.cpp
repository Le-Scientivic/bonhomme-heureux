#include "interface_graphique.hpp"
#include <ftxui/dom/elements.hpp>

#include "UI_modules/module_de_commande.hpp"
#include "UI_modules/module_de_discussion.hpp"
#include "UI_modules/module_automatisation.hpp"

using namespace ftxui;

void InterfaceGraphique::ajouter_module(std::shared_ptr<Module> module) {
  _liste_de_modules.push_back(module);
}

Element InterfaceGraphique::Render() const {
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

  return vbox({
    top_row,
    filler(),
    bottom_row,
  });
}

Element InterfaceGraphique::RenderAvecEnTete(const std::string& titre, const std::string& sous_titre, const std::string& description, const Element& corps) const {
  std::vector<Element> lignes;

  lignes.push_back(text(titre) | bold | center);

  if (!sous_titre.empty()) {
    lignes.push_back(text(sous_titre) | dim | center);
  }

  if (!description.empty()) {
    lignes.push_back(separator());
    lignes.push_back(paragraph(description) | center);
  }

  lignes.push_back(separator());
  lignes.push_back(corps);

  return vbox(std::move(lignes)) | border;
}
    
void InterfaceGraphique::afficher_rendu() {
  auto rendu = Renderer([&] {
    if (std::none_of(_liste_de_modules.begin(), _liste_de_modules.end(), [](const std::shared_ptr<Module>& module) {
          return module && module->estVisible();
        })) {
      return text("Aucun module d'affiché: appelez la fonction afficher d'un module pour mettre estVisible à True");
    }

    return Render();
  });
  _screen.Loop(rendu);
}