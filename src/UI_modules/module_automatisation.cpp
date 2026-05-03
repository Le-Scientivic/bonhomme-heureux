#include "module_automatisation.hpp"

using namespace ftxui;

ModuleDeAutomatisation::ModuleDeAutomatisation() = default;

Element ModuleDeAutomatisation::Render() const {
    return encadrer_avec_titre("Automatisation", text("Aucun contenu"));
}