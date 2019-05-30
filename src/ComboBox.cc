
# include "ComboBox.hh"

namespace sdl {
  namespace graphic {

    ComboBox::ComboBox(const std::string& name,
                       SdlWidget* parent,
                       const core::engine::Color& color,
                       const utils::Sizef& area):
      core::SdlWidget(name, area, parent, color)
    {}

    ComboBox::~ComboBox() {}

  }
}