#pragma once

#include "game_engine.hpp"
#include <ftxui/component/component.hpp>

class InterfaceGraphique;

using namespace ftxui;

/**
 * Affiche le texte d'introduction du jeu
 * @param meta métadonnées du jeu
 * @return true si le joueur veut commencer, false pour quitter
 */
bool show_intro(const GameMeta& meta, InterfaceGraphique& interface);

/**
 * Affiche la séquence de boot (messages système qui défilent)
 * @param phase la phase contenant la séquence de boot
 * @param state état actuel du jeu
 */
void show_boot_sequence(const GamePhase& phase, GameState& state, InterfaceGraphique& interface);

/**
 * Affiche un choix au joueur et retourne l'index du choix effectué
 * @param choices vecteur de choix disponibles
 * @return index du choix sélectionné, -1 si annulé
 */
int show_choice_menu(
	const std::vector<GameChoice>& choices,
	InterfaceGraphique& interface,
	const std::string& titre_phase = "Menu",
	const std::vector<std::string>& description_lignes = {},
	const std::string& titre_menu = "Faites votre choix"
);

/**
 * Affiche la fin de la phase avec message de transition
 * @param messages vecteur de messages de transition
 */
void show_phase_end(const std::vector<std::string>& messages, InterfaceGraphique& interface);

/**
 * Démarre et exécute le jeu complet
 * @param meta métadonnées du jeu
 */
void run_game(const GameMeta& meta, InterfaceGraphique& interface);
