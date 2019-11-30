
# include "GradientWidget.hh"

namespace sdl {
  namespace graphic {

    GradientWidget::GradientWidget(const std::string& name,
                                   GradientShPtr gradient,
                                   core::SdlWidget* parent,
                                   const utils::Sizef& hint):
      core::SdlWidget(name, hint, parent),

      m_propsLocker(),

      m_gradient(gradient)
    {
      if (m_gradient == nullptr) {
        log(std::string("Gradient widget has null gradient"), utils::Level::Warning);
      }

      build();
    }

    void
    GradientWidget::drawContentPrivate(const utils::Uuid& /*uuid*/,
                                       const utils::Boxf& /*area*/)
    {
      // TODO: Render gradient.
    }

    void
    GradientWidget::build() {
      // No focus possible for this widget.
      setFocusPolicy(core::FocusPolicy());
    }

  }
}
