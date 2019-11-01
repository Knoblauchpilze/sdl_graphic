#ifndef    SCROLL_BAR_HXX
# define   SCROLL_BAR_HXX

# include "ScrollBar.hh"

namespace sdl {
  namespace graphic {

    inline
    ScrollBar::~ScrollBar() {
      Guard guard(m_propsLocker);

      clearElements();
    }

    inline
    void
    ScrollBar::setMinimum(int minimum) {
      // Acquire the lock on this object.
      Guard guard(m_propsLocker);

      // Check whether the new value is different from the current value.
      if (minimum != m_minimum) {
        // Assign the value and check whether it is consistent with the
        // rest of the internal properties.
        m_minimum = minimum;

        if (m_maximum < m_minimum) {
          m_minimum = m_maximum;
        }

        m_pageStep = std::min(m_maximum - m_maximum, m_pageStep);

        if (m_value < m_minimum) {
          setValuePrivate(m_minimum);
        }
      }
    }

    inline
    void
    ScrollBar::setMaximum(int maximum) {
      // Acquire the lock on this object.
      Guard guard(m_propsLocker);

      // Check whether the new value is different from the current value.
      if (maximum != m_maximum) {
        // Assign the value and check whether it is consistent with the
        // rest of the internal properties.
        m_maximum = maximum;

        if (m_maximum < m_minimum) {
          m_maximum = m_minimum;
        }

        m_pageStep = std::min(m_maximum - m_maximum, m_pageStep);

        if (m_value > m_maximum) {
          setValuePrivate(m_maximum);
        }
      }
    }

    inline
    void
    ScrollBar::setValue(int value) {
      // Acquire the lock on this object.
      Guard guard(m_propsLocker);

      // Use the dedicated handler.
      setValuePrivate(value);
    }

    inline
    float
    ScrollBar::maxDimsAlongSlider() noexcept {
      return 100.0f;
    }

    inline
    float
    ScrollBar::minArrowSize() noexcept {
      return 20.0f;
    }

    inline
    float
    ScrollBar::maxArrowSize() noexcept {
      return 30.0f;
    }

    inline
    void
    ScrollBar::setValuePrivate(int value) {
      // Assume that the locker is already acquired.
      int old = m_value;

      // Check whether the value is different from the current value.
      if (value != m_value) {
        // Assign the value and check whether it is consistent with the
        // admissible range for this scroll bar.
        m_value = std::min(m_maximum, std::max(m_minimum, value));

        if (old != m_value) {
          log("Emitting on value changed for " + getName(), utils::Level::Notice);
          onValueChanged.emit(getName(), m_value);
        }
      }
    }

    inline
    void
    ScrollBar::clearElements() {
      // Clear any assigned texture.
      if (m_upArrowTex.valid()) {
        getEngine().destroyTexture(m_upArrowTex);
        m_upArrowTex.invalidate();
      }

      if (m_downArrowTex.valid()) {
        getEngine().destroyTexture(m_downArrowTex);
        m_downArrowTex.invalidate();
      }

      if (m_sliderTex.valid()) {
        getEngine().destroyTexture(m_sliderTex);
        m_sliderTex.invalidate();
      }
    }

    inline
    bool
    ScrollBar::elementsChanged() const noexcept {
      return m_elementsChanged;
    }

    inline
    void
    ScrollBar::setElementsChanged() noexcept {
      m_elementsChanged = true;
    }

    inline
    utils::Sizef
    ScrollBar::getArrowSize(const utils::Sizef& total) const {
      // The arrow size is dependent on the orientation. Right now some hard-coded values
      // prevent it from being too big which is fine for our purposes.
      // Note that we only consider the maximum arrow size here: indeed we can't do much
      // if the size of the scroll bar does not allow for minimum size to be respected.
      // We can't really add a scroll bar to display the scroll bar itself can we ?
      utils::Sizef arrow = total;

      switch (m_orientation) {
        case Orientation::Horizontal:
          arrow.w() = std::min(total.w(), maxArrowSize());
          break;
        case Orientation::Vertical:
          arrow.h() = std::min(total.h(), maxArrowSize());
          break;
        default:
          error(
            std::string("Could not determine arrow size for scroll bar"),
            std::string("Unknown orientation ") + std::to_string(static_cast<int>(m_orientation))
          );
          break;
      }

      return arrow;
    }

    inline
    utils::Sizef
    ScrollBar::getSliderSize(const utils::Sizef& total) const {
      // TODO: Implementation
      utils::Sizef slider = total;
      utils::Sizef arrow = getArrowSize(total);

      switch (m_orientation) {
        case Orientation::Horizontal:
          slider.w() = total.w() - 2.0f * arrow.w();
          break;
        case Orientation::Vertical:
          slider.h() = total.h() - 2.0f * arrow.h();
          break;
        default:
          error(
            std::string("Could not determine slider size for scroll bar"),
            std::string("Unknown orientation ") + std::to_string(static_cast<int>(m_orientation))
          );
          break;
      }

      return slider;
    }

  }
}

#endif    /* SCROLL_BAR_HXX */
