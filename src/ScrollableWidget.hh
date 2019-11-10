#ifndef    SCROLLABLE_WIDGET_HH
# define   SCROLLABLE_WIDGET_HH

# include <mutex>
# include <memory>
# include <sdl_core/SdlWidget.hh>
# include <maths_utils/Vector2.hh>

namespace sdl {
  namespace graphic {

    class ScrollableWidget: public core::SdlWidget {
      public:

        /**
         * @brief - Creates a scrollable widget with the specified name and
         *          area. Such a widget allows to easily manipulate the ways
         *          to communicate that a widget should be repainted at its
         *          location based on some scroll behavior.
         *          The user should assign the widget to scroll through the
         *          public `setScrollableWidget` method.
         * @param name - the name of this scrollable widget.
         * @param parent - the parent element to which this widget is attached.
         * @param area - the preferred size of this scrollable widget.
         */
        ScrollableWidget(const std::string& name,
                         core::SdlWidget* parent = nullptr,
                         const utils::Sizef& area = utils::Sizef());

        virtual ~ScrollableWidget();

        /**
         * @brief - Used to update the content wrapped by this scrollable object.
         *          This method is used to assign a new widget that this object
         *          will operate on. A `null` value means that no widget is being
         *          scrolled and is valid.
         * @param widget - the new widget to scroll onto.
         */
        void
        setSupport(core::SdlWidget* widget);

