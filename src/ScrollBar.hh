#ifndef    SCROLL_BAR_HH
# define   SCROLL_BAR_HH

# include <mutex>
# include <memory>
# include <sdl_core/SdlWidget.hh>
# include <core_utils/Signal.hh>
# include "LinearLayout.hh"
# include "ScrollOrientation.hh"

namespace sdl {
  namespace graphic {

    class ScrollBar: public core::SdlWidget {
      public:

      public:

        /**
         * @brief - Creates a scroll bar with the specified name and parent.
         *          The user should also specify the orientation of the scroll
         *          bar.
         * @param name - the name of this scroll bar.
         * @param orientation - the orientation of the scroll bar.
         * @param color - the background color for this scroll bar.
         * @param parent - the parent element to which this widget is attached.
         * @param area - the preferred size of this scroll bar.
         */
        ScrollBar(const std::string& name,
                  const scroll::Orientation& orientation,
                  const core::engine::Color& color,
                  core::SdlWidget* parent = nullptr,
                  const utils::Sizef& area = utils::Sizef());

        virtual ~ScrollBar();

        /**
         * @brief - Used to assign a new minimum possible value for this scroll bar.
         *          Note that if this value is larger than the current maximum it
         *          will be clamped to be equal to the maximum and the current value
         *          will be updated accordingly.
         *          Using this method and changing the current  value will result in
         *          the signal `valueChanged` to be fired.
         * @param minimum - the new minimum value for the range of this scroll bar.
         */
        void
        setMinimum(int minimum);

        /**
         * @brief - Similar function to `setMinimum` but assigns the maximum value
         *          possible for this scroll bar. Note that if this value is smaller
         *          than the current minimum it will be clamped to be equal to the
         *          minimum and the current value will be modified accordingly.
         *          Using this method and changing the current value will result in
         *          the signal `valueChanged` to be fired.
         * @param maximum - the new maximum value for the range of this scroll bar.
         */
        void
        setMaximum(int maximum);

        /**
         * @brief - Defines a new value for the page step for this scroll bar. The page
         *          step describes the number of meaningful increment for scrolling in
         *          the document linked to the scroll bar. The size of the page step is
         *          relative to the total range and the smaller the value the smaller
         *          the slider will appear in the scroll bar. This value is clamped in
         *          order not to exceed the available range for the document.
         * @param step - the new value for the page step.
         */
        void
        setPageStep(int step);

        /**
         * @brief - Used to assign the whole range for this scroll bar all at once. This
         *          is basically a wrapper using the `setMinimum` and such handlers. It
         *          does not do anything more.
         *          Note that the values are assigned in the following order: minimum,
         *          maximum and then page step.
         * @param minimum - the new minimum value for the range of this scroll bar.
         * @param step - the new value for the page step.
         * @param maximum - the new maximum value for the range of this scroll bar.
         */
        void
        setRange(int minimum,
                 int step,
                 int maximum);

        /**
         * @brief - Used to assign the value currently represented in the scroll bar from
         *          a percentage-based range. Each bound is interpreted in terms of the
         *          values provided internally for the range.
         * @param min - the minimum value visible for this scroll bar as a percentage of
         *              the internal minimum value.
         * @param max - the maximum value visible for this scroll bar as a percentage of
         *              the internal maximum value.
         */
        void
        setFromPercentage(float min,
                          float max);

        /**
         * @brief - Used to assign a new value for the step of this scroll bar. This
         *          method will call the `setValuePrivate` method internally and trigger
         *          a signal `valueChanged` if the value is effectively different from
         *          the currently registered one.
         *          The value will be checked against the admissible range for this
         *          scroll bar in order to maintain consistency.
         * @param value - the new value to add to the step of this scroll bar.
         */
        void
        setValue(int value);

      protected:

