
# include "GradientWidget.hh"

namespace sdl {
  namespace graphic {

    GradientWidget::GradientWidget(const std::string& name,
                                   core::engine::GradientShPtr gradient,
                                   core::SdlWidget* parent,
                                   const utils::Sizef& hint):
      core::SdlWidget(name, hint, parent, core::engine::Color::NamedColor::Olive),

      m_propsLocker(),

      m_gradient(gradient),

      m_gradientChanged(true),
      m_gradientTex()
    {
      if (m_gradient == nullptr) {
        log(std::string("Gradient widget has null gradient"), utils::Level::Warning);
      }

      build();
    }

    void
    GradientWidget::drawContentPrivate(const utils::Uuid& uuid,
                                       const utils::Boxf& area)
    {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Recreate the gradient's texture if needed.
      if (gradientTexChanged()) {
        loadGradientTex();
        m_gradientChanged = false;
      }

      // Return early in case the texture is invalid (meaning that the
      // associated gradient is probably null).
      if (!m_gradientTex.valid()) {
        return;
      }

      // Render the texture so that it takes up all the available space.
      utils::Boxf thisArea = LayoutItem::getRenderingArea().toOrigin();
      utils::Sizef gradArea = getEngine().queryTexture(m_gradientTex);
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);

      utils::Boxf dst = thisArea.intersect(area);
      utils::Boxf src = thisArea.intersect(area);

      if (!dst.valid()) {
        // Nothing to draw.
        return;
      }

      utils::Boxf dstEngine = convertToEngineFormat(dst, sizeEnv);
      utils::Boxf srcEngine = convertToEngineFormat(src, gradArea);

      getEngine().drawTexture(m_gradientTex, &srcEngine, &uuid, &dstEngine);
    }

    void
    GradientWidget::build() {
      // No focus possible for this widget.
      setFocusPolicy(core::FocusPolicy());
    }

  }
}
