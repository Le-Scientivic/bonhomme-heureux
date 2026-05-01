// ============================================================================
// FTXUI Examples Collection
// Complete source with all 84 examples converted to functions
// Generated: Mai 2026
// ============================================================================

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <map>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/dom/direction.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/linear_gradient.hpp"
#include "ftxui/screen/color.hpp"

#include "exemples.hpp"

namespace {
class ScreenInteractiveCompat {
 public:
  enum class Mode {
    Fullscreen,
    FitComponent,
    TerminalOutput,
    Active,
  };

  ScreenInteractiveCompat() = default;

  static ScreenInteractiveCompat Fullscreen() {
    ScreenInteractiveCompat screen;
    screen.mode_ = Mode::Fullscreen;
    return screen;
  }

  static ScreenInteractiveCompat FitComponent() {
    ScreenInteractiveCompat screen;
    screen.mode_ = Mode::FitComponent;
    return screen;
  }

  static ScreenInteractiveCompat TerminalOutput() {
    ScreenInteractiveCompat screen;
    screen.mode_ = Mode::TerminalOutput;
    return screen;
  }

  static ScreenInteractiveCompat* Active() {
    static thread_local ScreenInteractiveCompat active;
    active.mode_ = Mode::Active;
    return &active;
  }

  template <typename Component>
  void Loop(Component component) {
    switch (mode_) {
      case Mode::Fullscreen: {
        auto screen = ftxui::ScreenInteractive::Fullscreen();
        screen.Loop(component);
        break;
      }
      case Mode::FitComponent: {
        auto screen = ftxui::ScreenInteractive::FitComponent();
        screen.Loop(component);
        break;
      }
      case Mode::TerminalOutput: {
        auto screen = ftxui::ScreenInteractive::TerminalOutput();
        screen.Loop(component);
        break;
      }
      case Mode::Active:
        if (auto* screen = ftxui::ScreenInteractive::Active()) {
          screen->Loop(component);
        }
        break;
    }
  }

  void Exit() {
    if (auto* screen = ftxui::ScreenInteractive::Active()) {
      screen->Exit();
    }
  }

  void Print() {
    if (auto* screen = ftxui::ScreenInteractive::Active()) {
      screen->Print();
    }
  }

  auto ExitLoopClosure() {
    return [] {
      if (auto* screen = ftxui::ScreenInteractive::Active()) {
        screen->Exit();
      }
    };
  }

  template <typename Callback>
  auto WithRestoredIO(Callback&& callback) {
    return std::forward<Callback>(callback);
  }

  template <typename T>
  void Post(T) {}

  template <typename Callback>
  void SelectionChange(Callback&& callback) {
    selection_change_ = std::forward<Callback>(callback);
  }

  std::string GetSelection() const {
    return selection_;
  }

  void ExitLoopCleaner() {}

 private:
  Mode mode_ = Mode::FitComponent;
  std::function<void()> selection_change_;
  std::string selection_;
};
}  // namespace

#define ScreenInteractive ScreenInteractiveCompat

using namespace ftxui;

template <typename Screen, typename ComponentLike>
void LoopWithQuit(Screen& screen, ComponentLike component_like) {
  Component component = component_like;
  auto with_quit = CatchEvent(component, [&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      screen.Exit();
      return true;
    }
    return false;
  });
  screen.Loop(with_quit);
}

void ShowStaticDocumentUntilQuit(Element document, bool fullscreen = false) {
  auto component = Renderer([document] {
    return vbox({
               document,
               separator(),
               text("Press q to return") | dim,
           }) |
           border;
  });

  if (fullscreen) {
    auto screen = ScreenInteractive::Fullscreen();
    LoopWithQuit(screen, component);
  } else {
    auto screen = ScreenInteractive::FitComponent();
    LoopWithQuit(screen, component);
  }
}

// ============================================================================
// COMPONENT EXAMPLES - Interactive Controls
// ============================================================================

