#pragma once

#include <string>

#include <ftxui/component/component.hpp>

// Forward declaration to allow modules to receive a pointer to the
// InterfaceGraphique when building interactive components.
class InterfaceGraphique;

/// Classe de base de tous les modules UI du jeu.
///
/// Le contrat commun expose la notion de visibilite et force chaque module a
/// produire un element FTXUI via Render(). Les classes derives reutilisent
/// ensuite les helpers partages pour garder un encadrement visuel homogene.
class Module {
public:
    /// Construit un module visible par defaut.
    Module();
    /// Destructeur virtuel pour permettre la destruction polymorphique.
    virtual ~Module() = default;

    /// Produit le rendu FTXUI specifique au module.
    virtual ftxui::Element Render() const = 0;

    /**
     * @brief Construit un composant FTXUI interactif pour le module.
     *
     * Par defaut, cette methode enveloppe l'Element retourne par
     * `Render()` dans un `Renderer` simple. Les modules interactifs
     * peuvent surcharger `MakeComponent` pour fournir des widgets
     * avec gestion d'evenements interne (ex: `Menu`, `Input`, etc.).
     */
    virtual ftxui::Component MakeComponent(InterfaceGraphique& interface_graphique) {
        (void)interface_graphique;
        return ftxui::Renderer([this] { return this->Render(); });
    }
    
    /// Rend le module visible lors des prochains passages de rendu.
    void afficher();
    
    /// Cache le module lors des prochains passages de rendu.
    void cacher();
    
    /// Indique si le module doit actuellement etre rendu.
    bool estVisible() const;
    
protected:
    /// Encadre un contenu FTXUI avec un titre partage par les modules derives.
    static ftxui::Element encadrer_avec_titre(const std::string& titre, ftxui::Element contenu);

    /// Etat de visibilite courant du module.
    bool est_visible_ = true; ///< État de visibilité du module
};