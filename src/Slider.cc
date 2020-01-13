
# include "Slider.hh"

namespace sdl {
  namespace graphic {

    Slider::Slider(const std::string& name,
                   float /*value*/,
                   const utils::Vector2f& /*range*/,
                   int /*steps*/,
                   const std::string& /*font*/,
                   unsigned /*size*/,
                   SdlWidget* parent,
                   const utils::Sizef& area):
      core::SdlWidget(name,
                      area,
                      parent,
                      sdl::core::engine::Color::NamedColor::Purple),

      m_propsLocker()
    {
      build();
    }

    bool
    Slider::keyPressEvent(const core::engine::KeyEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::keyPressEvent(e);
    }

    bool
    Slider::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    bool
    Slider::mouseDoubleClickEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseDoubleClickEvent(e);
    }

    bool
    Slider::mouseDragEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseDragEvent(e);
    }

    void
    Slider::drawContentPrivate(const utils::Uuid& /*uuid*/,
                               const utils::Boxf& /*area*/)
    {
      // TODO: Implementation.
    }

    void
    Slider::build() {
      // TODO: Implementation.
    }

  }
}