void example_button_animated() {
  int value = 50;
  auto buttons = Container::Horizontal({
      Button("Decrease", [&] { value--; }, ButtonOption::Animated(Color::Red)),
      Button("Reset", [&] { value = 50; }, ButtonOption::Animated(Color::Green)),
      Button("Increase", [&] { value++; }, ButtonOption::Animated(Color::Blue)),
  });
  auto component = Renderer(buttons, [&] {
    return vbox({
        vbox({
            text("value = " + std::to_string(value)),
            separator(),
            gauge(value * 0.01f),
        }) | border,
        buttons->Render(),
    });
  });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, component);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_button() {
  auto screen = ScreenInteractive::Fullscreen();
  auto testComponent = Renderer([]() { return text("test Component"); });
  LoopWithQuit(screen, testComponent);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_button_in_frame() {
  int counter = 0;
  auto on_click = [&] { counter++; };
  auto style = ButtonOption::Animated(Color::Default, Color::GrayDark,
                                      Color::Default, Color::White);
  auto container = Container::Vertical({});
  for (int i = 0; i < 30; ++i) {
    auto button = Button("Button " + std::to_string(i), on_click, style);
    container->Add(button);
  }
  auto renderer = Renderer(container, [&] {
    return vbox({
               hbox({
                   text("Counter:"),
                   text(std::to_string(counter)),
               }),
               separator(),
               container->Render() | vscroll_indicator | frame |
                   size(HEIGHT, LESS_THAN, 20),
           }) |
           border;
  });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_button_style() {
  int value = 0;
  auto action = [&] { value++; };
  auto action_renderer =
      Renderer([&] { return text("count = " + std::to_string(value)); });
  auto buttons =
      Container::Vertical({
          action_renderer,
          Renderer([] { return separator(); }),
          Container::Horizontal({
              Container::Vertical({
                  Button("Ascii 1", action, ButtonOption::Ascii()),
                  Button("Ascii 2", action, ButtonOption::Ascii()),
                  Button("Ascii 3", action, ButtonOption::Ascii()),
              }),
              Renderer([] { return separator(); }),
              Container::Vertical({
                  Button("Simple 1", action, ButtonOption::Simple()),
                  Button("Simple 2", action, ButtonOption::Simple()),
                  Button("Simple 3", action, ButtonOption::Simple()),
              }),
              Renderer([] { return separator(); }),
              Container::Vertical({
                  Button("Animated 1", action, ButtonOption::Animated()),
                  Button("Animated 2", action, ButtonOption::Animated()),
                  Button("Animated 3", action, ButtonOption::Animated()),
              }),
              Renderer([] { return separator(); }),
              Container::Vertical({
                  Button("Animated 4", action,
                         ButtonOption::Animated(Color::Red)),
                  Button("Animated 5", action,
                         ButtonOption::Animated(Color::Green)),
                  Button("Animated 6", action,
                         ButtonOption::Animated(Color::Blue)),
              }),
          }),
      }) |
      border;
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, buttons);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_checkbox() {
  bool download = false;
  bool upload = false;
  bool ping = false;
  auto container = Container::Vertical({
      Checkbox("Download", &download),
      Checkbox("Upload", &upload),
      Checkbox("Ping", &ping),
  });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, container);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_checkbox_in_frame() {
  std::array<bool, 30> states;
  auto container = Container::Vertical({});
  for (int i = 0; i < 30; ++i) {
    states[i] = false;
    container->Add(Checkbox("Checkbox" + std::to_string(i), &states[i]));
  }
  auto renderer = Renderer(container, [&] {
    return container->Render() | vscroll_indicator | frame |
           size(HEIGHT, LESS_THAN, 10) | border;
  });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_toggle() {
  std::vector<std::string> toggle_1_entries = {"On", "Off"};
  std::vector<std::string> toggle_2_entries = {"Enabled", "Disabled"};
  std::vector<std::string> toggle_3_entries = {"10€", "0€"};
  std::vector<std::string> toggle_4_entries = {"Nothing", "One element",
                                                "Several elements"};
  int toggle_1_selected = 0;
  int toggle_2_selected = 0;
  int toggle_3_selected = 0;
  int toggle_4_selected = 0;
  Component toggle_1 = Toggle(&toggle_1_entries, &toggle_1_selected);
  Component toggle_2 = Toggle(&toggle_2_entries, &toggle_2_selected);
  Component toggle_3 = Toggle(&toggle_3_entries, &toggle_3_selected);
  Component toggle_4 = Toggle(&toggle_4_entries, &toggle_4_selected);
  auto container = Container::Vertical({toggle_1, toggle_2, toggle_3, toggle_4});
  auto renderer = Renderer(container, [&] {
    return vbox({
        text("Choose your options:"),
        text(""),
        hbox(text(" * Poweroff on startup      : "), toggle_1->Render()),
        hbox(text(" * Out of process           : "), toggle_2->Render()),
        hbox(text(" * Price of the information : "), toggle_3->Render()),
        hbox(text(" * Number of elements       : "), toggle_4->Render()),
    });
  });
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_slider() {
  auto screen = ScreenInteractive::TerminalOutput();
  int value = 50;
  auto slider = Slider("Value:", &value, 0, 100, 1);
  LoopWithQuit(screen, slider);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_slider_direction() {
  auto screen = ScreenInteractive::TerminalOutput();
  std::array<int, 30> values;
  for (size_t i = 0; i < values.size(); ++i) {
    values[i] = 50 + 20 * std::sin(i * 0.3);
  }
  auto layout_horizontal = Container::Horizontal({});
  for (auto& value : values) {
    SliderOption<int> option;
    option.value = &value;
    option.max = 100;
    option.increment = 5;
    option.direction = Direction::Up;
    layout_horizontal->Add(Slider<int>(option));
  }
  layout_horizontal |= size(HEIGHT, GREATER_THAN, 20);
  LoopWithQuit(screen, layout_horizontal);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_slider_rgb() {
  int red = 128;
  int green = 25;
  int blue = 100;
  auto slider_red = Slider("Red  :", &red, 0, 255, 1);
  auto slider_green = Slider("Green:", &green, 0, 255, 1);
  auto slider_blue = Slider("Blue :", &blue, 0, 255, 1);
  auto container = Container::Vertical({slider_red, slider_green, slider_blue});
  auto renderer = Renderer(container, [&] {
    auto color_tile = [](int r, int g, int b) {
      return text("") | size(WIDTH, GREATER_THAN, 14) |
             size(HEIGHT, GREATER_THAN, 7) | bgcolor(Color::RGB(r, g, b));
    };
    auto color_string = [](int r, int g, int b) {
      return text("RGB = (" + std::to_string(r) + "," + std::to_string(g) +
                  "," + std::to_string(b) + ")");
    };
    return hbox({
               color_tile(red, green, blue),
               separator(),
               vbox({
                   slider_red->Render(),
                   separator(),
                   slider_green->Render(),
                   separator(),
                   slider_blue->Render(),
                   separator(),
                   color_string(red, green, blue),
               }) | xflex,
           }) |
           border | size(WIDTH, LESS_THAN, 80);
  });
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_radiobox() {
  std::vector<std::string> radiobox_list = {
      "Use gcc", "Use clang", "Use emscripten", "Use tcc",
  };
  int selected = 0;
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, Radiobox(&radiobox_list, &selected));
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_radiobox_in_frame() {
  std::vector<std::string> entries;
  int selected = 0;
  for (int i = 0; i < 30; ++i)
    entries.push_back("RadioBox " + std::to_string(i));
  auto radiobox = Radiobox(&entries, &selected);
  auto renderer = Renderer(radiobox, [&] {
    return radiobox->Render() | vscroll_indicator | frame |
           size(HEIGHT, LESS_THAN, 10) | border;
  });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_input() {
  std::string first_name;
  std::string last_name;
  std::string password;
  std::string phoneNumber;
  Component input_first_name = Input(&first_name, "first name");
  Component input_last_name = Input(&last_name, "last name");
  InputOption password_option;
  password_option.password = true;
  Component input_password = Input(&password, "password", password_option);
  Component input_phone_number = Input(&phoneNumber, "phone number");
  input_phone_number |= CatchEvent([&](Event event) {
    return event.is_character() && !std::isdigit(event.character()[0]);
  });
  input_phone_number |= CatchEvent([&](Event event) {
    return event.is_character() && phoneNumber.size() > 10;
  });
  auto component = Container::Vertical(
      {input_first_name, input_last_name, input_password, input_phone_number});
  auto renderer = Renderer(component, [&] {
    return vbox({
               hbox(text(" First name : "), input_first_name->Render()),
               hbox(text(" Last name  : "), input_last_name->Render()),
               hbox(text(" Password   : "), input_password->Render()),
               hbox(text(" Phone num  : "), input_phone_number->Render()),
               separator(),
               text("Hello " + first_name + " " + last_name),
               text("Your password is " + password),
               text("Your phone number is " + phoneNumber),
           }) |
           border;
  });
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_input_in_frame() {
  Component input_list = Container::Vertical({});
  std::vector<std::string> items(100, "");
  for (size_t i = 0; i < items.size(); ++i) {
    input_list->Add(Input(&(items[i]), "placeholder " + std::to_string(i)));
  }
  auto renderer = Renderer(input_list, [&] {
    return input_list->Render() | vscroll_indicator | frame | border |
           size(HEIGHT, LESS_THAN, 10);
  });
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_input_style() {
  InputOption style_1 = InputOption::Default();
  InputOption style_2 = InputOption::Spacious();
  InputOption style_3 = InputOption::Spacious();
  style_3.transform = [](InputState state) {
    state.element |= borderEmpty;
    if (state.is_placeholder) {
      state.element |= dim;
    }
    if (state.focused) {
      state.element |= borderDouble;
      state.element |= bgcolor(Color::White);
      state.element |= color(Color::Black);
    } else if (state.hovered) {
      state.element |= borderRounded;
      state.element |=
          bgcolor(LinearGradient(90, Color::Blue, Color::Red));
      state.element |= color(Color::White);
    } else {
      state.element |= border;
      state.element |=
          bgcolor(LinearGradient(0, Color::Blue, Color::Red));
      state.element |= color(Color::White);
    }
    return state.element;
  };
  InputOption style_4 = InputOption::Spacious();
  style_4.transform = [](InputState state) {
    state.element = hbox({
        text("Theorem") | center | borderEmpty | bgcolor(Color::Red),
        separatorEmpty(),
        separator() | color(Color::White),
        separatorEmpty(),
        std::move(state.element),
    });
    state.element |= borderEmpty;
    if (state.is_placeholder) {
      state.element |= dim;
    }
    if (state.focused) {
      state.element |= bgcolor(Color::Black);
    } else {
      state.element |= bgcolor(Color::Blue);
    }
    if (state.hovered) {
      state.element |= bgcolor(Color::GrayDark);
    }
    return vbox({state.element, separatorEmpty()});
  };
  auto generateUiFromStyle = [&](InputOption style) {
    auto first_name = new std::string();
    auto middle_name = new std::string();
    auto last_name = new std::string();
    return Container::Vertical(
               {Input(first_name, "first name", style),
                Input(middle_name, "middle name", style),
                Input(last_name, "last name", style)}) |
           borderEmpty;
  };
  auto ui = Container::Horizontal({
      generateUiFromStyle(style_1),
      generateUiFromStyle(style_2),
      generateUiFromStyle(style_3),
      generateUiFromStyle(style_4),
  });
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, ui);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_textarea() {
  std::string content_1;
  std::string content_2;
  auto textarea_1 = Input(&content_1);
  auto textarea_2 = Input(&content_2);
  int size = 50;
  auto layout = ResizableSplitLeft(textarea_1, textarea_2, &size);
  auto component = Renderer(layout, [&] {
    return vbox({
               text("Input:"),
               separator(),
               layout->Render() | flex,
           }) |
           border;
  });
  auto screen = ScreenInteractive::Fullscreen();
  LoopWithQuit(screen, component);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

// ============================================================================
// COMPONENT EXAMPLES - Menus & Navigation
// ============================================================================

void example_menu() {
  auto screen = ScreenInteractive::TerminalOutput();
  std::vector<std::string> entries = {"entry 1", "entry 2", "entry 3"};
  int selected = 0;
  MenuOption option;
  option.on_enter = screen.ExitLoopClosure();
  auto menu = Menu(&entries, &selected, option);
  LoopWithQuit(screen, menu);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_menu2() {
  auto screen = ScreenInteractive::TerminalOutput();
  std::vector<std::string> left_menu_entries = {
      "0%", "10%", "20%", "30%", "40%", "50%", "60%", "70%", "80%", "90%",
  };
  std::vector<std::string> right_menu_entries = {
      "0%", "1%", "2%", "3%", "4%", "5%", "6%", "7%", "8%", "9%", "10%",
  };
  auto menu_option = MenuOption();
  menu_option.on_enter = screen.ExitLoopClosure();
  int left_menu_selected = 0;
  int right_menu_selected = 0;
  Component left_menu_ = Menu(&left_menu_entries, &left_menu_selected, menu_option);
  Component right_menu_ =
      Menu(&right_menu_entries, &right_menu_selected, menu_option);
  Component container = Container::Horizontal({left_menu_, right_menu_});
  auto renderer = Renderer(container, [&] {
    int sum = left_menu_selected * 10 + right_menu_selected;
    return vbox({
               hbox({
                   vbox({
                       hcenter(bold(text("Percentage by 10%"))),
                       separator(),
                       left_menu_->Render(),
                   }),
                   separator(),
                   vbox({
                       hcenter(bold(text("Percentage by 1%"))),
                       separator(),
                       right_menu_->Render(),
                   }),
                   separator(),
               }),
               separator(),
               vbox({
                   hbox({
                       text(" gauge : "),
                       gauge(sum / 100.0),
                   }),
                   hbox({
                       text("  text : "),
                       text(std::to_string(sum) + " %"),
                   }),
               }),
           }) |
           border;
  });
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_menu_entries() {
  auto screen = ScreenInteractive::TerminalOutput();
  int selected = 0;
  auto menu = Container::Vertical(
      {
          MenuEntry(" 1. improve"),
          MenuEntry(" 2. tolerant"),
          MenuEntry(" 3. career"),
          MenuEntry(" 4. cast"),
          MenuEntry(" 5. question"),
          Renderer([] { return separator(); }),
          MenuEntry(" 6. rear"),
          MenuEntry(" 7. drown"),
          MenuEntry(" 8. nail"),
          MenuEntry(" 9. quit"),
          MenuEntry("10. decorative"),
          Renderer([] { return separator(); }),
          MenuEntry("11. costume"),
          MenuEntry("12. pick"),
          MenuEntry("13. oral"),
          MenuEntry("14. minister"),
          MenuEntry("15. football"),
          MenuEntry("16. welcome"),
          MenuEntry("17. copper"),
          MenuEntry("18. inhabitant"),
          MenuEntry("19. fortune"),
      },
      &selected);
  auto renderer = Renderer(menu, [&] {
    return vbox({
               hbox(text("selected = "), text(std::to_string(selected))),
               separator(),
               menu->Render() | frame | size(HEIGHT, LESS_THAN, 10),
           }) |
           border;
  });
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_menu_entries_animated() {
  auto screen = ScreenInteractive::TerminalOutput();
  int selected = 0;
  auto menu = Container::Vertical(
      {
          MenuEntry(" 1. rear"),
          MenuEntry(" 2. drown"),
          MenuEntry(" 3. nail"),
          MenuEntry(" 4. quit"),
          MenuEntry(" 5. decorative"),
          MenuEntry(" 7. costume"),
          MenuEntry(" 8. pick"),
          MenuEntry(" 9. oral"),
          MenuEntry("11. minister"),
          MenuEntry("12. football"),
          MenuEntry("13. welcome"),
          MenuEntry("14. copper"),
          MenuEntry("15. inhabitant"),
      },
      &selected);
  auto renderer = Renderer(menu, [&] {
    return vbox({
               hbox(text("selected = "), text(std::to_string(selected))),
               separator(),
               menu->Render() | frame,
           }) |
           border | bgcolor(Color::Black);
  });
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_menu_in_frame() {
  std::vector<std::string> entries;
  int selected = 0;
  for (int i = 0; i < 30; ++i)
    entries.push_back("Entry " + std::to_string(i));
  auto radiobox = Menu(&entries, &selected);
  auto renderer = Renderer(radiobox, [&] {
    return radiobox->Render() | vscroll_indicator | frame |
           size(HEIGHT, LESS_THAN, 10) | border;
  });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_menu_in_frame_horizontal() {
  std::vector<std::string> entries;
  int selected = 0;
  for (int i = 0; i < 100; ++i)
    entries.push_back(std::to_string(i));
  auto radiobox = Menu(&entries, &selected, MenuOption::Horizontal());
  auto renderer = Renderer(
      radiobox, [&] { return radiobox->Render() | hscroll_indicator | frame; });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_menu_multiple() {
  int menu_selected[] = {0, 0, 0};
  std::vector<std::vector<std::string>> menu_entries = {
      {"Ananas", "Raspberry", "Citrus"},
      {"Potatoes", "Weat", "Rise"},
      {"Carrot", "Lettuce", "Tomato"},
  };
  int menu_selected_global = 0;
  auto menu_1 = Menu(&menu_entries[0], &menu_selected[0]);
  auto menu_2 = Menu(&menu_entries[1], &menu_selected[1]);
  auto menu_3 = Menu(&menu_entries[2], &menu_selected[2]);
  auto menu_global = Container::Vertical(
      {
          Renderer(menu_1, [menu_1] {
            return window(text(" Menu 1 "), menu_1->Render()) | flex;
          }),
          Renderer(menu_2, [menu_2] {
            return window(text(" Menu 2 "), menu_2->Render()) | flex;
          }),
          Renderer(menu_3, [menu_3] {
            return window(text(" Menu 3 "), menu_3->Render()) | flex;
          }),
      },
      &menu_selected_global);
  auto info = Renderer([&] {
    int g = menu_selected_global;
    std::string value = menu_entries[g][menu_selected[g]];
    return window(text("Content"),
                  vbox({
                      text("menu_selected_global = " + std::to_string(g)),
                      text("menu_selected[0]     = " +
                           std::to_string(menu_selected[0])),
                      text("menu_selected[1]     = " +
                           std::to_string(menu_selected[1])),
                      text("menu_selected[2]     = " +
                           std::to_string(menu_selected[2])),
                      text("Value                = " + value),
                  })) |
           flex;
  });
  auto global = Container::Horizontal({menu_global, info});
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, global);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_menu_style() {
  auto screen = ScreenInteractive::TerminalOutput();
  std::vector<std::string> entries{"Monkey", "Dog", "Cat", "Bird", "Elephant", "Cat"};
  int selected = 0;
  auto menu = Menu(&entries, &selected);
  LoopWithQuit(screen, menu);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_menu_underline_animated_gallery() {
  auto screen = ScreenInteractive::TerminalOutput();
  std::vector<std::string> tab_values{"Tab 1", "Tab 2", "Tab 3", "A very very long tab", "탭"};
  int tab_selected = 0;
  auto container = Container::Vertical({});
  auto option = MenuOption::HorizontalAnimated();
  container->Add(Menu(&tab_values, &tab_selected, option));
  LoopWithQuit(screen, container);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_dropdown() {
  std::vector<std::string> entries = {
      "tribute", "clearance", "ally", "bend", "electronics",
      "module", "era", "cultural", "sniff", "nationalism",
      "negotiation", "deliver", "figure", "east", "tribute",
      "clearance", "ally", "bend", "electronics", "module",
      "era", "cultural", "sniff", "nationalism", "negotiation",
      "deliver", "figure", "east", "tribute", "clearance",
      "ally", "bend", "electronics", "module", "era",
      "cultural", "sniff", "nationalism", "negotiation", "deliver",
      "figure", "east",
  };
  int selected_1 = 0;
  int selected_2 = 0;
  int selected_3 = 0;
  int selected_4 = 0;
  auto layout = Container::Vertical({
      Container::Horizontal({
          Dropdown(&entries, &selected_1),
          Dropdown(&entries, &selected_2),
      }),
      Container::Horizontal({
          Dropdown(&entries, &selected_3),
          Dropdown(&entries, &selected_4),
      }),
  });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, layout);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_dropdown_custom() {
  std::vector<std::string> entries = {
      "tribute", "clearance", "ally", "bend", "electronics",
      "module", "era", "cultural", "sniff", "nationalism",
      "negotiation", "deliver", "figure", "east", "tribute",
      "clearance", "ally", "bend", "electronics", "module",
      "era", "cultural", "sniff", "nationalism", "negotiation",
      "deliver", "figure", "east", "tribute", "clearance",
      "ally", "bend", "electronics", "module", "era",
      "cultural", "sniff", "nationalism", "negotiation", "deliver",
      "figure", "east",
  };
  auto dropdown_1 = Dropdown({
      .radiobox = {.entries = &entries},
      .transform =
          [](bool open, Element checkbox, Element radiobox) {
            if (open) {
              return vbox({
                  checkbox | inverted,
                  radiobox | vscroll_indicator | frame |
                      size(HEIGHT, LESS_THAN, 10),
                  filler(),
              });
            }
            return vbox({
                checkbox,
                filler(),
            });
          },
  });
  auto dropdown_2 = Dropdown({
      .radiobox = {.entries = &entries},
      .transform =
          [](bool open, Element checkbox, Element radiobox) {
            if (open) {
              return vbox({
                  checkbox | inverted,
                  radiobox | vscroll_indicator | frame |
                      size(HEIGHT, LESS_THAN, 10) | bgcolor(Color::Blue),
                  filler(),
              });
            }
            return vbox({
                checkbox | bgcolor(Color::Blue),
                filler(),
            });
          },
  });
  auto dropdown_3 = Dropdown({
      .radiobox =
          {
              .entries = &entries,
              .transform =
                  [](const EntryState& s) {
                    auto t = text(s.label) | borderEmpty;
                    if (s.active) {
                      t |= bold;
                    }
                    if (s.focused) {
                      t |= inverted;
                    }
                    return t;
                  },
          },
      .transform =
          [](bool open, Element checkbox, Element radiobox) {
            checkbox |= borderEmpty;
            if (open) {
              return vbox({
                  checkbox | inverted,
                  radiobox | vscroll_indicator | frame |
                      size(HEIGHT, LESS_THAN, 20) | bgcolor(Color::Red),
                  filler(),
              });
            }
            return vbox({
                checkbox | bgcolor(Color::Red),
                filler(),
            });
          },
  });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, Container::Horizontal({dropdown_1, dropdown_2, dropdown_3}));
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_selection() {
  auto screen = ScreenInteractive::TerminalOutput();
  auto quit = Button("Quit", screen.ExitLoopClosure(), ButtonOption::Animated());
  int selection_change_counter = 0;
  std::string selection_content = "";
  screen.SelectionChange([&] {
    selection_change_counter++;
    selection_content = screen.GetSelection();
  });
  auto renderer = Renderer(quit, [&] {
    return vbox({
        text("Select changed: " + std::to_string(selection_change_counter) +
             " times"),
        text("Currently selected: "),
        paragraph(selection_content) | vscroll_indicator | frame | border |
            size(HEIGHT, EQUAL, 10),
        quit->Render(),
    });
  });
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_tab_horizontal() {
  std::vector<std::string> tab_values{"tab_1", "tab_2", "tab_3"};
  int tab_selected = 0;
  auto tab_toggle = Toggle(&tab_values, &tab_selected);
  std::vector<std::string> tab_1_entries{"Forest", "Water", "I don't know"};
  int tab_1_selected = 0;
  std::vector<std::string> tab_2_entries{"Hello", "Hi", "Hay"};
  int tab_2_selected = 0;
  std::vector<std::string> tab_3_entries{"Table", "Nothing", "Is", "Empty"};
  int tab_3_selected = 0;
  auto tab_container = Container::Tab(
      {
          Radiobox(&tab_1_entries, &tab_1_selected),
          Radiobox(&tab_2_entries, &tab_2_selected),
          Radiobox(&tab_3_entries, &tab_3_selected),
      },
      &tab_selected);
  auto container = Container::Vertical({tab_toggle, tab_container});
  auto renderer = Renderer(container, [&] {
    return vbox({
               tab_toggle->Render(),
               separator(),
               tab_container->Render(),
           }) |
           border;
  });
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_tab_vertical() {
  std::vector<std::string> tab_values{"tab_1", "tab_2", "tab_3"};
  int tab_selected = 0;
  auto tab_menu = Menu(&tab_values, &tab_selected);
  std::vector<std::string> tab_1_entries{"Forest", "Water", "I don't know"};
  int tab_1_selected = 0;
  std::vector<std::string> tab_2_entries{"Hello", "Hi", "Hay"};
  int tab_2_selected = 0;
  std::vector<std::string> tab_3_entries{"Table", "Nothing", "Is", "Empty"};
  int tab_3_selected = 0;
  auto tab_container = Container::Tab(
      {
          Radiobox(&tab_1_entries, &tab_1_selected),
          Radiobox(&tab_2_entries, &tab_2_selected),
          Radiobox(&tab_3_entries, &tab_3_selected),
      },
      &tab_selected);
  auto container = Container::Horizontal({tab_menu, tab_container});
  auto renderer = Renderer(container, [&] {
    return hbox({
               tab_menu->Render(),
               separator(),
               tab_container->Render(),
           }) |
           border;
  });
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

// ============================================================================
// COMPONENT EXAMPLES - Layout
// ============================================================================

void example_composition() {
  auto left_count = 0;
  auto right_count = 0;
  auto left_buttons = Container::Horizontal({
      Button("Decrease", [&] { left_count--; }),
      Button("Increase", [&] { left_count++; }),
  });
  auto right_buttons = Container::Horizontal({
      Button("Decrease", [&] { right_count--; }),
      Button("Increase", [&] { right_count++; }),
  });
  auto leftpane = Renderer(left_buttons, [&] {
    return vbox({
               text("This is the left control"),
               separator(),
               text("Left button count: " + std::to_string(left_count)),
               left_buttons->Render(),
           }) |
           border;
  });
  auto rightpane = Renderer(right_buttons, [&] {
    return vbox({
               text("This is the right control"),
               separator(),
               text("Right button count: " + std::to_string(right_count)),
               right_buttons->Render(),
           }) |
           border;
  });
  auto composition = Container::Horizontal({leftpane, rightpane});
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, composition);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_resizable_split() {
  auto screen = ScreenInteractive::Fullscreen();
  auto middle = Renderer([] { return text("middle") | center; });
  auto left = Renderer([] { return text("Left") | center; });
  auto right = Renderer([] { return text("right") | center; });
  auto top = Renderer([] { return text("top") | center; });
  auto bottom = Renderer([] { return text("bottom") | center; });
  int left_size = 20;
  int right_size = 20;
  int top_size = 10;
  int bottom_size = 10;
  auto container = middle;
  container = ResizableSplitLeft(left, container, &left_size);
  container = ResizableSplitRight(right, container, &right_size);
  container = ResizableSplitTop(top, container, &top_size);
  container = ResizableSplitBottom(bottom, container, &bottom_size);
  auto renderer =
      Renderer(container, [&] { return container->Render() | border; });
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_flexbox_gallery() {
  auto screen = ScreenInteractive::Fullscreen();
  int direction_index = 0;
  std::vector<std::string> directions = {
      "Row", "RowInversed", "Column", "ColumnInversed",
  };
  auto radiobox_direction = Radiobox(&directions, &direction_index);
  auto renderer = Renderer(radiobox_direction, [&] {
    return vbox({
               text("Direction: " + directions[direction_index]),
               separator(),
               radiobox_direction->Render(),
           }) |
           border;
  });
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_nested_screen() {
  auto screen = ScreenInteractive::FitComponent();
  auto button_quit = Button("Quit", screen.ExitLoopClosure());
  auto button_nested = Button("Nested", [] {
    auto nested_screen = ScreenInteractive::FitComponent();
    auto back = Button("Back", nested_screen.ExitLoopClosure());
    LoopWithQuit(nested_screen, back);
  });
  LoopWithQuit(screen, Container::Vertical({button_quit, button_nested}));
  ScreenInteractive::Active()->ExitLoopCleaner();
}

// ============================================================================
// COMPONENT EXAMPLES - Visual & Dialogs
// ============================================================================

void example_canvas_animated() {
  int mouse_x = 0;
  int mouse_y = 0;
  auto renderer_line_braille = Renderer([&] {
    auto c = Canvas(100, 100);
    c.DrawText(0, 0, "Several lines (braille)");
    c.DrawPointLine(mouse_x, mouse_y, 80, 10, Color::Red);
    c.DrawPointLine(80, 10, 80, 40, Color::Blue);
    c.DrawPointLine(80, 40, mouse_x, mouse_y, Color::Green);
    return canvas(std::move(c));
  });
  std::vector<std::string> tab_titles = {"line (braille)"};
  int selected_tab = 0;
  auto tab = Container::Tab({renderer_line_braille}, &selected_tab);
  auto tab_with_mouse = CatchEvent(tab, [&](Event e) {
    if (e.is_mouse()) {
      mouse_x = (e.mouse().x - 1) * 2;
      mouse_y = (e.mouse().y - 1) * 4;
    }
    return false;
  });
  auto component = Container::Horizontal({
      tab_with_mouse,
      Menu(&tab_titles, &selected_tab),
  });
  auto component_renderer = Renderer(component, [&] {
    return hbox({
               tab_with_mouse->Render(),
               separator(),
               Menu(&tab_titles, &selected_tab)->Render(),
           }) |
           border;
  });
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, component_renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_scrollbar() {
  auto screen = ScreenInteractive::Fullscreen();
  LoopWithQuit(screen, Renderer([] { return text("Scrollbar example"); }));
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_collapsible() {
  auto Empty = [] { return std::make_shared<ComponentBase>(); };
  auto Inner = [&](std::vector<Component> children) {
    Component vlist = Container::Vertical(std::move(children));
    return Renderer(vlist, [vlist] {
      return hbox({
          text(" "),
          vlist->Render(),
      });
    });
  };
  auto component = Collapsible("Collapsible 1",
                               Inner({
                                   Collapsible("Collapsible 1.1", Empty()),
                                   Collapsible("Collapsible 1.2", Empty()),
                                   Collapsible("Collapsible 1.3", Empty()),
                               }));
  auto screen = ScreenInteractive::FitComponent();
  LoopWithQuit(screen, component);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_modal_dialog() {
  auto screen = ScreenInteractive::TerminalOutput();
  bool modal_shown = false;
  auto show_modal = [&] { modal_shown = true; };
  auto hide_modal = [&] { modal_shown = false; };
  auto exit = screen.ExitLoopClosure();
  auto do_nothing = [&] {};
  auto button_style = ButtonOption::Animated();
  auto main_component = Container::Vertical({
      Button("Show modal", show_modal, button_style),
      Button("Quit", exit, button_style),
  });
  main_component |= Renderer([&](Element inner) {
    return vbox({
               text("Main component"),
               separator(),
               inner,
           }) |
           size(WIDTH, GREATER_THAN, 15) | size(HEIGHT, GREATER_THAN, 15) |
           border | center;
  });
  auto modal_component = Container::Vertical({
      Button("Do nothing", do_nothing, button_style),
      Button("Quit modal", hide_modal, button_style),
  });
  modal_component |= Renderer([&](Element inner) {
    return vbox({
               text("Modal component"),
               separator(),
               inner,
           }) |
           size(WIDTH, GREATER_THAN, 30) | border;
  });
  main_component |= Modal(modal_component, &modal_shown);
  LoopWithQuit(screen, main_component);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_modal_dialog_custom() {
  auto screen = ScreenInteractive::TerminalOutput();
  int depth = 0;
  std::string rating = "3/5 stars";
  auto button_rate_ftxui = Button("Rate FTXUI", [&] { depth = 1; });
  auto button_quit = Button("Quit", screen.ExitLoopClosure());
  auto depth_0_container = Container::Horizontal({button_rate_ftxui, button_quit});
  auto depth_0_renderer = Renderer(depth_0_container, [&] {
    return vbox({
               text("Modal dialog example"),
               separator(),
               text("☆☆☆ FTXUI:" + rating + " ☆☆☆") | bold,
               filler(),
               hbox({
                   button_rate_ftxui->Render(),
                   filler(),
                   button_quit->Render(),
               }),
           }) |
           border | size(HEIGHT, GREATER_THAN, 18) | center;
  });
  std::vector<std::string> rating_labels = {
      "1/5 stars", "2/5 stars", "3/5 stars", "4/5 stars", "5/5 stars",
  };
  auto on_rating = [&](std::string new_rating) {
    rating = new_rating;
    depth = 0;
  };
  auto depth_1_container = Container::Horizontal({
      Button(&rating_labels[0], [&] { on_rating(rating_labels[0]); }),
      Button(&rating_labels[1], [&] { on_rating(rating_labels[1]); }),
      Button(&rating_labels[2], [&] { on_rating(rating_labels[2]); }),
      Button(&rating_labels[3], [&] { on_rating(rating_labels[3]); }),
      Button(&rating_labels[4], [&] { on_rating(rating_labels[4]); }),
  });
  auto depth_1_renderer = Renderer(depth_1_container, [&] {
    return vbox({
               text("Do you like FTXUI?"),
               separator(),
               hbox(depth_1_container->Render()),
           }) |
           border;
  });
  auto main_container =
      Container::Tab({depth_0_renderer, depth_1_renderer}, &depth);
  auto main_renderer = Renderer(main_container, [&] {
    Element document = depth_0_renderer->Render();
    if (depth == 1) {
      document = dbox({
          document,
          depth_1_renderer->Render() | clear_under | center,
      });
    }
    return document;
  });
  LoopWithQuit(screen, main_renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_window() {
  auto screen = ScreenInteractive::Fullscreen();
  auto content = Renderer([] { return text("Window content"); });
  auto window_1 = Window({
      .inner = content,
      .title = "First window",
      .left = 20,
      .top = 10,
      .width = 40,
      .height = 20,
  });
  auto window_container = Container::Stacked({window_1});
  LoopWithQuit(screen, window_container);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_focus() {
  float focus_x = 0.5f;
  float focus_y = 0.5f;
  auto slider_x = Slider("x", &focus_x, 0.f, 1.f, 0.01f);
  auto slider_y = Slider("y", &focus_y, 0.f, 1.f, 0.01f);
  auto renderer = Renderer(
      Container::Vertical({slider_x, slider_y}),
      [&] {
        return vbox({
                   text("focusPositionRelative(" + std::to_string(focus_x) +
                        ", " + std::to_string(focus_y) + ")"),
                   separator(),
                   slider_x->Render(),
                   slider_y->Render(),
               }) |
               border;
      });
  auto screen = ScreenInteractive::Fullscreen();
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_focus_cursor() {
  auto screen = ScreenInteractive::Fullscreen();
  auto Instance = [&](std::string label, Decorator focusCursor) {
    return Renderer([=](bool focused) {
      if (focused) {
        return hbox({
            text("> " + label + " "),
            focusCursor(text(" ")),
        });
      }
      return text("  " + label + " ");
    });
  };
  LoopWithQuit(screen, Container::Vertical({
      Instance("focus", focus),
      Instance("focusCursorBlock", focusCursorBlock),
      Instance("focusCursorBar", focusCursorBar),
      Instance("focusCursorUnderline", focusCursorUnderline),
  }));
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_maybe() {
  std::vector<std::string> entries = {"entry 1", "entry 2", "entry 3"};
  int menu_1_selected = 0;
  int menu_2_selected = 0;
  bool menu_1_show = false;
  bool menu_2_show = false;
  auto layout = Container::Vertical({
      Checkbox("Show menu_1", &menu_1_show),
      Radiobox(&entries, &menu_1_selected) | border | Maybe(&menu_1_show),
      Checkbox("Show menu_2", &menu_2_show),
      Radiobox(&entries, &menu_2_selected) | border | Maybe(&menu_2_show),
      Renderer([] {
        return text("You found the secret combinaison!") | color(Color::Red);
      }) | Maybe([&] { return menu_1_selected == 1 && menu_2_selected == 2; }),
  });
  auto screen = ScreenInteractive::TerminalOutput();
  LoopWithQuit(screen, layout);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

// ============================================================================
// COMPONENT EXAMPLES - Advanced
// ============================================================================

void example_custom_loop() {
  #undef ScreenInteractive
  auto screen = ftxui::ScreenInteractive::FitComponent();
  int custom_loop_count = 0;
  int frame_count = 0;
  int event_count = 0;
  bool should_quit = false;
  auto component = Renderer([&] {
    frame_count++;
    return vbox({
               text("This demonstrates using a custom ftxui::Loop."),
               text("ftxui event count: " + std::to_string(event_count)),
               text("ftxui frame count: " + std::to_string(frame_count)),
               text("Custom loop count: " + std::to_string(custom_loop_count)),
               separator(),
               text("Press q to return") | dim,
           }) |
           border;
  });
  component |= CatchEvent([&](Event event) -> bool {
    event_count++;
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      should_quit = true;
      return true;
    }
    return false;
  });
  Loop loop(&screen, component);
  while (!loop.HasQuitted() && !should_quit) {
    custom_loop_count++;
    loop.RunOnce();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  #define ScreenInteractive ScreenInteractiveCompat
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_print_key_press() {
  auto screen = ScreenInteractive::TerminalOutput();
  std::vector<Event> keys;
  auto left_column = Renderer([&] {
    Elements children = {text("Codes"), separator()};
    for (size_t i = std::max(0, (int)keys.size() - 20); i < keys.size(); ++i) {
      std::string codes;
      for (auto& it : keys[i].input()) {
        codes += " " + std::to_string((unsigned int)it);
      }
      children.push_back(text(codes));
    }
    return vbox(children);
  });
  auto right_column = Renderer([&] {
    Elements children = {text("Event"), separator()};
    for (size_t i = std::max(0, (int)keys.size() - 20); i < keys.size(); ++i) {
      children.push_back(text(keys[i].DebugString()));
    }
    return vbox(children);
  });
  int split_size = 40;
  auto component = ResizableSplitLeft(left_column, right_column, &split_size);
  component |= border;
  component |= CatchEvent([&](Event event) {
    keys.push_back(event);
    return false;
  });
  LoopWithQuit(screen, component);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_with_restored_io() {
  auto screen = ScreenInteractive::Fullscreen();
  auto btn_run = Button("Execute with restored IO", screen.WithRestoredIO([] {
    std::cout << "This is a child program using stdin/stdout." << std::endl;
  }));
  auto btn_quit = Button("Quit", screen.ExitLoopClosure());
  auto layout = Container::Horizontal({btn_run, btn_quit});
  auto renderer = Renderer(layout, [&] {
    return vbox({
               text("Click button to execute child program"),
               hbox({
                   btn_run->Render(),
                   filler(),
                   btn_quit->Render(),
               }),
           }) |
           border | center;
  });
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_homescreen() {
  auto screen = ScreenInteractive::Fullscreen();
  int shift = 0;
  auto htop = Renderer([&] {
    return vbox({
               text("HTOP Mock"),
               separator(),
               text("Frequency: " + std::to_string(shift % 100) + "%"),
           }) |
           flex;
  });
  int tab_index = 0;
  std::vector<std::string> tab_entries = {"htop"};
  auto tab_selection = Menu(&tab_entries, &tab_index, MenuOption::HorizontalAnimated());
  auto tab_content = Container::Tab({htop}, &tab_index);
  auto exit_button = Button("Exit", [&] { screen.Exit(); }, ButtonOption::Animated());
  auto main_container = Container::Vertical({
      Container::Horizontal({
          tab_selection,
          exit_button,
      }),
      tab_content,
  });
  auto main_renderer = Renderer(main_container, [&] {
    return vbox({
        text("FTXUI Demo") | bold | hcenter,
        hbox({
            tab_selection->Render() | flex,
            exit_button->Render(),
        }),
        tab_content->Render() | flex,
    });
  });
  std::atomic<bool> refresh_ui_continue = true;
  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      screen.Post([&] { shift++; });
      screen.Post(Event::Custom);
    }
  });
  LoopWithQuit(screen, main_renderer);
  refresh_ui_continue = false;
  refresh_ui.join();
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_gallery() {
  auto screen = ScreenInteractive::FitComponent();
  const std::vector<std::string> menu_entries = {
      "Menu 1", "Menu 2", "Menu 3", "Menu 4",
  };
  int menu_selected = 0;
  auto menu = Menu(&menu_entries, &menu_selected);
  std::string input_label;
  auto input = Input(&input_label, "placeholder");
  int slider_value = 12;
  auto slider = Slider("Value:", &slider_value, 0, 256, 1);
  auto layout = Container::Vertical({
      menu,
      input,
      slider,
  });
  auto component = Renderer(layout, [&] {
    return vbox({
               text("Gallery Example"),
               separator(),
               menu->Render(),
               input->Render(),
               slider->Render(),
           }) |
           border;
  });
  LoopWithQuit(screen, component);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_renderer() {
  auto screen = ScreenInteractive::FitComponent();
  auto renderer_focusable = Renderer([](bool focused) {
    if (focused)
      return text("FOCUSABLE RENDERER()") | center | bold | border;
    else
      return text(" Focusable renderer() ") | center | border;
  });
  auto renderer_non_focusable = Renderer([&] {
    return text("~~~~~ Non Focusable renderer() ~~~~~");
  });
  auto button = Button("Wrapped quit button", screen.ExitLoopClosure());
  auto renderer_wrap = Renderer(button, [&] {
    if (button->Focused())
      return button->Render() | bold | color(Color::Red);
    else
      return button->Render();
  });
  LoopWithQuit(screen, Container::Vertical({
      renderer_focusable,
      renderer_non_focusable,
      renderer_wrap,
  }));
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_linear_gradient_gallery() {
  auto screen = ScreenInteractive::Fullscreen();
  int angle = 180;
  float start = 0.f;
  float end = 1.f;
  std::string slider_angle_text;
  std::string slider_start_text;
  std::string slider_end_text;
  auto slider_angle = Slider(&slider_angle_text, &angle, 0, 360);
  auto slider_start = Slider(&slider_start_text, &start, 0.f, 1.f, 0.05f);
  auto slider_end = Slider(&slider_end_text, &end, 0.f, 1.f, 0.05f);
  auto layout = Container::Vertical({
      slider_angle,
      slider_start,
      slider_end,
  });
  auto renderer = Renderer(layout, [&] {
    slider_angle_text = "angle = " + std::to_string(angle) + "°";
    slider_start_text = "start = " + std::to_string(int(start * 100)) + "%";
    slider_end_text = "end   = " + std::to_string(int(end * 100)) + "%";
    auto background = text("Gradient") | center |
                      bgcolor(LinearGradient()
                                  .Angle(angle)
                                  .Stop(Color::Blue, start)
                                  .Stop(Color::Red, end));
    return vbox({
               background | flex,
               layout->Render(),
           }) |
           flex;
  });
  LoopWithQuit(screen, renderer);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

// ============================================================================
// DOM EXAMPLES - Styling & Colors
// ============================================================================

void example_border() {
  using namespace ftxui;
  auto document = hbox({
      vbox({
          text("Line 1"),
          text("Line 2"),
          text("Line 3"),
      }) | border,
      vbox({
          text("Line 4"),
          text("Line 5"),
          text("Line 6"),
      }) | border,
      vbox({
          text("Line 7"),
          text("Line 8"),
          text("Line 9"),
      }) | border,
  });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_border_colored() {
  using namespace ftxui;
  auto make_boxed = [] {
    return vbox({
        text("borderLight") | borderStyled(LIGHT, Color::Red),
        text("borderDashed") | borderStyled(DASHED, Color::Green),
        text("borderHeavy") | borderStyled(HEAVY, Color::Blue),
        text("borderDouble") | borderStyled(DOUBLE, Color::Yellow),
        text("borderRounded") | borderStyled(ROUNDED, Color::Cyan),
    });
  };
  auto document = hbox({
                      make_boxed(),
                      separator() | color(Color::Red),
                      make_boxed(),
                      separator() | color(Color::Red),
                      make_boxed(),
                  }) |
                  borderStyled(ROUNDED, Color::Red);
  auto screen =
      Screen::Create(Dimension::Fit(document), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_border_style() {
  using namespace ftxui;
  auto document = vbox({
      text("borderLight") | borderLight,
      text("borderDashed") | borderDashed,
      text("borderHeavy") | borderHeavy,
      text("borderDouble") | borderDouble,
      text("borderRounded") | borderRounded,
  });
  auto screen =
      Screen::Create(Dimension::Fit(document), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_color_gallery() {
  using namespace ftxui;
  auto basic_color_display = vbox({
      text("16 color palette:"),
      separator(),
      hbox({
          vbox({
              color(Color::Default, text("Default")),
              color(Color::Black, text("Black")),
              color(Color::GrayDark, text("GrayDark")),
              color(Color::GrayLight, text("GrayLight")),
              color(Color::White, text("White")),
              color(Color::Blue, text("Blue")),
              color(Color::BlueLight, text("BlueLight")),
              color(Color::Cyan, text("Cyan")),
              color(Color::CyanLight, text("CyanLight")),
              color(Color::Green, text("Green")),
              color(Color::GreenLight, text("GreenLight")),
              color(Color::Magenta, text("Magenta")),
              color(Color::MagentaLight, text("MagentaLight")),
              color(Color::Red, text("Red")),
              color(Color::RedLight, text("RedLight")),
              color(Color::Yellow, text("Yellow")),
              color(Color::YellowLight, text("YellowLight")),
          }),
          vbox({
              bgcolor(Color::Default, text("Default")),
              bgcolor(Color::Black, text("Black")),
              bgcolor(Color::GrayDark, text("GrayDark")),
              bgcolor(Color::GrayLight, text("GrayLight")),
              bgcolor(Color::White, text("White")),
              bgcolor(Color::Blue, text("Blue")),
              bgcolor(Color::BlueLight, text("BlueLight")),
              bgcolor(Color::Cyan, text("Cyan")),
              bgcolor(Color::CyanLight, text("CyanLight")),
              bgcolor(Color::Green, text("Green")),
              bgcolor(Color::GreenLight, text("GreenLight")),
              bgcolor(Color::Magenta, text("Magenta")),
              bgcolor(Color::MagentaLight, text("MagentaLight")),
              bgcolor(Color::Red, text("Red")),
              bgcolor(Color::RedLight, text("RedLight")),
              bgcolor(Color::Yellow, text("Yellow")),
              bgcolor(Color::YellowLight, text("YellowLight")),
          }),
      }),
  });
  auto document = basic_color_display;
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_color_info_palette256() {
  using namespace ftxui;
  auto document = text("Color palette example (simplified)");
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_color_truecolor_HSV() {
  using namespace ftxui;
  int saturation = 255;
  Elements array;
  for (int value = 0; value < 255; value += 20) {
    Elements line;
    for (int hue = 0; hue < 255; hue += 2) {
      line.push_back(text("▀") | color(Color::HSV(hue, saturation, value)) |
                     bgcolor(Color::HSV(hue, saturation, value + 10)));
    }
    array.push_back(hbox(std::move(line)));
  }
  auto document = vbox(std::move(array));
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_color_truecolor_RGB() {
  using namespace ftxui;
  Elements red_line, green_line, blue_line;
  for (int value = 0; value < 255; value += 3) {
    int v = value * value / 255;
    red_line.push_back(text(" ") | bgcolor(Color::RGB(v, 0, 0)));
    green_line.push_back(text(" ") | bgcolor(Color::RGB(0, v, 0)));
    blue_line.push_back(text(" ") | bgcolor(Color::RGB(0, 0, v)));
  }
  auto document = vbox({
      text("Primary colors"),
      hbox({text("Red line    :"), hbox(std::move(red_line))}),
      hbox({text("Green line  :"), hbox(std::move(green_line))}),
      hbox({text("Blue line   :"), hbox(std::move(blue_line))}),
  });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_bold() {
  using namespace ftxui;
  auto document = text("Bold text") | bold;
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_dim() {
  using namespace ftxui;
  auto document = text("Dim text") | dim;
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_italic() {
  using namespace ftxui;
  auto document = text("Italic text") | italic;
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_underlined() {
  using namespace ftxui;
  auto document = text("Underlined text") | underlined;
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_underlined_double() {
  using namespace ftxui;
  auto document = text("Double underlined text") | underlinedDouble;
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_strikethrough() {
  using namespace ftxui;
  auto document = text("Strikethrough text") | strikethrough;
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_inverted() {
  using namespace ftxui;
  auto document = text("Inverted text") | inverted;
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_blink() {
  using namespace ftxui;
  auto document = text("Blink text") | blink;
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_color() {
  using namespace ftxui;
  auto document = text("Colored text") | color(Color::Red);
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_gallery() {
  using namespace ftxui;
  auto document = vbox({
      text("Bold") | bold,
      text("Dim") | dim,
      text("Italic") | italic,
      text("Underlined") | underlined,
      text("Strikethrough") | strikethrough,
      text("Inverted") | inverted,
  });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_style_hyperlink() {
  using namespace ftxui;
  auto document = text("Hyperlink") | hyperlink("https://example.com");
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_linear_gradient() {
  using namespace ftxui;
  auto document = text("gradient") | center;
  document |= bgcolor(LinearGradient()
                          .Angle(45)
                          .Stop(Color::DeepPink1)
                          .Stop(Color::DeepSkyBlue1));
  auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

// ============================================================================
// DOM EXAMPLES - Layout Elements
// ============================================================================

void example_dbox() {
  using namespace ftxui;
  auto document = dbox({
      vbox({
          text("line_1"),
          text("line_2"),
          text("line_3"),
          text("line_4"),
          text("line_5"),
      }) | border,
      text("overlay") | border | center,
  });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_vbox_hbox() {
  using namespace ftxui;
  auto document = hbox({
      vbox({text("a"), text("b"), text("c")}),
      text(" "),
      vbox({text("d"), text("e"), text("f")}),
  });
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_vflow() {
  using namespace ftxui;
  auto document = vflow({
      text("text1"),
      text("text2"),
      text("text3"),
  });
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_hflow() {
  using namespace ftxui;
  auto make_box = [](int dimx, int dimy) {
    std::string title = std::to_string(dimx) + "x" + std::to_string(dimy);
    return window(text(title) | hcenter | bold,
                  text("content") | hcenter | dim) |
           size(WIDTH, EQUAL, dimx) | size(HEIGHT, EQUAL, dimy);
  };
  auto document = hflow({
      make_box(7, 7),
      make_box(7, 5),
      make_box(5, 7),
      make_box(10, 4),
      make_box(10, 4),
  });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_gridbox() {
  using namespace ftxui;
  auto cell = [](const char* t) { return text(t) | border; };
  auto document = gridbox({
      {cell("NW"), cell("N"), cell("NE")},
      {cell("W"), text("center"), cell("E")},
      {cell("SW"), cell("S"), cell("SE")},
  });
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_size() {
  using namespace ftxui;
  auto document = text("Sized element") | border | size(WIDTH, EQUAL, 20);
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_separator() {
  using namespace ftxui;
  auto document = vbox({
      text("Line 1"),
      separator(),
      text("Line 2"),
      separator(),
      text("Line 3"),
  });
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_separator_style() {
  using namespace ftxui;
  auto document = vbox({
      text("Line 1"),
      separator() | color(Color::Red),
      text("Line 2"),
      separator() | color(Color::Blue),
      text("Line 3"),
  });
  auto screen = Screen::Create(Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_paragraph() {
  using namespace ftxui;
  std::string p =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
  auto document = paragraph(p);
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

// ============================================================================
// DOM EXAMPLES - Visual Components
// ============================================================================

void example_canvas() {
  using namespace ftxui;
  auto c = Canvas(100, 100);
  c.DrawText(0, 0, "This is a canvas");
  c.DrawPointLine(10, 10, 80, 10, Color::Red);
  c.DrawPointLine(80, 10, 80, 40, Color::Blue);
  c.DrawPointLine(80, 40, 10, 10, Color::Green);
  auto document = canvas(&c) | border;
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_gauge() {
  using namespace ftxui;
  float percentage = 0.75f;
  auto document = hbox({
      text("downloading:"),
      gauge(percentage),
      text(" " + std::to_string(int(percentage * 100)) + "%"),
  });
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_gauge_direction() {
  using namespace ftxui;
  float percentage = 0.75f;
  auto document = vbox({
      gaugeUp(percentage),
      separator(),
      gaugeDown(percentage),
  });
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_graph() {
  using namespace ftxui;
  std::vector<int> data = {1, 2, 3, 4, 5, 4, 3, 2, 1};
  auto make_graph = [](const std::vector<int>& data) {
    return [data](int width, int height) { return data; };
  };
  auto document = graph(make_graph(data));
  ShowStaticDocumentUntilQuit(document, true);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_spinner() {
  using namespace ftxui;
  auto document = spinner(0, 0);
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_table() {
  using namespace ftxui;
  auto document = text("Table example (simplified)");
  ShowStaticDocumentUntilQuit(document);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_html_like() {
  using namespace ftxui;
  auto document = vbox({
      text("HTML-like example"),
      hflow({
          paragraph("This is a paragraph."),
          paragraph("Another paragraph."),
      }),
  });
  ShowStaticDocumentUntilQuit(document, true);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

void example_package_manager() {
  using namespace ftxui;
  auto document = text("Package manager example (simplified)");
  ShowStaticDocumentUntilQuit(document, true);
  ScreenInteractive::Active()->ExitLoopCleaner();
}

// ============================================================================
// EXAMPLE MENU DATA STRUCTURES
// ============================================================================

// Note: Example struct is defined in exemples.hpp
// Use the GetAllExamples() function below to get the registry

// Helper function to convert category enum to string for display
std::string category_to_string(ExampleCategory cat) {
  return (cat == ExampleCategory::COMPONENTS) ? "COMPONENTS" : "DOM";
}

std::vector<Example> GetAllExamples() {
  return {
      // COMPONENTS - Interactive Controls
      {"button_animated", example_button_animated, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"button", example_button, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"button_in_frame", example_button_in_frame, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"button_style", example_button_style, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"checkbox", example_checkbox, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"checkbox_in_frame", example_checkbox_in_frame, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"toggle", example_toggle, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"slider", example_slider, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"slider_direction", example_slider_direction, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"slider_rgb", example_slider_rgb, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"radiobox", example_radiobox, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"radiobox_in_frame", example_radiobox_in_frame, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"input", example_input, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"input_in_frame", example_input_in_frame, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"input_style", example_input_style, ExampleCategory::COMPONENTS, "Interactive Controls"},
      {"textarea", example_textarea, ExampleCategory::COMPONENTS, "Interactive Controls"},

      // COMPONENTS - Menus & Navigation
      {"menu", example_menu, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"menu2", example_menu2, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"menu_entries", example_menu_entries, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"menu_entries_animated", example_menu_entries_animated, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"menu_in_frame", example_menu_in_frame, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"menu_in_frame_horizontal", example_menu_in_frame_horizontal, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"menu_multiple", example_menu_multiple, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"menu_style", example_menu_style, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"menu_underline_animated_gallery", example_menu_underline_animated_gallery, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"dropdown", example_dropdown, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"dropdown_custom", example_dropdown_custom, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"selection", example_selection, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"tab_horizontal", example_tab_horizontal, ExampleCategory::COMPONENTS, "Menus & Navigation"},
      {"tab_vertical", example_tab_vertical, ExampleCategory::COMPONENTS, "Menus & Navigation"},

      // COMPONENTS - Layout
      {"composition", example_composition, ExampleCategory::COMPONENTS, "Layout"},
      {"resizable_split", example_resizable_split, ExampleCategory::COMPONENTS, "Layout"},
      {"flexbox_gallery", example_flexbox_gallery, ExampleCategory::COMPONENTS, "Layout"},
      {"nested_screen", example_nested_screen, ExampleCategory::COMPONENTS, "Layout"},

      // COMPONENTS - Visual & Dialogs
      {"canvas_animated", example_canvas_animated, ExampleCategory::COMPONENTS, "Visual & Dialogs"},
      {"scrollbar", example_scrollbar, ExampleCategory::COMPONENTS, "Visual & Dialogs"},
      {"collapsible", example_collapsible, ExampleCategory::COMPONENTS, "Visual & Dialogs"},
      {"modal_dialog", example_modal_dialog, ExampleCategory::COMPONENTS, "Visual & Dialogs"},
      {"modal_dialog_custom", example_modal_dialog_custom, ExampleCategory::COMPONENTS, "Visual & Dialogs"},
      {"window", example_window, ExampleCategory::COMPONENTS, "Visual & Dialogs"},
      {"focus", example_focus, ExampleCategory::COMPONENTS, "Visual & Dialogs"},
      {"focus_cursor", example_focus_cursor, ExampleCategory::COMPONENTS, "Visual & Dialogs"},
      {"maybe", example_maybe, ExampleCategory::COMPONENTS, "Visual & Dialogs"},

      // COMPONENTS - Advanced
      {"custom_loop", example_custom_loop, ExampleCategory::COMPONENTS, "Advanced"},
      {"print_key_press", example_print_key_press, ExampleCategory::COMPONENTS, "Advanced"},
      {"with_restored_io", example_with_restored_io, ExampleCategory::COMPONENTS, "Advanced"},
      {"homescreen", example_homescreen, ExampleCategory::COMPONENTS, "Advanced"},
      {"gallery", example_gallery, ExampleCategory::COMPONENTS, "Advanced"},
      {"renderer", example_renderer, ExampleCategory::COMPONENTS, "Advanced"},
      {"linear_gradient_gallery", example_linear_gradient_gallery, ExampleCategory::COMPONENTS, "Advanced"},

      // DOM - Styling & Colors
      {"border", example_border, ExampleCategory::DOM, "Styling & Colors"},
      {"border_colored", example_border_colored, ExampleCategory::DOM, "Styling & Colors"},
      {"border_style", example_border_style, ExampleCategory::DOM, "Styling & Colors"},
      {"color_gallery", example_color_gallery, ExampleCategory::DOM, "Styling & Colors"},
      {"color_info_palette256", example_color_info_palette256, ExampleCategory::DOM, "Styling & Colors"},
      {"color_truecolor_HSV", example_color_truecolor_HSV, ExampleCategory::DOM, "Styling & Colors"},
      {"color_truecolor_RGB", example_color_truecolor_RGB, ExampleCategory::DOM, "Styling & Colors"},
      {"style_bold", example_style_bold, ExampleCategory::DOM, "Styling & Colors"},
      {"style_dim", example_style_dim, ExampleCategory::DOM, "Styling & Colors"},
      {"style_italic", example_style_italic, ExampleCategory::DOM, "Styling & Colors"},
      {"style_underlined", example_style_underlined, ExampleCategory::DOM, "Styling & Colors"},
      {"style_underlined_double", example_style_underlined_double, ExampleCategory::DOM, "Styling & Colors"},
      {"style_strikethrough", example_style_strikethrough, ExampleCategory::DOM, "Styling & Colors"},
      {"style_inverted", example_style_inverted, ExampleCategory::DOM, "Styling & Colors"},
      {"style_blink", example_style_blink, ExampleCategory::DOM, "Styling & Colors"},
      {"style_color", example_style_color, ExampleCategory::DOM, "Styling & Colors"},
      {"style_gallery", example_style_gallery, ExampleCategory::DOM, "Styling & Colors"},
      {"style_hyperlink", example_style_hyperlink, ExampleCategory::DOM, "Styling & Colors"},
      {"linear_gradient", example_linear_gradient, ExampleCategory::DOM, "Styling & Colors"},

      // DOM - Layout Elements
      {"dbox", example_dbox, ExampleCategory::DOM, "Layout Elements"},
      {"vbox_hbox", example_vbox_hbox, ExampleCategory::DOM, "Layout Elements"},
      {"vflow", example_vflow, ExampleCategory::DOM, "Layout Elements"},
      {"hflow", example_hflow, ExampleCategory::DOM, "Layout Elements"},
      {"gridbox", example_gridbox, ExampleCategory::DOM, "Layout Elements"},
      {"size", example_size, ExampleCategory::DOM, "Layout Elements"},
      {"separator", example_separator, ExampleCategory::DOM, "Layout Elements"},
      {"separator_style", example_separator_style, ExampleCategory::DOM, "Layout Elements"},
      {"paragraph", example_paragraph, ExampleCategory::DOM, "Layout Elements"},

      // DOM - Visual Components
      {"canvas", example_canvas, ExampleCategory::DOM, "Visual Components"},
      {"gauge", example_gauge, ExampleCategory::DOM, "Visual Components"},
      {"gauge_direction", example_gauge_direction, ExampleCategory::DOM, "Visual Components"},
      {"graph", example_graph, ExampleCategory::DOM, "Visual Components"},
      {"spinner", example_spinner, ExampleCategory::DOM, "Visual Components"},
      {"table", example_table, ExampleCategory::DOM, "Visual Components"},
      {"html_like", example_html_like, ExampleCategory::DOM, "Visual Components"},
      {"package_manager", example_package_manager, ExampleCategory::DOM, "Visual Components"},
  };
}

// ============================================================================
// INTERACTIVE MENU IMPLEMENTATION
// ============================================================================

void run_examples_menu() {
  auto examples = GetAllExamples();

  // State
  int state = 0;  // 0: category, 1: subcategory, 2: example
  int category_selected = 0;
  int subcategory_selected = 0;
  int example_selected = 0;

  // Get unique categories (as enums)
  std::vector<ExampleCategory> category_enums;
  for (const auto& ex : examples) {
    if (std::find(category_enums.begin(), category_enums.end(), ex.category) ==
        category_enums.end()) {
      category_enums.push_back(ex.category);
    }
  }
  
  // Create display strings for categories
  std::vector<std::string> category_strings;
  for (const auto& cat : category_enums) {
    category_strings.push_back(category_to_string(cat));
  }

  // Prepare subcategories for all categories
  std::map<int, std::vector<std::string>> all_subcategories;
  for (size_t i = 0; i < category_enums.size(); ++i) {
    std::vector<std::string> subcategories;
    ExampleCategory cat = category_enums[i];
    for (const auto& ex : examples) {
      if (ex.category == cat) {
        if (std::find(subcategories.begin(), subcategories.end(),
                      ex.subcategory) == subcategories.end()) {
          subcategories.push_back(ex.subcategory);
        }
      }
    }
    all_subcategories[i] = subcategories;
  }

  while (true) {
    auto screen = ScreenInteractive::Fullscreen();

    std::vector<std::string> subcategory_strings;
    std::vector<std::string> example_strings;
    std::function<void()> selected_example_function;
    bool run_selected_example = false;

    auto category_menu = Menu(&category_strings, &category_selected);
    auto subcategory_menu = Menu(&subcategory_strings, &subcategory_selected);
    auto example_menu = Menu(&example_strings, &example_selected);

    auto refresh_examples = [&] {
      example_strings.clear();
      if (category_selected < 0 ||
          category_selected >= static_cast<int>(category_enums.size())) {
        example_selected = 0;
        return;
      }
      if (subcategory_selected < 0 ||
          subcategory_selected >= static_cast<int>(subcategory_strings.size())) {
        example_selected = 0;
        return;
      }

      ExampleCategory current_cat = category_enums[category_selected];
      const std::string& current_sub = subcategory_strings[subcategory_selected];
      for (const auto& ex : examples) {
        if (ex.category == current_cat && ex.subcategory == current_sub) {
          example_strings.push_back(ex.name);
        }
      }

      if (example_strings.empty() ||
          example_selected >= static_cast<int>(example_strings.size())) {
        example_selected = 0;
      }
    };

    auto refresh_subcategories = [&] {
      if (category_selected < 0 ||
          category_selected >= static_cast<int>(category_enums.size())) {
        category_selected = 0;
      }
      subcategory_strings = all_subcategories[category_selected];
      if (subcategory_strings.empty() ||
          subcategory_selected >= static_cast<int>(subcategory_strings.size())) {
        subcategory_selected = 0;
      }
      refresh_examples();
    };

    refresh_subcategories();

    auto tab = Container::Tab({category_menu, subcategory_menu, example_menu}, &state);

    auto component = Renderer(tab, [&] {
                       if (state == 0) {
                         return vbox({
                                    text("Select Category:") | bold,
                                    separator(),
                                    category_menu->Render(),
                                    separator(),
                                    text("Press Enter to select, Q to quit"),
                                }) |
                                border | center;
                       }

                       if (state == 1) {
                         return vbox({
                                    text("Select Subcategory:") | bold,
                                    separator(),
                                    subcategory_menu->Render(),
                                    separator(),
                                    text("Press Enter to select, Q to go back"),
                                }) |
                                border | center;
                       }

                       return vbox({
                                  text("Select Example:") | bold,
                                  separator(),
                                  example_menu->Render() | vscroll_indicator | frame |
                                      size(HEIGHT, LESS_THAN, 20),
                                  separator(),
                                  text("Press Enter to run, Q to go back"),
                              }) |
                              border | center;
                     }) |
                     CatchEvent([&](Event event) {
      if (event == Event::Character('q') || event == Event::Character('Q')) {
        if (state > 0) {
          state--;
          if (state == 0) {
            refresh_subcategories();
          } else if (state == 1) {
            refresh_examples();
          }
        } else {
          screen.Exit();
        }
        return true;
      }

      if (event == Event::Return) {
        if (state < 2) {
          if (state == 0) {
            state = 1;
            refresh_subcategories();
          } else {
            state = 2;
            refresh_examples();
          }
        } else {
          // Run selected example
          if (category_selected >= 0 &&
              category_selected < static_cast<int>(category_enums.size()) &&
              subcategory_selected >= 0 &&
              subcategory_selected < static_cast<int>(subcategory_strings.size()) &&
              example_selected >= 0 &&
              example_selected < static_cast<int>(example_strings.size())) {
            ExampleCategory current_cat = category_enums[category_selected];
            const std::string& current_sub =
                subcategory_strings[subcategory_selected];
            const std::string& current_example = example_strings[example_selected];

            for (const auto& ex : examples) {
              if (ex.category == current_cat && ex.subcategory == current_sub &&
                  ex.name == current_example) {
                run_selected_example = true;
                selected_example_function = ex.function;
                screen.Exit();
                return true;
              }
            }
          }
        }
        return true;
      }

      return false;
    });

    screen.Loop(component);

    if (!run_selected_example || !selected_example_function) {
      break;
    }

    selected_example_function();
  }
}

// Export main entry function
void run_ftxui_examples_menu() {
  run_examples_menu();
}