        /**
         * @brief - Reimplementation of the base class method to invalidate the elements
         *          whenever this widget gets redrawn.
         * @param window - the available size to perform the update.
         */
        void
        updatePrivate(const utils::Boxf& window) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method in order to
         *          provide drawing primitives for the textures used to represent the
         *          scroll bar: this includes both motion arrows and the slider itself.
         *          Note that as for most reimplementation we will try to reuse existing
         *          textures as much as possible using a `dirty` mechanism which is
         *          triggered when the size of this scroll area is changed or when the
         *          position of the slider is modified.
         *          Note that as for the base implementation the area is expressed in
         *          LOCAL coordinate frame (so no conversion is required to use it).
         * @param uuid - the identifier of the canvas which we can use to draw the scroll
         *               bar representation.
         * @param area - the area of the canvas to update.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid,
                           const utils::Boxf& area) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method in order to detect
         *          when the focus is lost. This allows to reset the highlight of any of the
         *          element composing the scroll bar to their default state.
         * @param state - the current internal state which we will use to detect complete
         *                loss of focus.
         * @param gainedFocus - `true` if this method was triggered from a gain focus event
         *                      and `false` otherwise.
         */
        void
        stateUpdatedFromFocus(const core::FocusState& state,
                              bool gainedFocus) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` method in order to provide
         *          custom behavior when the user press the direction keys. Based on the
         *          orientation of the scroll bar some direction keys will allow to move
         *          the slider of the scroll bar of one line.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        keyPressEvent(const core::engine::KeyEvent& e) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` method in order to scroll
         *          the slider to reach the position of the mouse at the moment of the
         *          click in order to provide easy navigation.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseButtonPressEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method to detect when the
         *          scroll bar should be released from the selection.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` method in order to provide a
         *          similar behavior on double click as on mouse button release event. This
         *          makes the navigation much easier as it will move the cursor with a larger
         *          value than a single step but still not much compared to the page step.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseDoubleClickEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method to detect when the
         *          scroll bar should be dragged and its value updated. The drag event is
         *          responsible of the motion of the scroll bar and is usually linked to the
         *          main component which is `controlled` by this bar.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseDragEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method to detect when the
         *          mouse overs over on of the component of this scroll bar (like the motion
         *          arrows or the slider) which will allows to make the hovered over element
         *          to stand out compared to the others.
         * @param e - the evenv to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseMoveEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method to detect when the
         *          wheel is used: this should trigger the scroll bar behavior to change the
         *          active value.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseWheelEvent(const core::engine::MouseEvent& e) override;

      private:

        /**
         * @brief - Describes the possible action to be performed by a scroll bar.
         *          Each action corresponds to some sort of motion in an abstract
         *          way. It is up to the user who instantiated the scroll bar to
         *          translate it into meaningful operation: depending on whether
         *          the scroll bar is supposed to make a text move, an image, etc.
         *          the action will be interpreted differently.
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
         * @brief - Returns the maximum size of the scroll bar along the slider dimensions.
         *          This value is used to assign it as a maximum size for the scroll bar's
         *          dimensions in order not to make it grow past a certain size.
         *          Depending on the orientation of the scroll bar this value is applied on
         *          the width of height of the bar.
         * @return - a value describing the maximum size along the slider dimensions.
         */
        static
        float
        maxDimsAlongSlider() noexcept;

        /**
         * @brief - Returns the minimum size for an arrow used to move the slider along the
         *          scroll bar's axis. This size is used in order to keep the area with a
         *          reasonable size which allow to easily use it.
         * @return - a value describing the minimum size of the arrow along the slider axis.
         */
        static
        float
        minArrowSize() noexcept;

        /**
         * @brief - Returns the maximum size for an arrow used to move the slider along the
         *          scroll bar's axis. This size is used in order to not make the arrows too
         *          big compared to the available space which allows to keep more area for
         *          the slider.
         * @return - a value describing the maximum size of the arrow along the slider axis.
         */
        static
        float
        maxArrowSize() noexcept;

        /**
         * @brief - Defines the ratio of page step performed by a single wheel step. Basically
         *          it indicates how many page steps are advanced when the mouse wheel is rolled
         *          on a scroll bar.
         * @return - a value describing the wheel step to page step ratio.
         */
        static
        float
        wheelStepToPageStepRatio() noexcept;

        /**
         * @brief - Defines the advancement to perform when double clicking on the control arrows.
         *          This value is meant as an intermediate between the single step and the page
         *          step motion.
         * @param pageStep - the page step currently associated to the scroll bar.
         * @return - a value describing an intermediate value between the page step in argument
         *           and the single step add.
         */
        static
        int
        getDoubleClickAdvance(int pageStep) noexcept;

        /**
         * @brief - Defines the threshold above which the a percentage of visibility (typically one
         *          received in the `setFromPercentage` method) is not considered to be exact when
         *          compared to the internal `m_pageStep`.
         *          This allows to enforce some sort of consistency between both values.
         * @return - a value indicating from which point a range received when assigning the value
         *           from a percentage is assumed to be different from the internal range.
         */
        static
        float
        getPercentageThreshold() noexcept;

        /**
         * @brief - Used to retrieve the button to use to trigger the elements representing the
         *          scroll bar (arrows and sldier).
         * @return - the default button to use to perform the scrolling operations.
         */
        static
        core::engine::mouse::Button
        getScrollingButton() noexcept;

        /**
         * @brief - Used to determine the color role to use for motion arrows depending on
         *          whether the user hovers over them or not. This allows to visually tell
         *          the user that an action is possible with these arrows
         * @param highlight - `true` if the color should be retrieved for the case where the
         *                    user hovers over the arrow and `false` otherwise.
         * @return - a color role representing the role assumed by a motion arrow.
         */
        static
        core::engine::Palette::ColorRole
        getArrowColorRole(bool highlight) noexcept;

        /**
         * @brief - Similar method as `getArrowColorRole` but to determine the color assumed
         *          by the slider object.
         * @param highlight - `true` if the color should be retrieved for the case where the
         *                    user hovers over the slider and `false` otherwise.
         * @return - a color role representing the role assumed by the slider.
         */
        static
        core::engine::Palette::ColorRole
        getSliderColorRole(bool highlight) noexcept;

        /**
         * @brief - Called by the constructor which allows to create the components
         *          needed by this scrollbar.
         */
        void
        build();

        /**
         * @brief - Used to assign a new value to the current step of this scroll bar.
         *          Note that this value is checked to be consistent with the range
         *          defined by this scroll bar.
         *          If the value is different from the current one a `valueChanged`
         *          signal is emitted.
         *          Note that this method assumes that the locker protecting this object
         *          from concurrent accesses is already locked.
         * @param value - the value to be assigned to this scroll bar's step. It will
         *                be checked against the valid range for this scroll bar.
         * @param notify - `true` if the listeners should be notified of this change and
         *                 `false` otherwise.
         * @return - `true` if the internal value has been changed and `false` otherwise.
         *           Note that a `true` value does not mean that the `m_value` will be
         *           exactly `value` but that it is different from the initial value
         *           upon entering this method.
         */
        bool
        setValuePrivate(int value,
                        bool notify = true);

        /**
         * @brief - Used to try to performt he requested action and update the value of
         *          the scroll bar within the admissible range. Note that the internal
         *          conditions will be considered in order to only execute the action
         *          if the consistency while doing so is preserved. If the action can
         *          be at least partially performed the return value will be `true`.
         *          The value in argument is only used when the action is set to `Move`.
         *          It represents in this case the new desired value which might not be
         *          representable using a single action like `Step add` or `To minimum`.
         *          Otherwise this value is ignored.
         *          Note that this function assumes that the locker protecting the scroll
         *          bar from concurrent accesses is already locked.
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
         * @brief - Used to update the slider's position based on the value currently
         *          pointed at by the scroll bar. Basically when the value is set to the
         *          minimum of the scroll bar the slider should be close to the up arrow
         *          while it should be close to the down arrow when the value gets closer
         *          to the maximum.
         *          In between the slider's position should be somwhere in the available
         *          region provided by the scroll bar. This method allows to easily set
         *          it to what it should be.
         *          Note that it does not trigger a repaint event and assumes that the
         *          locker protecting this object is already acquired.
         */
        void
        updateSliderPosFromValue();

        /**
         * @brief - Reverse method of `updateSliderPosFromValue`. Given the input local
         *          position (i.e. expressed in this widget's coordinate frame) this
         *          method determines the equivalent value of the scroll bar which would
         *          lead to a slider with this position.
         *          Note that the output value might be out of the admissible range for
         *          this scroll bar.
         *          Note that it does not trigger a repaint event and assumes that the
         *          locker protecting this object is already acquired.
         * @param local - the desired position of the slider.
         * @return - the value corresponding to a slider position as defined in input.
         */
        int
        getValueFromSliderPos(const utils::Vector2f& local) const;

        /**
         * @brief - Used to perform the update of the elements' roles considering that
         *          the mouse is actually located at the `local` position. As its name
         *          suggests this value is expected to be in local coordinate frame.
         *          The user can determine whether any change were made using the out
         *          value. No repaint event is issued by this method.
         *          Note that this method assumes the that the locker protecting from
         *          concurrency is already locked.
         * @param local - the position at which the mouse is located.
         * @return - `true` if any role were updated and `false` otherwise.
         */
        bool
        updateElementsRolesFromMousePos(const utils::Vector2f& local);

        /**
         * @brief - Used to create the textures allowing to represent the scroll bar
         *          components, namely the two arrows allowing to scroll and the slider
         *          which indicates the current position of the scroll bar in the
         *          range.
         *          Note that this method does not check whether it is actually needed
         *          to recreate the textures, this operation should be performed before
         *          calling it.
         *          Any existing texture will be cleared. Also note that this method
         *          assumes that the locker has already been acquired.
         */
        void
        loadElements();

        /**
         * @brief - Used to perform a fill operation on the textures representing the element
         *          for this scroll bar. The fill operation allows to initialize the data of
         *          the texture and potentially update it based on a color role change.
         *          The fill operation is usually triggered by the fact that the internal data
         *          for an element request it but the user can force it using the input bool
         *          if needed.
         * @param force - `true` if the elements should be filled no matter what and `false`
         *                if we should use the internal data to handle each case.
         */
        void
        fillElements(bool force = false);

        /**
         * @brief - Destroys the textures describing the elements representing the scroll
         *          bar and invalidate the corresponding identifiers.
         *          Should typically be used when recreating the elements after the size
         *          of the scroll bar has been changed or any geometry modification did
         *          impact the way these elements should look.
         *          Note that this method assumes that the locker has already been acquired.
         */
        void
        clearElements();

        /**
         * @brief - Used to determine whether any of the elements composing this scroll bar
         *          has been modified since the last `drawContentPrivate` operation. It uses
         *          internally the `m_elementsChanged` status to perform the check.
         *          Note that this method assumes that the locker has already been acquired.
         * @return - `true` if at least one of the elements properties have been modified and
         *           `false` otherwise.
         */
        bool
        elementsChanged() const noexcept;

        /**
         * @brief - Marks this widget for a rebuild of the text upon calling the `drawContentPrivate`
         *          method. Assumes that the content locker is already acquired.
         *          Note that this method assumes that the locker has already been acquired.
         */
        void
        setElementsChanged() noexcept;

        /**
         * @brief - Used to determine the size of the arrows to represent this scroll bar.
         *          Basically the arrows will grow up to a maximum size and then not change
         *          anymore. The input `total` area represents the total size available for
         *          this scroll bar.
         * @param total - an area representing the total size available for the scroll area.
         * @return - an area representing the size of the arrow regardless of whether it is
         *           the top or bottom arrow.
         */
        utils::Sizef
        getArrowSize(const utils::Sizef& total) const;

        /**
         * @brief - Similar method to `getArrowSize` but to compute the size of the slider
         *          associated to this scroll bar. Note that the slider's size is function
         *          of both the size of the scroll bar represented by the `total` argument
         *          but also of the range available compared to the page step.
         * @param total - an area representing the total size available for the scroll area.
         * @return - an area representing the size to assign to the slider of this scroll
         *           area.
         */
        utils::Sizef
        getSliderSize(const utils::Sizef& total) const;

      private:

        /**
         * @brief - Used to hold rendering properties for any element representing the scroll
         *          bar. This allows to conveniently group data in a meaningful way.
         */
        struct ElementDesc {
          utils::Uuid id;                        //<! - The identifier of the texture to use to
                                                 //     represent this element.
          utils::Boxf box;                       //<! - The box to use to position this element.
          core::engine::Palette::ColorRole role; //<! - The color role attached to this element.
          bool roleUpdated;                      //<! - `true` when the role has been changed and
                                                 //     requires an update of the texture.
        };

        /**
         * @brief - A mutex allowing to protect this object from concurrent accesses.
         *          Should be used to modify or read the values of most of the internal
         *          properties.
         */
        mutable std::mutex m_propsLocker;

        /**
         * @brief - Contains the orientation of the scroll bar. Allows to specify
         *          both main orientation (horizontal or vertical) to know how the
         *          components of the scroll bar should be represented.
         */
        scroll::Orientation m_orientation;

        /**
         * @brief - Describes the minimum value possible for this scroll bar. The value
         *          is expressed as an integer to be able to provide whole steps to move
         *          `up` or `down` along the slider.
         *          The value is not really important per say, it should correspond to
         *          some meaningful value for the one using the scroll bar.
         *          Note that no `valueChanged` event can be emitted with a value lower
         *          than this one.
         */
        int m_minimum;

        /**
         * @brief - Similar to `m_minimum` but holds the maximum value possible for the
         *          slider. Note that no `valueChanged` event can be emitted with a value
         *          larger than this one.
         */
        int m_maximum;

        /**
         * @brief - Holds the number of single step performed by a single page step operation.
         *          The page step action is usually linked to pressing the `Page Down` or `Up`
         *          key and is usually linked to a larger advance of the slider along its range.
         *          The value of this step is clamped so that it is not larger than the total
         *          range of the scroll bar.
         *          In case moving of an entire page step would bring the value of the slider
         *          to be larger than the maximum or smaller than the minimum it will be changed
         *          so that the maximum possible value is used instead.
         */
        int m_pageStep;

        /**
         * @brief - The current value of the slider in the range defined by the two internal values
         *          `[m_minimum; m_maximum]`. This value cannot exceed the maximum nor be smaller
         *          than the minimum.
         *          This value can either be advanced by single steps or by page steps depending on
         *          the control used to modify the slider's value.
         */
        int m_value;

        /**
         * @brief - Used to determine whether the elements used to render this scroll bar have been
         *          modified and should be repainted. When this value is `false` it indicates that
         *          even in the occurrence of a repaint event the elements constituting the scroll
         *          bar have not been modified and can be reused.
         */
        bool m_elementsChanged;

        /**
         * @brief - The structure regrouping the rendering properties of the up arrow. Describes the
         *          identifier of the texture used to represent it, its position and the role of the
         *          texture (i.e. a loose indication of the color that will be used to draw it.
         *          Using such a structure rather than individual data allows to reduce the number of
         *          attributes declared for the scroll bar class and to group data in a meaningful
         *          way.
         */
        ElementDesc m_upArrow;

        /**
         * @brief - Similar to `m_upArrow` but contains information about the slider element used to
         *          navigate in the document attached to the scroll bar.
         */
        ElementDesc m_slider;

        /**
         * @brief - Similar attribute as `m_downArrowBox` but holds the information related to the down
         *          arrow.
         */
        ElementDesc m_downArrow;

      public:

        /**
         * @brief - This signal can be used for external objects to register whenever the value of
         *          this scrollbar is modified. The listener can react on modifications of the
         *          scroll bar's pointed value.
         *          Note that the signal includes the name of the scroll bar emitting the signal
         *          along with the minimum value visible and the maximum visible value. The minimum
         *          and maximum are usually separated by a page step and are expressed in percentage
         *          of the total range of possible values.
         *          The range of possible value is thus `[0; 100]`.
         */
        utils::Signal<scroll::Orientation, float, float> onValueChanged;
    };

    using ScrollBarShPtr = std::shared_ptr<ScrollBar>;
  }
}

# include "ScrollBar.hxx"

#endif    /* SCROLL_BAR_HH */
