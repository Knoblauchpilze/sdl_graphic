#ifndef    GRADIENT_WIDGET_HXX
# define   GRADIENT_WIDGET_HXX

# include "GradientWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    GradientWidget::~GradientWidget() {}

    inline
    void
    GradientWidget::setGradient(GradientShPtr gradient) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Assign the new gradient and request a repaint operation.
      m_gradient = gradient;
      requestRepaint();
    }

  }
}

#endif    /* GRADIENT_WIDGET_HXX */
