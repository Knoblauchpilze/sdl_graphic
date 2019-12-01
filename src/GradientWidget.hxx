#ifndef    GRADIENT_WIDGET_HXX
# define   GRADIENT_WIDGET_HXX

# include "GradientWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    GradientWidget::~GradientWidget() {
      Guard guard(m_propsLocker);

      clearGradientTex();
    }

    inline
    void
    GradientWidget::setGradient(core::engine::GradientShPtr gradient) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Assign the new gradient and request a repaint operation.
      m_gradient = gradient;
      requestRepaint();
    }

    inline
    void
    GradientWidget::clearGradientTex() {
      if (m_gradientTex.valid()) {
        getEngine().destroyTexture(m_gradientTex);
        m_gradientTex.invalidate();
      }
    }

    inline
    void
    GradientWidget::loadGradientTex() {
      // Clear existing label if any.
      clearGradientTex();

      // Create the gradient texture if needed.
      if (m_gradient != nullptr) {
        // Use a brush to paint the texture representing the gradient.
        // To do so we will sample the gradient at various percentages
        // and report the corresponding color in the brush's canvas.
        utils::Sizef area = LayoutItem::getRenderingArea().toSize();
        core::engine::BrushShPtr brush = std::make_shared<core::engine::Brush>("grad_brush", area);
        brush->drawGradient(*m_gradient);

        m_gradientTex = getEngine().createTextureFromBrush(brush);

        if (!m_gradientTex.valid()) {
          error(
            std::string("Could not create visual data for gradient"),
            std::string("Unable to create texture for \"") + m_gradient->getName() + "\""
          );
        }

        getEngine().fillTexture(m_gradientTex, getPalette());
      }
    }

    inline
    bool
    GradientWidget::gradientTexChanged() const noexcept {
      return m_gradientChanged;
    }

    inline
    void
    GradientWidget::setGradientTexChanged() noexcept {
      // The gradient texture is now dirty.
      m_gradientChanged = true;

      // Request a repaint.
      requestRepaint();
    }

  }
}

#endif    /* GRADIENT_WIDGET_HXX */
