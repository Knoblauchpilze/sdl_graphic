#ifndef    SLIDER_HH
# define   SLIDER_HH

# include <memory>
# include <sdl_core/SdlWidget.hh>
# include "VirtualLayoutItem.hh"
# include "LabelWidget.hh"
# include <core_utils/Signal.hh>

namespace sdl {
  namespace graphic {

    class Slider: public core::SdlWidget {
      public:

        /**
         * @brief - Creates a new slider witht he specified range and value.
         * @param name - the name of `this` slider.
         * @param value - the initial value of the slider.
         * @param range - the range of the slider. Note that in case the `value`
         *                is not consistent with the range it is clamped to the
         *                closest bound.
         * @param steps - the number of steps to create between both bounds. If
         *                the value does not lie exactly on such a step it is
         *                rounded to be on a whole step.
         * @param font - the name of the font to use to render the current
         *               value selected.
         * @param size - the size of the font to set for the label indicating
         *               the currently selected value.
         * @param parent - a pointer to the parent widget for this textbox.
         * @param area - the size hint for this text box.
         */
        Slider(const std::string& name,
               float value,
               const utils::Vector2f& range,
               int steps,
               const std::string& font,
               unsigned size = 15,
               SdlWidget* parent = nullptr,
               const utils::Sizef& area = utils::Sizef());

        virtual ~Slider();

      protected:

        /**
         * @brief - Reimplementation of the base class method to notify slider's elements
         *          that the size of the widget has changed and thus that the elements are
         *          to be repainted (because their appearance has most likely changed).
         * @param window - the available size to perform the update.
         */
        void
        updatePrivate(const utils::Boxf& window) override;

        /**
         * @brief - Reimplementation of the base `SdlWidget` method. Several textures
         *          are used to represent the slider and we should update them upon
         *          calling this method so that it is always up-to-date with the value
         *          selected. Only the specified part is updated by this function.
         * @param uuid - the identifier of the canvas which we can use to draw the
         *               overlay.
         * @param area - the area of the canvas to update.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid,
                           const utils::Boxf& area) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` method to provide
         *          specific behavior upon detecting a key event.
         *          We want to make the slider move when some specified key is
         *          pressed.
         * @param e - the key event which should be handled.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        keyPressEvent(const core::engine::KeyEvent& e) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` in order to provide
         *          custom behavior when the user clicks on the slider. We want to
         *          move the cursor one step in the direction of the click (i.e. if
         *          the mouse is between the slider and the end of the range it will
         *          be moved forward and backward otherwise.
         * @param e - the event to be handled.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` method to provide
         *          specific behavior when the user double click on the slider. We
         *          will use a similar behavior to what happen on a click but will
         *          send the slider all the way to the bound.
         * @param e - the double click event which should be handled.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseDoubleClickEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Specialization of the base `EngineObject` method to provide
         *          custom behavior upon dragging the mouse. We will handle the
         *          progressive displacement of the slider to follow the mouse.
         * @param e - the drag event to process.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseDragEvent(const core::engine::MouseEvent& e) override;

      private:

        /**
         * @brief - Convenience structure allowing to group properties rleated to the
         *          range of this slider. It describes both the bounds and the step 
         *          (i.e. the minimum increment of the slider) and the current value.
         */
        struct RangeData {
          utils::Vector2f range;
          int steps;
          int pageStep;

          int value;

          utils::Boxf maBox;
          utils::Boxf box;
        };

        /**
         * @brief - Describes the possible action to be performed by a slider. Each
         *          action corresponds to some sort of motion in an abstract way.
         *          It is up to the user who instantiated the slider to translate
         *          it into meaningful operation: depending on whether the slider is
         *          supposed to make a text move, an image, etc. the action will be
         *          interpreted differently.
         */
        enum class Action {
          NoAction,
          SingleStepAdd,
          SingleStepSub,
          PageStepAdd,
          PageStepSub,
          ToMinimum,
          ToMaximum,
          Move
        };

        /**
         * @brief - Used to retrieve the actual value defined by the provided range data.
         *          This basically uses the boundaries and the current step to convert to
         *          a real world value.
         *          Note that in case the `steps` is invalid (i.e. set to `0`) the value
         *          returned is the minimum bound.
         * @param data - the range data to interpret.
         * @return - a value in the range defined by the data and corresponding to the
         *           current number of steps.
         */
        static
        float
        getValueFromRangeData(const RangeData& data) noexcept;

        /**
         * @brief - Used as a default value to configure the internal layout to get nice
         *          margins around the component.
         * @return - a value representing the default margins to assign to this slider.
         */
        static
        float
        getGlobalMargins() noexcept;

        /**
         * @brief - Used to retrieve a default name to assign to the label widget holding
         *          the current value of the slider. Using the name provided by this method
         *          allows for consistent naming all across the component.
         * @return - a name suited to represent the label holding the current value of the
         *           slider.
         */
        static
        const char*
        getValueLabelName() noexcept;

