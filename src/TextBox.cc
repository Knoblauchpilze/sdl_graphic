
# include "TextBox.hh"

namespace sdl {
  namespace graphic {

    TextBox::TextBox(const std::string& name,
                     SdlWidget* parent,
                     const utils::Sizef& area):
      core::SdlWidget(name, area, parent, core::engine::Color::NamedColor::White)
    {}

    TextBox::~TextBox() {}

  }
}
