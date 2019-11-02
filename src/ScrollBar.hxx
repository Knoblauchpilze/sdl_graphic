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
    ScrollBar::setPageStep(int step) {
      // Acquire the lock on this object.
      Guard guard(m_propsLocker);

      // Check whether the new value is different from the current value.
      int save = m_pageStep;

      m_pageStep = std::min(step, m_maximum - m_minimum);

      if (m_pageStep != save) {
        // Also request a repaint to indicate that the scroll bar should
        // be updated: indeed it probably means that the slider's size
        // needs to be updated.
        requestRepaint();
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
          // Fire the signal to indicate that the value has been changed.
          log("Emitting on value changed for " + getName(), utils::Level::Notice);
          onValueChanged.emit(getName(), m_value);

          // Also request a repaint to indicate that the scroll bar should
          // be updated: indeed it probably means that the slider should be
          // updated.
          requestRepaint();
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
      // Assume that the locker is already acquired.

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
      // Assume that the locker is already acquired.

      // We want the slider to be as large as a page step. This allows to accurately
      // represent the fact that the document is large (if the page step is small
      // compared to the range).
      // Once we have the size of the slider relatively to the range of the scrolling
      // area we should convert this into real world units knowing that the `total`
      // size in input is supposed to represent the whole scroll bar.
      // We know how to determine the size of the arrows so it's not too complicated
      // to work out the dimension of the area dedicated to the slider itself.

      // First, determine how many page steps we can fit in the total range of this
      // scroll bar. Exact computations are not actually required because we already
      // have some sort of clamping mechanism in the `setValue` method.
      int stepsCount = (m_pageStep == 0 ? 1 : (m_maximum - m_minimum) / m_pageStep);

      // Compute the available space to display these `stepCount` steps.
      utils::Sizef sliderArea = total;
      utils::Sizef arrow = getArrowSize(total);

      switch (m_orientation) {
        case Orientation::Horizontal:
          sliderArea.w() = total.w() - 2.0f * arrow.w();
          break;
        case Orientation::Vertical:
          sliderArea.h() = total.h() - 2.0f * arrow.h();
          break;
        default:
          error(
            std::string("Could not determine slider size for scroll bar"),
            std::string("Unknown orientation ") + std::to_string(static_cast<int>(m_orientation))
          );
          break;
      }

      // Now we can work out the real size of the slider compared to its logical size.
      // We know that `stepsCount` are represented using an area of `sliderArea` size.
      // We can simply deduce the size of the slider by assigning a single step size.
      utils::Sizef slider = sliderArea;

      switch (m_orientation) {
        case Orientation::Horizontal:
          slider.w() = sliderArea.w() / stepsCount;
          break;
        case Orientation::Vertical:
        default:
          // No need to check for errors as the previous switch already handled it.
          slider.h() = sliderArea.h() / stepsCount;
          break;
      }

      return slider;
    }

  }
}

#endif    /* SCROLL_BAR_HXX */
