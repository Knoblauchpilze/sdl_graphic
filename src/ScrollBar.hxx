#ifndef    SCROLL_BAR_HXX
# define   SCROLL_BAR_HXX

# include "ScrollBar.hh"
# include <core_utils/SafetyNet.hh>

namespace sdl {
  namespace graphic {

    inline
    ScrollBar::~ScrollBar() {
      const std::lock_guard guard(m_propsLocker);

      clearElements();
    }

    inline
    void
    ScrollBar::setMinimum(int minimum) {
      // Acquire the lock on this object.
      const std::lock_guard guard(m_propsLocker);

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

        // Request a repaint and a repaint of the internal controls.
        setElementsChanged();
        requestRepaint();
      }
    }

    inline
    void
    ScrollBar::setMaximum(int maximum) {
      // Acquire the lock on this object.
      const std::lock_guard guard(m_propsLocker);

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

        // Request a repaint and a repaint of the internal controls.
        setElementsChanged();
        requestRepaint();
      }
    }

    inline
    void
    ScrollBar::setPageStep(int step) {
      // Acquire the lock on this object.
      const std::lock_guard guard(m_propsLocker);

      // Check whether the new value is different from the current value.
      int save = m_pageStep;

      m_pageStep = std::min(step, m_maximum - m_minimum);

      if (m_pageStep != save) {
        // Also request a repaint to indicate that the scroll bar should
        // be updated: indeed it probably means that the slider's size
        // needs to be updated.
        setElementsChanged();
        requestRepaint();
      }
    }

    inline
    void
    ScrollBar::setRange(int minimum,
                        int step,
                        int maximum)
    {
      // Use the related method to assign each new value.
      setMinimum(minimum);
      setMaximum(maximum);
      setPageStep(step);
    }

    inline
    void
    ScrollBar::setValue(int value) {
      // Acquire the lock on this object.
      const std::lock_guard guard(m_propsLocker);

      // Use the dedicated handler.
      setValuePrivate(value);
    }

    inline
    void
    ScrollBar::updatePrivate(const utils::Boxf& /*window*/) {
      // Acquire the lock on this object.
      const std::lock_guard guard(m_propsLocker);

      // Invalidate the cache for internal elements.
      setElementsChanged();
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
    float
    ScrollBar::wheelStepToPageStepRatio() noexcept {
      return 2.0f;
    }

    inline
    int
    ScrollBar::getDoubleClickAdvance(int pageStep) noexcept {
      return std::min(pageStep / 2, 10);
    }

    inline
    float
    ScrollBar::getPercentageThreshold() noexcept {
      return 0.001f;
    }

    inline
    core::engine::mouse::Button
    ScrollBar::getScrollingButton() noexcept {
      return core::engine::mouse::Button::Left;
    }

    inline
    core::engine::Palette::ColorRole
    ScrollBar::getArrowColorRole(bool highlight) noexcept {
      switch (highlight) {
        case true:
          return core::engine::Palette::ColorRole::HighlightedText;
        default:
          // Assume default case is to not be highlighed.
          return core::engine::Palette::ColorRole::Dark;
      }
    }

    inline
    core::engine::Palette::ColorRole
    ScrollBar::getSliderColorRole(bool highlight) noexcept {
      switch (highlight) {
        case true:
          return core::engine::Palette::ColorRole::HighlightedText;
        default:
          // Assume default case is to not be highlighed.
          return core::engine::Palette::ColorRole::Mid;
      }
    }

    inline
    bool
    ScrollBar::setValuePrivate(int value,
                               bool notify)
    {
      // Assume that the locker is already acquired.
      bool update = false;
      int old = m_value;

      // Check whether the value is different from the current value.
      if (value != m_value) {
        // Assign the value and check whether it is consistent with the
        // admissible range for this scroll bar: we need the value not
        // to be smaller then the minimum and not to be greater than
        // that maximum minus the page step.
        m_value = std::min(m_maximum - m_pageStep, std::max(m_minimum, value));

        if (old != m_value) {
          update = true;

          // Fire the signal to indicate that the value has been changed
          // after converting to percentage.
          if (notify) {
            int iRange = m_maximum - m_minimum;
            float min = 1.0f * m_value / iRange;
            float max = 1.0f * (m_value + m_pageStep) / iRange;

            notice(
              "Emitting on value changed for " + getName() + " with range " +
              "[" + std::to_string(m_value) + ", " + std::to_string(m_value + m_pageStep) + "] " +
              "(" + std::to_string(min) + " - " + std::to_string(max) + ")"
            );

            onValueChanged.safeEmit(
              std::string("onValueChanged::emit(") + std::to_string(static_cast<int>(m_orientation)) +
              ", " + std::to_string(min) + std::to_string(max) + ")",
              m_orientation, min, max
            );
          }

          // Also request a repaint to indicate that the scroll bar should
          // be updated: indeed it probably means that the slider should be
          // updated.
          requestRepaint();
        }
      }

      return update;
    }

    inline
    void
    ScrollBar::clearElements() {
      // Clear any assigned texture.
      if (m_upArrow.id.valid()) {
        getEngine().destroyTexture(m_upArrow.id);
        m_upArrow.id.invalidate();
      }

      if (m_downArrow.id.valid()) {
        getEngine().destroyTexture(m_downArrow.id);
        m_downArrow.id.invalidate();
      }

      if (m_slider.id.valid()) {
        getEngine().destroyTexture(m_slider.id);
        m_slider.id.invalidate();
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
        case scroll::Orientation::Horizontal:
          arrow.w() = std::min(total.w(), maxArrowSize());
          break;
        case scroll::Orientation::Vertical:
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
      float stepsCount = (m_pageStep == 0 ? 1.0f : 1.0 * (m_maximum - m_minimum) / m_pageStep);

      // Compute the available space to display these `stepCount` steps.
      utils::Sizef sliderArea = total;
      utils::Sizef arrow = getArrowSize(total);

      switch (m_orientation) {
        case scroll::Orientation::Horizontal:
          sliderArea.w() = total.w() - 2.0f * arrow.w();
          break;
        case scroll::Orientation::Vertical:
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
        case scroll::Orientation::Horizontal:
          slider.w() = sliderArea.w() / stepsCount;
          break;
        case scroll::Orientation::Vertical:
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
