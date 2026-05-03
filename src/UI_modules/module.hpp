#pragma once

#include <string>

#include <ftxui/component/component.hpp>

class Module {
public:
    Module();
    virtual ~Module() = default;

    virtual ftxui::Element Render() const = 0;
    
    /// Affiche le module lors du rendu
    void afficher();
    
    /// Cache le module lors du rendu
    void cacher();
    
    /// Retourne l'état de visibilité du module
    bool estVisible() const;
    
protected:
    static ftxui::Element encadrer_avec_titre(const std::string& titre, ftxui::Element contenu);

    bool est_visible_ = true; ///< État de visibilité du module
};