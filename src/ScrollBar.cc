
# include "ScrollBar.hh"

namespace sdl {
  namespace graphic {

    ScrollBar::ScrollBar(const std::string& name,
                         const Orientation& orientation,
                         const core::engine::Color& color,
                         core::SdlWidget* parent,
                         const utils::Sizef& area):
      core::SdlWidget(name, area, parent, color),

      m_orientation(orientation)
    {
      // Build this component.
      build();
    }

    ScrollBar::~ScrollBar() {}

    void
    ScrollBar::build() {}

  }
}
