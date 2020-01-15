#ifndef    SLIDER_HXX
# define   SLIDER_HXX

# include "Slider.hh"
# include <sstream>
# include <iomanip>

namespace sdl {
  namespace graphic {

    inline
    Slider::~Slider() {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      clearSlider();
    }

    inline
    float
    Slider::getValue() {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      return getValueFromRangeData(m_data);
    }

    inline
    void
    Slider::updatePrivate(const utils::Boxf& window) {
      // Use the base handler.
      core::SdlWidget::updatePrivate(window);

      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      setSliderChanged();
    }

    inline
    float
    Slider::getValueFromRangeData(const RangeData& data) noexcept {
      // Detect invalid steps count. Note that as in case the slider
      // only has a single step, we should return the minimum bound
      // as well so we hanlde this here.
      if (data.steps < 2) {
        return data.range.x();
      }

      // Otherwise compute the percentage reached by the current step.
      // We consider that the step can range within `[0u; steps - 1]`.
      return data.range.x() + (data.range.y() - data.range.x()) * data.value / data.steps;
    }

    inline
    float
    Slider::getGlobalMargins() noexcept {
      return 5.0f;
    }

    inline
    const char*
    Slider::getValueLabelName() noexcept {
      return "slider_value_label";
    }

    inline
    float
    Slider::getValueLabelMaxWidth() noexcept {
      return 60.0f;
    }

    inline
    float
    Slider::getRulerLineHeight() noexcept {
      return 10.0f;
    }

    inline
    float
    Slider::getMobileAreaWidth() noexcept {
      return 30.0f;
    }

    inline
    int
    Slider::getDoubleClickAdvance(int pageStep) noexcept {
      return std::min(pageStep / 2, 10);
    }

    inline
    core::engine::mouse::Button
    Slider::getInteractionButton() noexcept {
      return core::engine::mouse::Button::Left;
    }

    inline
    float
    Slider::getStepRoundingThreshold() noexcept {
      return 0.01f;
    }

    inline
    int
    Slider::getStepFromValue(float value,
                             const utils::Vector2f& range,
                             int steps) noexcept
    {
      // Compute the number of steps represented by the input `value`.
      // We don't consider that being too far off a real step is an
      // issue in this method. It is up to the caller to determine if
      // this is the case.
      float stepsLen = (range.y() - range.x()) / steps;
      int stepVal = static_cast<int>(std::round((value - range.x()) / stepsLen));

      return std::min(std::max(0, stepVal), steps - 1);
    }

    inline
    std::string
    Slider::stringifyValue(float value,
                           unsigned decimals) noexcept
    {
      std::stringstream ss;
      ss << std::setprecision(decimals) << value;

      return ss.str();
    }

    inline
    LabelWidget*
    Slider::getValueLabel() {
      return getChildAs<LabelWidget>(getValueLabelName());
    }

    inline
    bool
    Slider::sliderChanged() const noexcept {
      return m_sliderChanged;
    }

    inline
    void
    Slider::setSliderChanged() {
      // Indicate that the slider's elements should be repainted and
      // trigger said repaint.
      m_sliderChanged = true;

      requestRepaint();
    }

    inline
    void
    Slider::clearSlider() {
      // Destroy both the ruler line and the mobile area textures.
      if (m_rulerLine.valid()) {
        getEngine().destroyTexture(m_rulerLine);
        m_rulerLine.invalidate();
      }

      if (m_mobileArea.valid()) {
        getEngine().destroyTexture(m_mobileArea);
        m_mobileArea.invalidate();
      }
    }

    inline
    bool
    Slider::performAction(const Action& action,
                          int value,
                          bool notify)
    {
      // Assume that the locker is already acquired.

      // The input action describes the motion to apply to the mobile area.
      // Depending on the range of the slider and the page step we need to
      // adapt the corresponding modification.
      // A second task of this method is to update the position of the visual
      // representation of the mobile area so that it is consistent with the
      // actual value it is meant to represent.
      // For example if the mobile area is at the minimum possible value it
      // should be rendered on the far left of the available area.

      // Handle trivial case of `NoAction`.
      if (action == Action::NoAction) {
        // Consider that `doing nothing` is never successful.
        return false;
      }

      // First thing is to compute the target value pointed to by the slider.
      int targetValue = m_data.value;

      switch (action) {
        case Action::SingleStepAdd:
          ++targetValue;
          break;
        case Action::SingleStepSub:
          --targetValue;
          break;
        case Action::PageStepAdd:
          targetValue = m_data.value + m_data.pageStep;
          break;
        case Action::PageStepSub:
          targetValue = m_data.value - m_data.pageStep;
          break;
        case Action::ToMinimum:
          targetValue = 0;
          break;
        case Action::ToMaximum:
          targetValue = m_data.steps - 1;
          break;
        case Action::Move:
          targetValue = value;
          break;
        default:
          // Note that this does not include the `NoAction` as it has been handled
          // beforehand.
          error(
            std::string("Cannot perform requested action in scroll bar"),
            std::string("Unknown action value ") + std::to_string(static_cast<int>(action))
          );
          break;
      }

      // Apply this value.
      bool changed = setValuePrivate(targetValue, notify);

      // If the value could not be modified, nothing is left to do.
      if (!changed) {
        return false;
      }

      // The internal value has been changed: we need to update the position of the slider
      // based on the value represented by this scroll bar.
      updateSliderPosFromValue();

      // The position of the slider has been updated.
      return true;
    }

    bool
    Slider::setValuePrivate(int value,
                            bool notify)
    {
      // Assume that the locker is already acquired.
      bool update = false;
      int old = m_data.value;

      // Check whether the value is different from the current value.
      if (value != old) {
        // Assign the value and check whether it is consistent with the
        // admissible range for this scroll bar: we need the value not
        // to be smaller then the minimum and not to be greater than
        // that maximum minus the page step.
        m_data.value = std::min(m_data.steps - 1, std::max(0, value));

        if (old != m_data.value) {
          update = true;

          // Fire the signal to indicate that the value has been changed
          // after converting to percentage.
          if (notify) {
            float value = getValueFromRangeData(m_data);

            // Update the label's text.
            LabelWidget* label = getValueLabel();
            if (label != nullptr) {
              label->setText(stringifyValue(value, m_decimals));
            }

            log(
              "Emitting on value changed for " + getName() + " with range " +
              m_data.range.toString() + ", steps: " + std::to_string(m_data.steps) +
              " (current: " + std::to_string(m_data.value) + ", page: " + std::to_string(m_data.pageStep) + ")" +
              " value: " + std::to_string(value),
              utils::Level::Verbose
            );

            onValueChanged.safeEmit(
              std::string("onValueChanged(") + std::to_string(value) + ")",
              value
            );
          }

          // Also request a repaint to indicate that the slider should
          // be updated: indeed it probably means that the mobile area
          // should be updated.
          requestRepaint();
        }
      }

      return update;
    }

  }
}

#endif    /* SLIDER_HXX */
