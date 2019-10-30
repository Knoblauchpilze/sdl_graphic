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

      private:

        /**
         * @brief - A mutex allowing to protect this object from concurrent accesses.
         *          Should be used to modify or read the values of most of the internal
         *          properties.
         */
        mutable std::mutex m_locker;

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
