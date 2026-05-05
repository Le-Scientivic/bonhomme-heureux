#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "UI_modules/module_de_commande.hpp"
#include "UI_modules/module_de_discussion.hpp"
#include "UI_modules/module_automatisation.hpp"

/**
 * @brief Orchestrateur de rendu FTXUI regroupant les differentes parties de l'UI.
 *
 * Cette classe centralise la composition des modules et lance la boucle
 * d'affichage interactive via FTXUI. Les modules sont possedes directement.
 */
class InterfaceGraphique {
public:
    /**
     * @brief Construit l'orchestrateur avec les trois modules principaux.
     * @param module_automatisation module d'automatisations
     * @param module_commande module de commande/choix
     * @param module_discussion module de discussion
     */
    InterfaceGraphique(
        std::shared_ptr<ModuleDeAutomatisation> module_automatisation,
        std::shared_ptr<ModuleDeCommande> module_commande,
        std::shared_ptr<ModuleDeDiscussion> module_discussion
    );

    /**
     * @brief Accès au module d'automatisation.
     */
    std::shared_ptr<ModuleDeAutomatisation> get_module_automatisation() const;

    /**
     * @brief Accès au module de commande.
     */
    std::shared_ptr<ModuleDeCommande> get_module_commande() const;

    /**
     * @brief Accès au module de discussion.
     */
    std::shared_ptr<ModuleDeDiscussion> get_module_discussion() const;

    /**
     * @brief Compose l'interface complete a partir des modules visibles.
     * @return Element FTXUI representant l'interface complete
     */
    ftxui::Element Render() const;

    /**
     * @brief Construit le composant racine qui garde tous les modules sur la meme page.
     */
    ftxui::Component construire_racine();

    /**
     * @brief Construit une vue encadree avec titre, sous-titre, description et contenu.
     * @param titre Titre principal de la vue
     * @param sous_titre Sous-titre optionnel
     * @param description_lignes Description multi-lignes visible sous le titre
     * @param corps Contenu a afficher dans le corps du cadre
     * @return Element FTXUI compose
     */
    ftxui::Element RenderAvecEnTete(
        const std::string& titre,
        const std::string& sous_titre,
        const std::vector<std::string>& description_lignes,
        const ftxui::Element& corps = ftxui::filler()
    ) const;

    /**
     * @brief Lance la boucle interactive sur un composant FTXUI.
     * @param composant composant a afficher
     * @param gestionnaire_evenement callback optionnel pour traiter les evenements
     */
    void afficher_composant(ftxui::Component composant, std::function<bool(ftxui::Event)> gestionnaire_evenement = {});

    /**
     * @brief Ferme la boucle interactive courante.
     */
    void fermer_rendu();

    /**
     * @brief Poste un evenement dans la boucle interactive courante.
     */
    void poster_evenement(const ftxui::Event& evenement);

private:
    /**
     * @brief Module d'automatisations debloquees.
     */
    std::shared_ptr<ModuleDeAutomatisation> _module_automatisation;

    /**
     * @brief Module de commande et choix.
     */
    std::shared_ptr<ModuleDeCommande> _module_commande;

    /**
     * @brief Module de discussion.
     */
    std::shared_ptr<ModuleDeDiscussion> _module_discussion;

    /**
     * @brief Ecran FTXUI plein ecran utilise pour la boucle de rendu.
     */
    ftxui::ScreenInteractive _screen = ftxui::ScreenInteractive::Fullscreen();
};