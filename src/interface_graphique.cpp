#include "interface_graphique.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

InterfaceGraphique::InterfaceGraphique(
    std::shared_ptr<ModuleDeAutomatisation> module_automatisation,
    std::shared_ptr<ModuleDeCommande> module_commande,
    std::shared_ptr<ModuleDeDiscussion> module_discussion
)
    : _module_automatisation(module_automatisation),
      _module_commande(module_commande),
      _module_discussion(module_discussion) {}

std::shared_ptr<ModuleDeAutomatisation> InterfaceGraphique::get_module_automatisation() const {
  return _module_automatisation;
}

std::shared_ptr<ModuleDeCommande> InterfaceGraphique::get_module_commande() const {
  return _module_commande;
}

std::shared_ptr<ModuleDeDiscussion> InterfaceGraphique::get_module_discussion() const {
  return _module_discussion;
}

void InterfaceGraphique::fermer_rendu() {
  _screen.ExitLoopClosure()();
}

void InterfaceGraphique::poster_evenement(const Event& evenement) {
  _screen.PostEvent(evenement);
}

Element InterfaceGraphique::Render() const {
  auto render_slot = [](const std::shared_ptr<Module>& module) -> Element {
    if (!module || !module->estVisible()) {
      return filler();
    }
    return module->Render();
  };

  auto render_left_slot = [](const std::shared_ptr<Module>& module) -> Element {
    if (!module || !module->estVisible()) {
      return filler() | flex;
    }
    return module->Render() | flex;
  };

  auto render_right_slot = [](const std::shared_ptr<Module>& module) -> Element {
    if (!module || !module->estVisible()) {
      return filler() | flex;
    }
    return hbox({filler(), module->Render() | flex}) | flex;
  };

  auto top_row = hbox({
    render_left_slot(_module_automatisation),
    render_right_slot(_module_discussion),
  });

  auto bottom_row = render_slot(_module_commande);

  return vbox({
    top_row,
    filler(),
    bottom_row,
  });
}

Component InterfaceGraphique::construire_racine() {
  auto composant_commande = _module_commande ? _module_commande->MakeComponent(*this) : Renderer([this] {
    return _module_commande ? _module_commande->Render() : filler();
  });

  auto composant_automatisation = _module_automatisation ? _module_automatisation->MakeComponent(*this) : Renderer([this] {
    return _module_automatisation ? _module_automatisation->Render() : filler();
  });

  auto composant_discussion = _module_discussion ? _module_discussion->MakeComponent(*this) : Renderer([this] {
    return _module_discussion ? _module_discussion->Render() : filler();
  });

  auto racine = Container::Vertical({
      composant_commande,
      composant_automatisation,
      composant_discussion,
  });

  return Renderer(racine, [this] {
    return Render();
  });
}

Element InterfaceGraphique::RenderAvecEnTete(
    const std::string& titre,
    const std::string& sous_titre,
    const std::vector<std::string>& description_lignes,
    const Element& corps
) const {
  std::vector<Element> lignes;

  lignes.push_back(text(titre) | bold | center);

  if (!sous_titre.empty()) {
    lignes.push_back(text(sous_titre) | dim | center);
  }

  if (!description_lignes.empty()) {
    std::vector<Element> desc;
    desc.reserve(description_lignes.size());

    for (const auto& ligne : description_lignes) {
      if (ligne.empty()) {
        desc.push_back(text(""));
      } else {
        desc.push_back(paragraph(ligne) | center);
      }
    }

    lignes.push_back(separator());
    lignes.push_back(vbox(std::move(desc)));
  }

  lignes.push_back(separator());
  lignes.push_back(corps);

  return vbox(std::move(lignes)) | border;
}

void InterfaceGraphique::afficher_composant(
    Component composant,
    std::function<bool(Event)> gestionnaire_evenement
) {
  if (!composant) {
    return;
  }

  Component cible = composant;
  if (gestionnaire_evenement) {
    cible = CatchEvent(composant, std::move(gestionnaire_evenement));
  }

  _screen.Loop(cible);
}
