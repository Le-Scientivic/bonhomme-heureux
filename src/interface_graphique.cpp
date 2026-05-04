#include "interface_graphique.hpp"
#include <ftxui/dom/elements.hpp>
#include <fstream>

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

void InterfaceGraphique::afficher_module(
    std::shared_ptr<Module> module,
    const std::string& titre_phase,
    const std::string& sous_titre,
    const std::vector<std::string>& description_lignes,
    std::function<bool(Event)> gestionnaire_evenement
) {
  if (!module) return;

  // Construire le composant interactif du module (les modules peuvent
  // surcharger MakeComponent pour exposer des widgets interactifs).
  auto module_component = module->MakeComponent(*this);

  // Envelopper le rendu du module (component->Render()) dans un
  // Renderer qui ajoute l'en-tete/encadrement via RenderAvecEnTete.
  auto wrapped = Renderer(module_component, [&] {
    return RenderAvecEnTete(titre_phase, sous_titre, description_lignes, module_component->Render());
  });

  // Deleguer l'affichage au chemin habituel (gestionnaire_evenement optionnel).
  std::ofstream lg_mod("/tmp/bonhomme-debug.log", std::ios::app);
  lg_mod << "[InterfaceGraphique] afficher_module -> afficher_composant\n";
  lg_mod.close();

  afficher_composant(wrapped, gestionnaire_evenement);
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

void InterfaceGraphique::afficher_page(
    const std::string& titre,
    const std::string& sous_titre,
    const std::vector<std::string>& description_lignes,
    const Element& corps,
    std::function<bool(Event)> gestionnaire_evenement
) {
  auto page = Renderer([&] {
    return RenderAvecEnTete(titre, sous_titre, description_lignes, corps);
  });

  std::ofstream lg_page("/tmp/bonhomme-debug.log", std::ios::app);
  lg_page << "[InterfaceGraphique] afficher_page -> afficher_composant\n";
  lg_page.close();

  afficher_composant(page, gestionnaire_evenement);
}

void InterfaceGraphique::definir_entete(
    const std::string& titre,
    const std::string& sous_titre,
    const std::vector<std::string>& description_lignes
) {
  _entete_titre = titre;
  _entete_sous_titre = sous_titre;
  _entete_description = description_lignes;
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

void InterfaceGraphique::afficher_rendu(std::function<bool(Event)> gestionnaire_evenement) {

  // Fallback to element-based render to ensure content is visible.
  auto rendu = Renderer([&] {
    bool aucun_visible = true;
    if (_module_automatisation && _module_automatisation->estVisible()) aucun_visible = false;
    if (_module_commande && _module_commande->estVisible()) aucun_visible = false;
    if (_module_discussion && _module_discussion->estVisible()) aucun_visible = false;

    if (aucun_visible) {
      return text("Aucun module d'affiche: appelez la fonction afficher d'un module pour mettre estVisible a True");
    }

    return RenderAvecEnTete(_entete_titre, _entete_sous_titre, _entete_description, Render());
  });

  afficher_composant(rendu, gestionnaire_evenement);
}