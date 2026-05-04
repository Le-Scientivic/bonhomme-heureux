#pragma once

#include <string>

#include <ftxui/component/component.hpp>

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