        /**
         * @brief - Used to retrieve the preferred size of this scrollable widget.
         *          The goal is to provide an indication of the size that this
         *          widget could use if it was possible: it is directly linked to
         *          the expected size of the underlying support widget and allows
         *          to get an idea of how big the widget should be if it was not
         *          constrained to fit in this element.
         *          Note that inheriting classes are encouraged to rather specialize
         *          the `getPreferredSizePrivate` method as it is used internally
         *          by this method.
         * @return - the size that this widget would occupy should it have all the
         *           space it needs.
         */
        utils::Sizef
        getPreferredSize() const noexcept;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` in order to filter
         *          the returned widget if it corresponds to the support widget but
         *          outside of its assigned area.
         *          Indeed it might happen that the support widget is assigned a larger
         *          size that actually displayed. While the standard process uses this
         *          as a valid information to retrieve the item at the position we don't
         *          want this here.
         *          More generally the interaction on the support widget should be reduced
         *          so that this widget is able to control the way the support widget is
         *          displayed. So in the case the best item corresponds to the support
         *          widget we want to substitute this widget in lieu of the support widget.
         *          This will prevent the regular events system to assign some events to
         *          the support widget and rather send them to this widget.
         * @param pos - a vector describing the position which should be spanned by the items.
         * @return - a valid pointer if any of the children items spans the input position and
         *           `null` otherwise.
         */
        const core::SdlWidget*
        getItemAt(const utils::Vector2f& pos) const noexcept override;

      protected:

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method in order
         *          to provide a custom implementation for the size update process.
         *          We actually want to transmit the information to the underlying
         *          support widget so that it can update itself if needed.
         * @param window - the available size to perform the update.
         */
        void
        updatePrivate(const utils::Boxf& window) override;

        /**
         * @brief - Actual implementation of the `getPreferredSize` method. This is
         *          mainly useful because it assumes that the locker protecting this
         *          object is already locked so it can be used by internal methods.
         *          Fills the same role as described by the `getPreferredSize` method.
         *          Note that as discussed the locker is assumed to be acquired.
         * @return - the size that this widget would occupy should it have all the
         *           space it needs.
         */
        virtual utils::Sizef
        getPreferredSizePrivate() const noexcept;

        /**
         * @brief - This method is called right after a new support widget has been
         *          defined through the `setSupport` method and is meant to perform
         *          some additional initializations before this widget is fully used
         *          as the support area for this scroll area.
         *          Typical definition include assigning a size based on the widget's
         *          requirements in order to get a valid display anyway. The position
         *          can also be set to either be centered on the widget or be on the
         *          top left corner, or whatever.
         *          Entering this method guarantees that the input `widget` is not
         *          `null`. Also the input `widget` is already a child of this widget.
         * @param widget - the new support widget to use.
         */
        virtual void
        setupSupport(core::SdlWidget* widget);

        /**
         * @brief - Performs the scrolling of the widget displayed in this component
         *          to the desired position. Basically this method will be requested
         *          to do the necessary modifications in order to bring the `posToFix`
         *          expressed in local coordinate frame so that it coincide with the
         *          `whereTo` position (expressed in local coordinate frame as well)
         *          visually.
         *          Basically in the usual case where the content widget is larger
         *          than the size of this scrollable widget it means that we should
         *          make some adjustment so that the visible part of the displayed
         *          widget still includes the `posToFix` and displays it exactly at
         *          the `whereTo` position.
         *          Note that this method is meant to be an interface which can be
         *          specialized in inheriting classes. For example if we want to
         *          use this component for an infinite widget it might be a good
         *          idea to define a specific behavior for this method which would
         *          generate the needed data in case some new parts of the content
         *          widget is exposed.
         *          The return value indicates whether some changes have been made
         *          to the content of the widget. If this is the case the caller
         *          should issue a repaint event (usually) as it is *not* part of
         *          the responsability of this method. In case no visible changes
         *          have been made to the widget the return value should be set to
         *          `false`.
         *          Note that this method assumes that the locker for this widget
         *          is already acquired.
         * @param posToFix - a position expressed in local coordinate frame which
         *                   corresponds to the old position to be fixed. This
         *                   position should never be outside of the range of the
         *                   content widget.
         * @param whereTo - the new position also expressed in local coordinate
         *                  frame which represents where the old `posToFix` should
         *                  now be displayed.
         *                  Note that it might not be possible to display the
         *                  `posToFix` at this specified position and in this case
         *                  it is up to the implementation to know what to do (one
         *                  could generate more data, or forbid the requested move
         *                  for example).
         * @param motion - the current motion applied which triggered the call to
         *                 this content scrolling action. This motion corresponds
         *                 to the actual change in `whereTo` position since the
         *                 last call to this method.
         * @return - `true` if some modifications where made to the structure of
         *            the widget (usually a modification of the position of the
         *            content widget) and `false` otherwise.
         */
        virtual bool
        handleContentScrolling(const utils::Vector2f& posToFix,
                               const utils::Vector2f& whereTo,
                               const utils::Vector2i& motion);

        /**
         * @brief - Interface method allowing to handle the positionning of the
         *          support widget in case of a resize of the parent scrollable
         *          container. This allows inheriting classes to specialize the
         *          behavior if needed.
         *          The base behavior tries to accomodate the new display area
         *          so that the old center remains the same. The size of the
         *          displayed content might vary depending on the new size of
         *          the parent. Also we check for bounds so that we do not try
         *          to display inexisting parts of the content widget.
         *          Note that this method is called even when the support widget
         *          is not defined which means that the `support` widget can be
         *          `null`.
         *          We expect this method to returns the new size of the support
         *          widget as return value. In case this widget is valid a new
         *          resize event will be issued towards the support: otherwise
         *          nothing more will happen.
         * @param window - the new area available for this widget. This can be
         *                 used to display the support widget.
         * @param support - the support widget (might be `null`) which rendering
         *                  area should be adapted to the new `window`.
         * @return - the new rendering area to assign to the support widget if
         *           any or a suitable area to use to render the content of this
         *           widget.
         */
        virtual utils::Boxf
        onResize(const utils::Boxf& window,
                 core::SdlWidget* support);

        /**
         * @brief - Reimplementation of the base `core::engine::EngineObject` in order
         *          to handle the reset of the coordinates to follow. Indeed in case
         *          the scrolling stops we need to prepare to follow a new position.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        dropEvent(const core::engine::DropEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::engine::EngineObject` method to
         *          perform a saving of the position of the click of a mouse so that we
         *          can efficiently handle the drag events.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseButtonPressEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method to reset 
         *          coordinates of the position to follow. Indeed to perform scrolling
         *          on this widget we fix a position that we want to keep below the
         *          current position of the mouse. This behavior should stop whenever
         *          the user stops dragging the mouse (i.e. upon receiving a `dropEvent`)
         *          or if the mouse button corresponding to the scroll operation is not
         *          pressed anymore.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method to detect when
         *          an action is performed to modify the position of the viewport widget
         *          associated to this element. This allows to update the content to make
         *          it match the desired position.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseDragEvent(const core::engine::MouseEvent& e) override;

