
# include "ScrollBar.hh"

namespace sdl {
  namespace graphic {

    ScrollBar::ScrollBar(const std::string& name,
                         const Orientation& orientation,
                         const core::engine::Color& color,
                         core::SdlWidget* parent,
                         const utils::Sizef& area):
      core::SdlWidget(name, area, parent, color),

      m_locker(),

      m_orientation(orientation),

      m_minimum(0),
      m_maximum(0),
      m_pageStep(0),

      m_value(0),


      onValueChanged()
    {
      // Build this component.
      build();
    }

    bool
    ScrollBar::mouseButtonPressEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseButtonPressEvent(e);
    }

    bool
    ScrollBar::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    bool
    ScrollBar::mouseDragEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseDragEvent(e);
    }

    bool
    ScrollBar::mouseWheelEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseWheelEvent(e);
    }

    void
    ScrollBar::build() {}

  }
}
