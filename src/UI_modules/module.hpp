#pragma once

#include <ftxui/component/component.hpp>

class Module : public ftxui::Element {
public:
    Module();
    
    /// Affiche le module lors du rendu
    void afficher();
    
    /// Cache le module lors du rendu
    void cacher();
    
    /// Retourne l'état de visibilité du module
    bool estVisible() const;
    
protected:
    bool est_visible_ = true; ///< État de visibilité du module
};