      private:

        /**
         * @brief - Used to retrieve the button to use to scroll the content
         *          of this widget.
         * @return - the default button to use to perform scrolling in this
         *           widget.
         */
        static
        core::engine::mouse::Button
        getScrollingButton() noexcept;

        /**
         * @brief - Used to determine whether this widget has a support widget
         *          attached to it. This allows to simplify a lot of methods
         *          by easily detecting when we should or shouldn't perform
         *          some operations.
         * @return - `true` if this widget as a support widget attached to it
         *           and `false` otherwise.
         */
        bool
        hasSupportWidget() const noexcept;

        /**
         * @brief - Returns the support widget attached to this element as a
         *          valid widget. In case no support widget is assigned an
         *          error is raised.
         *          Note that the user can check whether a support widget is
         *          attached through the `hasSupportWidget` method.
         *          If this method returns the value is guaranteed to not be
         *          `null`. This method assumes that the locker is already
         *          acquired.
         * @return - the support widget attached to this element.
         */
        core::SdlWidget*
        getSupportWidget() const;

        /**
         * @brief - Attempt to reset the coordinates to follow in case of a scrolling
         *          event. This method checks that the button provided in argument is
         *          the one used by the scrolling process and if this is the case do
         *          perform a reset of the coordinates to follow.
         *          Note that the locker is acquired by this method.
         * @param button - the button which attempts to reset the scrolling coordinates.
         */
        void
        attemptToClearCoords(const core::engine::mouse::Button& button) noexcept;

        /**
         * @brief - Used to attempt to retrieve the coordinates to follow. The
         *          drag and drop operation should allow to follow a specific
         *          position and make it always below the mouse cursor. To do
         *          that we need to remember what was this position and make
         *          it move according to the mouse's cursor.
         *          In case a drag operation is running we don't want to reset
         *          the position all the time as it might lead to inacurracies
         *          so this method provides a way to either set the coordinates
         *          to follow to the input value or to retrieve the existing
         *          ones.
         *          If the `m_coordsToFollow` are already defined this method
         *          will return the saved value. Otherwise it will create a new
         *          saved value with the input `coords` and return this.
         *          Note that this method assumes that the locker protecting the
         *          object is already acquired.
         * @param coords - the coordinates to assign as saved value if none are
         *                 defined yet.
         * @param force - `true` if the input `coords` should be set as the
         *                coordinates to track no matter if an existing value
         *                already exist.
         * @return - the saved coordinates to follow: either the already saved
         *           ones or the input `coords` if none were saved.
         */
        utils::Vector2f
        createOrGetCoordsToFollow(const utils::Vector2f& coords,
                                  bool force = false);

      private:

      /**
       * @brief - Convenience define allowing to define an optional position
       *          which is basically a position which may not be filled.
       */
      using OptionalPos = std::shared_ptr<utils::Vector2f>;

        /**
         * @brief - Used to protect concurrent accesses to the internal data of
         *          this scrollable widget.
         */
        mutable std::mutex m_propsLocker;

        /**
         * @brief - Used to hold the name of the support widget currently managed
         *          by this scrollable object. This is used when we need to delete
         *          the corresponding data when the content is updated for example.
         */
        std::string m_supportName;

        /**
         * @brief - Used to keep the initial position that was under the cursor when
         *          a dragging operation started. This allows to remember which point
         *          needs to be brought under the current mouse position.
         */
        OptionalPos m_coordsToFollow;

    };

    using ScrollableWidgetShPtr = std::shared_ptr<ScrollableWidget>;
  }
}

# include "ScrollableWidget.hxx"

#endif    /* SCROLLABLE_WIDGET_HH */
