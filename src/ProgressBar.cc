
# include "ProgressBar.hh"

namespace sdl {
  namespace graphic {

    ProgressBar::ProgressBar(const std::string& name,
                             core::SdlWidget* parent,
                             const core::engine::Color& color,
                             const utils::Sizef& hint):
      sdl::core::SdlWidget(name, hint, parent, color),

      m_propsLocker()
    {
      build();
    }

    void
    ProgressBar::build() {}

  }
}