        /**
         * @brief - Used to provide a suitable maximum width for the value label. Indeed it
         *          should not benefit from growing too much as its main goal is only to be
         *          large enough to display the current value of the slider. It usually don't
         *          require a lot of horizontal space.
         * @return - a avlue to use to assign a maximum size to the value label.
         */
        static
        float
        getValueLabelMaxWidth() noexcept;

        /**
         * @brief - Used to retrieve a suited height for the ruler line running across the
         *          slider. It is supposed to be wide enough so that it adds a nice benefit
         *          to the slider's representation but slim enough so that it does not hide
         *          the mobile area to use to change the slider's value.
         * @return - a suited height for the ruler line.
         */
        static
        float
        getRulerLineHeight() noexcept;

        /**
         * @brief - Used to retrieve a suited width for the mobile area allowing to change
         *          the slider's value. The width should not be too slim so that it is easy
         *          for the user to select it and move it around, but not too large neither
         *          so that it is clear what is the current value compared to the possible
         *          range.
         * @return - a suited width for this slider's mobile area.
         */
        static
        float
        getMobileAreaWidth() noexcept;

        /**
         * @brief - Defines the advancement to perform when double clicking on the slider.
         *          This value is meant as an intermediate between the single step and the
         *          page step motion.
         * @param pageStep - the page step currently associated to the slider.
         * @return - a value describing an intermediate value between the page step in
         *           argument and the single step add.
         */
        static
        int
        getDoubleClickAdvance(int pageStep) noexcept;

        /**
         * @brief - Retrieves the button to use to trigger interactions with the slider. This
         *          button is checked for any mouse related events (such as mouse drag or a
         *          mouse button clicked).
         *          Using any other button will not allow to perform interactions with this
         *          slider.
         * @return - a default button for any interaction with the slider.
         */
        static
        core::engine::mouse::Button
        getInteractionButton() noexcept;

        /**
         * @brief - Used to define a threshold that is applied when the user provides a value
         *          to detect when it's too far off one of the step to notify that there's
         *          probably an issue with the value.
         * @return - a threshold used to detect rounding errors when assigning the value of
         *           the slider from a steps count.
         */
        static
        float
        getStepRoundingThreshold() noexcept;

        /**
         * @brief - Used to compute the step's index from a value given the input range. THis
         *          can be used to convert a real world value into internal semantic that uses
         *          steps to measure the progression of the slider.
         * @param value - the value to convert (should be in the input range).
         * @param range - the bounds of the value.
         * @param steps - the number of steps the range is divided into.
         * @return - the step count of the input value.
         */
        static
        int
        getStepFromValue(float value,
                         const utils::Vector2f& range,
                         int steps) noexcept;

        /**
         * @brief - Used internally upon constructing the slider to initialize internal
         *          states.
         * @param font - the font to use when creating the label displaying the current
         *               slider's value.
         * @param size - the size of the font to use.
         */
        void
        build(const std::string& font,
              unsigned size);

        /**
         * @brief - Used to retrieve the label widget displaying the current slider's value.
         *          This method can be used in case the label should be fetched to update its
         *          value for example.
         *          This method is merely a wrapper around the `getChildAs` method and is
         *          guaranteed to return a valid value if it returns.
         * @return - the label widget holding the current slider's value.
         */
        LabelWidget*
        getValueLabel();

        /**
         * @brief - Describes whether the slider's elements should be repainted. It usually
         *          occurs upon resizeing this slider: otherwise we make do with moving the
         *          elements around.
         *          Note that the internal mutex is assumed to already be acquired.
         * @return - `true` if the `m_rulerLine` and `m_mobileArea` textures should not be
         *           used as is and repainted instead.
         */
        bool
        sliderChanged() const noexcept;

        /**
         * @brief - Indicate that the internal textures representing slider's elements are
         *          to be repainted upon the next call to `drawContentPrivate`. It does not
         *          repaint the textures in any case though.
         *          Note that the locker should already be assumed before using this method.
         */
        void
        setSliderChanged();

        /**
         * @brief - Used to perform the loading of the individual textures for slider's elems.
         *          Note that no checks are performed to verify whether it is actually needed.
         *          Note that the locker is assumed to already be acquired.
         */
        void
        loadSlider();

        /**
         * @brief - Clear the textures representing the slider's elements, i.e. the `m_rulerLine`
         *          and the `m_mobileArea` representing respectively the horizontal line ruling
         *          the slider and the rectangle mobile area that can be used to change the value.
         */
        void
        clearSlider();

