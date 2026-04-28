# bonhomme-heureux
Le bonhomme sera-t-il heureux ? Automatise sa vie et nous verrons.

## Librairie
FTXUI (Fast Terminal User Interface) - v5.0.0

## Build

```bash
cmake -S . -B build
cmake --build build
./build/bonhomme-heureux
```

Navigation: ↑/↓ to move, Enter to confirm, q to quit.

---

## Architecture modulaire de l'interface

Le projet utilise une architecture modulaire qui permet d'activer/désactiver différents modules de l'interface utilisateur selon les besoins.

### Concept

Chaque module UI est un composant optionnel et indépendant qui peut être activé ou désactivé dynamiquement. L'état d'activation des modules est géré via un **bitset** pour une gestion efficace et concise.

### Structure des modules

```
src/
├── interface_graphique.hpp    # Enum et bitset centralisé
├── interface_graphique.cpp    # Fonction d'affichage et composition
└── UI_modules/
    ├── input.hpp              # Module de saisie utilisateur
    ├── input.cpp
    ├── chat.hpp               # (Planifié) Module chat
    ├── automations.hpp        # (Planifié) Module automations
    └── video.hpp              # (Planifié) Module vidéo
```

### Énumération des modules

Chaque module est représenté par un index dans l'énumération `ModuleIndex` (fichier [interface_graphique.hpp](src/interface_graphique.hpp)) :

```cpp
enum ModuleIndex {
    INPUT,           // Module de saisie utilisateur
    CHAT,            // (Futur) Chat/conversation
    AUTOMATIONS,     // (Futur) Automations
    VIDEO,           // (Futur) Lecteur vidéo
    MODULE_BIT_COUNT // Nombre total de modules
};

std::bitset<MODULE_BIT_COUNT> uiModules;  // État d'activation des modules
```

### Activation/Désactivation

Les modules sont activés et désactivés dans la fonction `init_modules()` :

```cpp
void init_modules() {
    uiModules.set(INPUT);               // Activer INPUT
    uiModules.reset(CHAT);              // Désactiver CHAT
    uiModules.reset(AUTOMATIONS);       // Désactiver AUTOMATIONS
    uiModules.reset(VIDEO);             // Désactiver VIDEO
}
```

La fonction `display_modules()` affiche l'interface avec les modules activés :

```cpp
void display_modules() {
    bool isInputEnabled = uiModules.test(INPUT);
    // ... composition de l'interface avec les modules activés
}
```

---

## Module INPUT

Le module `INPUT` fournit une zone de saisie interactive pour capturer du texte de l'utilisateur.

### Caractéristiques

- **Texte configurable** : Affichage de texte à gauche et à droite de la zone de saisie
- **Placeholder** : Message d'aide affiché quand le champ est vide
- **Composant ftxui** : Basé sur le composant `ftxui::Input` pour une intégration fluide
- **Flexibilité** : Permet une configuration dynamique à tout moment

### Classe `InputModule`

Fichier : [src/UI_modules/input.hpp](src/UI_modules/input.hpp)

#### Constructeur

```cpp
InputModule(const std::string& left_text = "", const std::string& right_text = "");
```

**Paramètres :**
- `left_text` : Texte à afficher à gauche de la zone de saisie (ex: "Prompt: ")
- `right_text` : Texte à afficher à droite (ex: " > ")

#### Méthodes principales

| Méthode | Description |
|---------|-------------|
| `Component render()` | Retourne le composant ftxui rendu pour l'intégration |
| `std::string& get_input()` | Récupère le texte saisi par l'utilisateur |
| `void set_left_text(const std::string&)` | Modifie le texte à gauche |
| `void set_right_text(const std::string&)` | Modifie le texte à droite |
| `void set_placeholder(const std::string&)` | Modifie le placeholder |
| `std::string get_left_text() const` | Récupère le texte à gauche |
| `std::string get_right_text() const` | Récupère le texte à droite |

### Exemple d'utilisation

```cpp
#include "UI_modules/input.hpp"

// Créer une instance du module input
InputModule input_module("Vous: ", " > ");
input_module.set_placeholder("Tapez votre message...");

// Récupérer le composant rendu
auto input_component = input_module.render();

// Intégrer dans l'interface (ex: vbox layout)
auto interface = vbox({
    text("Ma superbe interface"),
    separator(),
    input_component,
});

// Après validation utilisateur
std::string user_text = input_module.get_input();
std::cout << "L'utilisateur a écrit: " << user_text << std::endl;
```

### Exemple de rendu

```
┌──────────────────────────────────────────┐
│ Bienvenue dans l'interface modulaire      │
├──────────────────────────────────────────┤
│ Module INPUT activé                      │
├──────────────────────────────────────────┤
│ Vous: [ texte saisi par l'utilisateur ] >│
└──────────────────────────────────────────┘
```

---

## Ajouter un nouveau module

Pour créer un nouveau module UI (ex: `CHAT`) :

1. **Créer les fichiers** : `src/UI_modules/chat.hpp` et `src/UI_modules/chat.cpp`

2. **Définir la classe** : Implémenter une classe `ChatModule` avec au minimum la méthode `render()` qui retourne un `ftxui::Component`

3. **Ajouter l'index** : Ajouter une entrée dans l'énumération `ModuleIndex` (fichier [interface_graphique.hpp](src/interface_graphique.hpp))

4. **Initialiser** : Ajouter la logique d'activation/désactivation dans `init_modules()`

5. **Intégrer** : Ajouter le composant dans `display_modules()` pour l'afficher dans l'interface

6. **Mettre à jour CMake** : Ajouter le fichier `.cpp` à `add_executable()` dans [CMakeLists.txt](CMakeLists.txt)
