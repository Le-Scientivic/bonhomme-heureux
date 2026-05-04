#pragma once

#include <memory>

#include "game_engine.hpp"
#include <ftxui/component/component.hpp>

class InterfaceGraphique;
class ModuleDeAutomatisation;
class ModuleDeCommande;
class ModuleDeDiscussion;

struct UIRenderContext {
	InterfaceGraphique* interface_graphique = nullptr;
	std::shared_ptr<ModuleDeAutomatisation> module_automatisation;
	std::shared_ptr<ModuleDeCommande> module_commande;
	std::shared_ptr<ModuleDeDiscussion> module_discussion;
};

using namespace ftxui;

/**
 * Affiche le texte d'introduction du jeu
 * @param meta métadonnées du jeu
 * @return true si le joueur veut commencer, false pour quitter
 */
bool show_intro(const GameMeta& meta, const UIRenderContext* ui_context = nullptr);

/**
 * Affiche la séquence de boot (messages système qui défilent)
 * @param phase la phase contenant la séquence de boot
 * @param state état actuel du jeu
 */
void show_boot_sequence(const GamePhase& phase, GameState& state, const UIRenderContext* ui_context = nullptr);

/**
 * Affiche un choix au joueur et retourne l'index du choix effectué
 * @param choices vecteur de choix disponibles
 * @return index du choix sélectionné, -1 si annulé
 */
int show_choice_menu(const std::vector<GameChoice>& choices, const UIRenderContext* ui_context = nullptr);

/**
 * Affiche la zone de saisie pour que le joueur tape un message
 * @param prompt texte à afficher avant la zone de saisie
 * @return message tapé par le joueur
 */
std::string show_input(const std::string& prompt);

/**
 * Affiche un message de l'histoire (typewriter effect)
 * @param message texte à afficher
 * @param speaker nom de la personne qui parle
 */
void show_message(const std::string& speaker, const std::string& message, const UIRenderContext* ui_context = nullptr);

/**
 * Affiche un message avec effet typewriter dans un composant FTXUI
 * @param message texte à afficher
 * @return Element FTXUI
 */
Element render_message_with_effect(const std::string& message);

/**
 * Affiche la fin de la phase avec message de transition
 * @param messages vecteur de messages de transition
 */
void show_phase_end(const std::vector<std::string>& messages);

/**
 * Démarre et exécute le jeu complet
 * @param meta métadonnées du jeu
 */
void run_game(const GameMeta& meta, const UIRenderContext* ui_context = nullptr);
