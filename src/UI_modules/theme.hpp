#include <string>

#include <ftxui/component/component.hpp>

namespace theme {
    inline ftxui::Element incription_text(std::string text) { return ftxui::paragraph(text);};

    inline ftxui::Element automation_progression(std::string titre, float ratio) { return ftxui::vbox(ftxui::text(titre), ftxui::gaugeUp(ratio));};
}