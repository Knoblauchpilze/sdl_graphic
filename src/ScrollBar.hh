#ifndef    SCROLL_BAR_HH
# define   SCROLL_BAR_HH

# include <mutex>
# include <memory>
# include <sdl_core/SdlWidget.hh>
# include <core_utils/Signal.hh>
# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    class ScrollBar: public core::SdlWidget {
      public:

        /**
         * @brief - Describes the possible orientation of the scroll bar which
         *          allows to produce both horizontal and vertical layouts.
         */
        enum class Orientation {
          Horizontal, //<! - Scroll bar is horizontal.
          Vertical    //<! - Scroll bar is vertical.
        };

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
                  const Orientation& orientation,
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
         * @brief - Reimplementation of the base `EngineObject` method in order to provide
         *          custom behavior when the user press the direction keys. Based on the
         *          orientation of the scroll bar some direction keys will allow to move
         *          the slider of the scroll bar of one line.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        keyReleaseEvent(const core::engine::KeyEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` class to detect when the
         *          scroll bar should be displayed as selected.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseButtonPressEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` class to detect when the
         *          scroll bar should be released from the selection.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` class to detect when the
         *          scroll bar should be dragged and its value updated. The drag event is
         *          responsible of the motion of the scroll bar and is usually linked to the
         *          main component which is `controlled` by this bar.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseDragEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` class to detect when the
         *          wheel is used: this should trigger the scroll bar behavior to change the
         *          active value.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseWheelEvent(const core::engine::MouseEvent& e) override;

      private:

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
         */
        void
        setValuePrivate(int value);

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
        Orientation m_orientation;

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
         * @brief - The identifier of the texture representing the up arrow for this scroll bar. The
         *          up arrow is only a logical term and does not mean that the texture actually does
         *          represent an arrow.
         *          This texture is allocated and drawn as long as the scroll bar does not change its
         *          size. We have a minimum value to have in order to be able to draw this texture.
         */
        utils::Uuid m_upArrowTex;

        /**
         * @brief - Similar to `m_upArrowTex` except for the bottom arrow. Adding the minimum size
         *          required for these textures will give the minimum size to provide for the scroll
         *          bar.
         */
        utils::Uuid m_downArrowTex;

        /**
         * @brief - Represents the slider to use to modify the value controlled by the scroll bar.
         *          This slider has a size consistent with both the range of the scroll bar and
         *          its size.
         */
        utils::Uuid m_sliderTex;

      public:

        /**
         * @brief - This signal can be used for external objects to register whenever the value of
         *          this scrollbar is modified. The listener can react on modifications of the
         *          scroll bar's pointed value.
         */
        utils::Signal<const std::string&, int> onValueChanged;
    };

    using ScrollBarShPtr = std::shared_ptr<ScrollBar>;
  }
}

# include "ScrollBar.hxx"

#endif    /* SCROLL_BAR_HH */
