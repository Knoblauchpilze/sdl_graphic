#ifndef    GRADIENT_HXX
# define   GRADIENT_HXX

# include "Gradient.hh"

namespace sdl {
  namespace graphic {

    inline
    gradient::Mode
    Gradient::getMode() const noexcept {
      Guard guard(m_propsLocker);

      return m_mode;
    }

    inline
    gradient::Stops
    Gradient::getStops() const noexcept {
      Guard guard(m_propsLocker);

      return m_stops;
    }

    inline
    float
    Gradient::getGradientStepThreshold() noexcept {
      return 0.00001f;
    }

    inline
    bool
    Gradient::isBeforeStop(float coord,
                           float stop,
                           bool& replace) const noexcept
    {
      // The coordinate is before the stop if it is smaller than it or
      // if it lies within a threshold distance from the stop.
      replace = std::fabs(coord - stop) < getGradientStepThreshold();
      return coord < stop || replace;
    }

  }
}

#endif    /* GRADIENT_HXX */