        /**
         * @brief - Used to update the mobile area's position based on the value currently
         *          held by the slider. Basically when the value is set to the minimum of
         *          the range the mobile area should be on the far left side of the slider
         *          and when the value gets closer to the maximum it should reach the far
         *          right side of the widget.
         *          In between the mobile area's position should be somwhere in the available
         *          region provided by the slider. This method allows to easily set it to
         *          what it should be.
         *          Note that it does not trigger a repaint event and assumes that the
         *          locker protecting this object is already acquired.
         */
        void
        updateSliderPosFromValue();

        /**
         * @brief - Reverse method of `updateSliderPosFromValue`. Given the input local
         *          position (i.e. expressed in this widget's coordinate frame) this
         *          method determines the equivalent value of the sliderwhich would lead
         *          to a mobile area with this position.
         *          Note that the output value might be out of the admissible range for
         *          this slider.
         *          Note that it does not trigger a repaint event and assumes that the
         *          locker protecting this object is already acquired.
         * @param local - the desired position of the mobile area.
         * @return - the value corresponding to a mobile area position as defined in input.
         *           Note that the return value is set in number of steps and not in real
         *           world value.
         */
        int
        getValueFromSliderPos(const utils::Vector2f& local) const;

        /**
         * @brief - Used to try to perform the requested action and update the value of
         *          the slider within the admissible range. Note that the internal
         *          conditions will be considered in order to only execute the action
         *          if the consistency while doing so is preserved. If the action can
         *          be at least partially performed the return value will be `true`.
         *          The value in argument is only used when the action is set to `Move`.
         *          It represents in this case the new desired value which might not be
         *          representable using a single action like `Step add` or `To minimum`.
         *          Otherwise this value is ignored.
         *          Note that this function assumes that the locker protecting the slider
         *          from concurrent accesses is already locked.
         * @param action - the action to perform.
         * @param value - the value to assign in the case of a `Move` action.
         * @param notify - transmitted as is to the `setValuePrivate` method in case it
         *                 needs to be called, allows to specify whether listeners should
         *                 be notified of this change.
         * @return - `true` if the input `action` could at least be partially performed
         *           and `false` otherwise.
         */
        bool
        performAction(const Action& action,
                      int value = 0,
                      bool notify = true);

        /**
         * @brief - Used to assign a new value to the current step of this slider.
         *          Note that this value is checked to be consistent with the range
         *          defined by the object.
         *          If the value is different from the current one a `valueChanged`
         *          signal is emitted.
         *          Note that this method assumes that the locker protecting this
         *          object from concurrent accesses is already locked.
         * @param value - the value to be assigned to this scroll bar's step. It
         *                will be checked against the valid range for this slider.
         * @param notify - `true` if the listeners should be notified of this change
         *                 and `false` otherwise.
         * @return - `true` if the internal value has been changed and `false` otherwise.
         *           Note that a `true` value does not mean that the `m_value` will be
         *           exactly `value` but that it is different from the initial value
         *           upon entering this method.
         */
        bool
        setValuePrivate(int value,
                        bool notify = true);

      private:

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this slider.
         */
        std::mutex m_propsLocker;

        /**
         * @brief - Holds all the relevant data to describe the range for this slider. It
         *          is updated whenever an action changes the value of the slider and is
         *          controlled as to not allow inconsistent values.
         */
        RangeData m_data;

        /**
         * @brief - Describes whether the slider's representation has changed or can be used
         *          as is during `ðrawContentPrivate` operation.
         */
        bool m_sliderChanged;

        /**
         * @brief - A virtual layout item which allows to position the slider's visual data
         *          (i.e. the horizontal and vertical area which the user can use to change
         *          the value of the slider) as if it was a regular item.
         *          This will help managing its size and also offset the `text` by a suited
         *          margin to fit the selection box.
         */
        VirtualLayoutItemShPtr m_sliderItem;

        /**
         * @brief - An identifier holding the texture created by the engine to represent a
         *          horizontal line on the active part of the slider which allows to get a
         *          feeling of alignment for the slider.
         *          The rectangle indicating the current value of the slider will be set to
         *          cross this horizontal line so as to get the feeling that it is attached
         *          to it.
         *          It is valid unless the `m_sliderChanged` value is set to `false` which
         *          usually indicates a resize operation.
         */
        utils::Uuid m_rulerLine;

        /**
         * @brief - An identifier holding the texture created to rerpesent the element which
         *          can be dragged by the user to change the value of the slider. This is a
         *          mobile part of the slider and its position should reflect some sort of
         *          indication of the current value compared to the total range.
         *          Note that this element can only occupy discrete positions on the ruler
         *          line, each position corresponding to a precise step.
         */
        utils::Uuid m_mobileArea;

      public:

        /**
         * @brief - This signal is used to notify a change in the value of the slider. It
         *          is provided as an argument to the slot.
         */
        utils::Signal<float> onValueChanged;
    };

    using SliderShPtr = std::shared_ptr<Slider>;
  }
}

# include "Slider.hxx"

#endif    /* SLIDER_HH */
