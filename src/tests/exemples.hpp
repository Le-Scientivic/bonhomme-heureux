#pragma once

#include <functional>
#include <string>
#include <vector>

// ─── Déclarations de tous les exemples ftxui ──────────────────────────────

// COMPONENT Examples
// Interactive Controls
void example_button();
void example_button_animated();
void example_button_in_frame();
void example_button_style();
void example_checkbox();
void example_checkbox_in_frame();
void example_toggle();
void example_slider();
void example_slider_direction();
void example_slider_rgb();
void example_radiobox();
void example_radiobox_in_frame();
void example_input();
void example_input_in_frame();
void example_input_style();
void example_textarea();

// Menus & Navigation
void example_menu();
void example_menu2();
void example_menu_entries();
void example_menu_entries_animated();
void example_menu_in_frame();
void example_menu_in_frame_horizontal();
void example_menu_multiple();
void example_menu_style();
void example_menu_underline_animated_gallery();
void example_dropdown();
void example_dropdown_custom();
void example_selection();

// Layout & Composition
void example_composition();
void example_resizable_split();
void example_flexbox_gallery();
void example_nested_screen();

// Visual Elements & Dialogs
void example_canvas_animated();
void example_scrollbar();
void example_collapsible();
void example_modal_dialog();
void example_modal_dialog_custom();
void example_window();

// Styling & Effects
void example_focus();
void example_focus_cursor();
void example_maybe();

// Advanced Features
void example_custom_loop();
void example_print_key_press();
void example_with_restored_io();
void example_homescreen();
void example_gallery();
void example_renderer();
void example_linear_gradient_gallery();

// DOM Examples
// Styling & Colors
void example_border();
void example_border_colored();
void example_border_style();
void example_color_gallery();
void example_color_info_palette256();
void example_color_truecolor_HSV();
void example_color_truecolor_RGB();
void example_style_bold();
void example_style_dim();
void example_style_italic();
void example_style_underlined();
void example_style_underlined_double();
void example_style_strikethrough();
void example_style_inverted();
void example_style_blink();
void example_style_color();
void example_style_gallery();
void example_style_hyperlink();
void example_linear_gradient();

// Layout Elements
void example_dbox();
void example_vbox_hbox();
void example_vflow();
void example_hflow();
void example_gridbox();
void example_size();
void example_separator();
void example_separator_style();
void example_paragraph();

// Visual Components
void example_canvas();
void example_gauge();
void example_gauge_direction();
void example_graph();
void example_spinner();
void example_table();
void example_html_like();
void example_package_manager();

// ─── Structures pour le menu ──────────────────────────────────────────────

enum class ExampleCategory {
    COMPONENTS,
    DOM
};

struct Example {
    std::string name;
    std::function<void()> function;
    ExampleCategory category;
    std::string subcategory;
};

// ─── Fonction de menu ────────────────────────────────────────────────────

/**
 * Affiche un menu interactif ftxui permettant de sélectionner et lancer
 * les 84 exemples ftxui organisés par catégories et sous-catégories.
 */
void run_examples_menu();
