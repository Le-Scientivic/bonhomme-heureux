#pragma once

/*!
 * \file game_engine.hpp
 * \brief Modele de donnees et fonctions de chargement du jeu.
 *
 * Ce fichier porte la vue metier de haut niveau: metadonnees globales,
 * phases, taches, evenements, automations et etat courant de la partie.
 * Les fonctions libres associees transforment les fichiers JSON de l'histoire
 * en structures en memoire pretes pour le rendu et les decisions de jeu.
 */

#include <map>
#include <string>
#include <vector>

/// Reponse possible a un choix du joueur dans une phase.
struct GameChoice {
    /// Identifiant stable du choix.
    std::string id;
    /// Libelle affiche au joueur.
    std::string label;
    /// Variantes de reponse affichees dans la discussion (optionnel).
    std::vector<std::string> response_variants;
    /// Phase de destination si ce choix est valide.
    std::string next_phase;
    /// Tache suivante associee a ce choix, quand applicable.
    std::string next_task;
    /// Section de navigation ou de contexte UI.
    std::string section;
};

/// Variante de reponse proposee par une automation.
struct AutomResponseOption {
    /// Identifiant de la variante.
    std::string id;
    /// Libelle presente au joueur.
    std::string label;
    /// Formulations alternatives pouvant etre tirees au hasard.
    std::vector<std::string> variants;
    /// Resultat metier associe a cette reponse.
    std::string outcome;
};

/// Description complete d'une automation exploitable dans une phase.
struct Autom {
    /// Reference interne de l'automation.
    std::string autom_id;
    /// Nom court affiche dans les interfaces.
    std::string label;
    /// Texte descriptif de l'automation.
    std::string description;
    /// Variantes de formulation possibles pour la demande de l'operateur.
    std::vector<std::string> owner_request_variants;
    /// Reponses possibles cote joueur.
    std::vector<AutomResponseOption> player_response_options;
};

/// Tache narrative ou systeme a accomplir au cours d'une phase.
struct GameTask {
    /// Identifiant de la tache.
    std::string id;
    /// Reference vers l'automation concernee.
    std::string autom_ref;
    /// Sujet ou objectif affiche dans l'UI.
    std::string topic;
    /// Indique si la tache a ete terminee.
    bool completed = false;
    /// Indique si la tache a ete acceptee par le joueur.
    bool accepted = false;
};

/// Evenement declenche dans une phase lorsque certaines conditions sont remplies.
struct GameEvent {
    /// Identifiant de l'evenement.
    std::string id;
    /// Contexte narratif ou declencheur visible dans l'histoire.
    std::string context;
    /// Automation attendue pour traiter correctement l'evenement.
    std::string correct_autom_ref;
    /// Messages cotes proprietaire associes a l'evenement.
    std::vector<std::string> owner_messages;
    /// Reponses correctes cote joueur.
    std::vector<std::string> correct_responses;
};

/// Donnees de fin de partie.
struct Ending {
    /// Identifiant de la fin.
    std::string id;
    /// Intitule affiche pour cette fin.
    std::string label;
    /// Textes de narration alternatifs.
    std::vector<std::string> narration_variants;
    /// Messages finaux du proprietaire.
    std::vector<std::string> owner_final_messages;
    /// Reponses finales du joueur.
    std::vector<std::string> player_final_responses;
};

/// Phase narrative principale du jeu.
struct GamePhase {
    /// Identifiant de phase.
    std::string phase_id;
    /// Titre lisible de la phase.
    std::string label;
    /// Description de contexte de la phase.
    std::string description;

    /// Sequence de boot affichant des messages systeme avant l'entree en jeu.
    struct BootSequence {
        /// Indice ou nom d'animation a utiliser.
        std::string animation_hint;
        /// Messages systeme affiches pendant le boot.
        std::vector<std::string> system_messages;
    };
    /// Donnees de boot de la phase.
    BootSequence boot_sequence;
    /// Indique si la phase possede un boot sequence.
    bool has_boot = false;

    /// Choix proposes au joueur au demarrage de la phase.
    struct PlayerChoiceOnStart {
        /// Texte d'introduction du choix initial.
        std::string description;
        /// Choix disponibles.
        std::vector<GameChoice> options;
    };
    /// Bloc de choix initial.
    PlayerChoiceOnStart player_choice_on_start;
    /// Indique si le bloc de choix initial est actif.
    bool has_player_choice = false;

    /// Taches actives pendant la phase.
    std::vector<GameTask> tasks;
    /// Indique si la phase expose des taches.
    bool has_tasks = false;

    /// Evenements declenchables dans la phase.
    std::vector<GameEvent> events;
    /// Indique si la phase expose des evenements.
    bool has_events = false;

    /// Conditions de sortie de la phase.
    struct PhaseEnd {
        /// Condition textuelle ou logique de sortie.
        std::string condition;
        /// Phase suivante si la condition est satisfaite.
        std::string next_phase;
        /// Messages de transition affiches avant le passage.
        std::vector<std::string> transition_messages;
    };
    /// Bloc de fin de phase.
    PhaseEnd phase_end;
    /// Indique si la phase a une fin definie.
    bool has_phase_end = false;

    /// Choix de navigation ou de branchement disponibles dans la phase.
    std::vector<GameChoice> path_choices;
    /// Indique si la phase propose un choix de chemin.
    bool has_path_choice = false;

    /// Messages systeme ou narratifs declenches par des evenements.
    std::vector<std::string> trigger_messages;
};

/// Metadonnees globales du jeu chargees depuis le fichier principal.
struct GameMeta {
    /// Titre global du jeu.
    std::string title;
    /// Version logique du contenu.
    std::string version;
    /// Langue de reference des textes.
    std::string language;
    /// Nom du joueur injecte dans les textes variables.
    std::string player_name;
    /// Nom du proprietaire / interlocuteur principal.
    std::string owner_name;
    /// Titre du texte d'introduction.
    std::string intro_text_title;
    /// Sous-titre du texte d'introduction.
    std::string intro_text_subtitle;
    /// Corps du texte d'introduction.
    std::string intro_text_body;

    /// Association entre identifiants de phases et fichiers JSON.
    std::map<std::string, std::string> phases_map;
    /// Dossier racine contenant les automations.
    std::string automs_directory;
};

/// Etat runtime minimal conserve entre les ecrans et les phases.
struct GameState {
    /// Identifiant de la phase courante.
    std::string current_phase_id = "P0";
    /// Identifiant de la fin de partie courante.
    std::string ending_id = "ending_normal";

    /// Automations debloquees par le joueur.
    std::vector<std::string> unlocked_automs;
    /// Indique si la partie a deja demarre.
    bool game_started = false;
};

/// Charge les metadonnees globales du jeu depuis le JSON principal.
GameMeta load_game_meta();
/// Charge une phase a partir de son identifiant et des metadonnees globales.
GamePhase load_phase(const std::string& phase_id, const GameMeta& meta);
/// Charge une automation referencee par une phase.
Autom load_autom(const std::string& autom_ref, const GameMeta& meta);
/// Charge une fin de partie a partir de son identifiant.
Ending load_ending(const std::string& ending_id, const GameMeta& meta);

/// Remplace les variables textuelles connues dans une chaine de caracteres.
std::string replace_variables(const std::string& text, const GameMeta& meta);
/// Retourne un element choisi parmi plusieurs variantes textuelles.
std::string pick_text(const std::vector<std::string>& values);
/// Construit un etat de depart pour une nouvelle partie.
GameState init_game_state();