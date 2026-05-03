#pragma once

#include <map>
#include <string>
#include <vector>

struct GameChoice {
    std::string id;
    std::string label;
    std::string next_phase;
    std::string next_task;
    std::string section;
};

struct AutomResponseOption {
    std::string id;
    std::string label;
    std::vector<std::string> variants;
    std::string outcome;
};

struct Autom {
    std::string autom_id;
    std::string label;
    std::string description;
    std::vector<std::string> owner_request_variants;
    std::vector<AutomResponseOption> player_response_options;
};

struct GameTask {
    std::string id;
    std::string autom_ref;
    std::string topic;
    bool completed = false;
    bool accepted = false;
};

struct GameEvent {
    std::string id;
    std::string context;
    std::string correct_autom_ref;
    std::vector<std::string> owner_messages;
    std::vector<std::string> correct_responses;
};

struct Ending {
    std::string id;
    std::string label;
    std::vector<std::string> narration_variants;
    std::vector<std::string> owner_final_messages;
    std::vector<std::string> player_final_responses;
};

struct GamePhase {
    std::string phase_id;
    std::string label;
    std::string description;

    struct BootSequence {
        std::string animation_hint;
        std::vector<std::string> system_messages;
    };
    BootSequence boot_sequence;
    bool has_boot = false;

    struct PlayerChoiceOnStart {
        std::string description;
        std::vector<GameChoice> options;
    };
    PlayerChoiceOnStart player_choice_on_start;
    bool has_player_choice = false;

    std::vector<GameTask> tasks;
    bool has_tasks = false;

    std::vector<GameEvent> events;
    bool has_events = false;

    struct PhaseEnd {
        std::string condition;
        std::string next_phase;
        std::vector<std::string> transition_messages;
    };
    PhaseEnd phase_end;
    bool has_phase_end = false;

    std::vector<GameChoice> path_choices;
    bool has_path_choice = false;

    std::vector<std::string> trigger_messages;
};

struct GameMeta {
    std::string title;
    std::string version;
    std::string language;
    std::string player_name;
    std::string owner_name;
    std::string intro_text_title;
    std::string intro_text_subtitle;
    std::string intro_text_body;

    std::map<std::string, std::string> phases_map;
    std::string automs_directory;
};

struct GameState {
    std::string current_phase_id = "P0";
    std::string ending_id = "ending_normal";

    std::vector<std::string> unlocked_automs;
    bool game_started = false;
    int current_message_index = 0;
};

GameMeta load_game_meta();
GamePhase load_phase(const std::string& phase_id, const GameMeta& meta);
Autom load_autom(const std::string& autom_ref, const GameMeta& meta);
Ending load_ending(const std::string& ending_id, const GameMeta& meta);

std::string replace_variables(const std::string& text, const GameMeta& meta);
std::string pick_text(const std::vector<std::string>& values);
GameState init_game_state();