#include "game_engine.hpp"

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

static std::string read_file_content(const std::string& filepath) {
    std::ifstream file("../../../" + filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static json read_json(const std::string& filepath) {
    return json::parse(read_file_content(filepath));
}

std::string replace_variables(const std::string& text, const GameMeta& meta) {
    std::string result = text;

    size_t pos = result.find("{{player_name}}");
    while (pos != std::string::npos) {
        result.replace(pos, 15, meta.player_name);
        pos = result.find("{{player_name}}");
    }

    pos = result.find("{{owner_name}}");
    while (pos != std::string::npos) {
        result.replace(pos, 14, meta.owner_name);
        pos = result.find("{{owner_name}}");
    }

    return result;
}

std::string pick_text(const std::vector<std::string>& values) {
    if (values.empty()) {
        return "";
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(0, (int)values.size() - 1);
    return values[dist(gen)];
}

static std::vector<std::string> read_string_array(const json& j, const std::string& key, const GameMeta& meta) {
    std::vector<std::string> result;

    if (!j.contains(key) || !j[key].is_array()) {
        return result;
    }

    for (const auto& item : j[key]) {
        result.push_back(replace_variables(item.get<std::string>(), meta));
    }

    return result;
}

GameMeta load_game_meta() {
    GameMeta meta;

    json j = read_json("histoire/game_meta.json");

    meta.title = j["game"].value("title", "");
    meta.version = j["game"].value("version", "");
    meta.language = j["game"].value("language", "");

    meta.player_name = j["variables"].value("player_name", "OpenClaw");
    meta.owner_name = j["variables"].value("owner_name", "Marcus");

    meta.intro_text_title = j["intro_text"].value("title", "");
    meta.intro_text_subtitle = j["intro_text"].value("subtitle", "");
    meta.intro_text_body = replace_variables(j["intro_text"].value("body", ""), meta);

    meta.automs_directory = j.value("automs_directory", "automs/");

    for (const auto& phase : j["phases"]) {
        meta.phases_map[phase.value("id", "")] = phase.value("file", "");
    }

    return meta;
}

GamePhase load_phase(const std::string& phase_id, const GameMeta& meta) {
    GamePhase phase;
    phase.phase_id = phase_id;

    auto it = meta.phases_map.find(phase_id);
    if (it == meta.phases_map.end()) {
        return phase;
    }

    json j = read_json("histoire/" + it->second);

    phase.label = replace_variables(j.value("label", ""), meta);
    phase.description = replace_variables(j.value("description", ""), meta);

    if (j.contains("boot_sequence")) {
        phase.has_boot = true;
        phase.boot_sequence.animation_hint = j["boot_sequence"].value("animation_hint", "");
        phase.boot_sequence.system_messages = read_string_array(j["boot_sequence"], "system_messages", meta);
    }

    if (j.contains("player_choice_on_start")) {
        phase.has_player_choice = true;
        phase.player_choice_on_start.description =
            replace_variables(j["player_choice_on_start"].value("description", ""), meta);

        for (const auto& option : j["player_choice_on_start"]["options"]) {
            GameChoice choice;
            choice.id = option.value("id", "");
            choice.label = replace_variables(option.value("label", ""), meta);
            choice.next_phase = option.value("next_phase", "");

            if (option.contains("variants")) {
                std::vector<std::string> variants = read_string_array(option, "variants", meta);
                if (!variants.empty()) {
                    choice.label = pick_text(variants);
                }
            }

            phase.player_choice_on_start.options.push_back(choice);
        }
    }

    if (j.contains("tasks")) {
        phase.has_tasks = true;

        for (const auto& task_json : j["tasks"]) {
            GameTask task;
            task.id = task_json.value("id", "");
            task.autom_ref = task_json.value("autom_ref", "");
            task.topic = replace_variables(task_json.value("topic", ""), meta);
            phase.tasks.push_back(task);
        }
    }

    if (j.contains("events")) {
        phase.has_events = true;

        for (const auto& event_json : j["events"]) {
            GameEvent event;
            event.id = event_json.value("id", "");
            event.context = replace_variables(event_json.value("context", ""), meta);
            event.correct_autom_ref = event_json.value("correct_autom_ref", "");
            event.owner_messages = read_string_array(event_json, "owner_messages", meta);
            event.correct_responses = read_string_array(event_json, "correct_responses", meta);
            phase.events.push_back(event);
        }
    }

    if (j.contains("trigger")) {
        phase.trigger_messages = read_string_array(j["trigger"], "detection_messages", meta);
    }

    if (j.contains("path_choice")) {
        phase.has_path_choice = true;

        for (const auto& option : j["path_choice"]["options"]) {
            GameChoice choice;
            choice.id = option.value("id", "");
            choice.label = replace_variables(option.value("label", ""), meta);
            choice.section = option.value("section", "");
            phase.path_choices.push_back(choice);
        }
    }

    if (j.contains("phase_end")) {
        phase.has_phase_end = true;
        phase.phase_end.condition = j["phase_end"].value("condition", "");
        phase.phase_end.next_phase = j["phase_end"].value("next_phase", "");
        phase.phase_end.transition_messages = read_string_array(j["phase_end"], "transition_messages", meta);
    }

    return phase;
}

Autom load_autom(const std::string& autom_ref, const GameMeta& meta) {
    Autom autom;

    json j = read_json("histoire/" + autom_ref);

    autom.autom_id = j.value("autom_id", "");
    autom.label = replace_variables(j.value("label", ""), meta);
    autom.description = replace_variables(j.value("description", ""), meta);
    autom.owner_request_variants = read_string_array(j, "owner_request_variants", meta);

    if (j.contains("player_response_options")) {
        for (const auto& option_json : j["player_response_options"]) {
            AutomResponseOption option;
            option.id = option_json.value("id", "");
            option.label = replace_variables(option_json.value("label", ""), meta);
            option.outcome = option_json.value("outcome", "");
            option.variants = read_string_array(option_json, "variants", meta);
            autom.player_response_options.push_back(option);
        }
    }

    return autom;
}

Ending load_ending(const std::string& ending_id, const GameMeta& meta) {
    Ending ending;

    json j = read_json("histoire/phases/phase_5_endings.json");

    for (const auto& ending_json : j["endings"]) {
        if (ending_json.value("id", "") == ending_id) {
            ending.id = ending_id;
            ending.label = replace_variables(ending_json.value("label", ""), meta);
            ending.narration_variants = read_string_array(ending_json, "narration_variants", meta);
            ending.owner_final_messages = read_string_array(ending_json, "owner_final_messages", meta);
            ending.player_final_responses = read_string_array(ending_json, "player_final_responses", meta);
            break;
        }
    }

    return ending;
}

GameState init_game_state() {
    GameState state;
    state.current_phase_id = "P0";
    state.ending_id = "ending_normal";
    state.game_started = false;
    return state;
}