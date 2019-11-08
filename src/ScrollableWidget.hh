#ifndef    SCROLLABLE_WIDGET_HH
# define   SCROLLABLE_WIDGET_HH

# include <mutex>
# include <memory>
# include <sdl_core/SdlWidget.hh>

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
         *          to get an idea of how big the widget should be
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
         * @brief - Reimplementation of the base `core::SdlWidget` method to detect when
         *          an action is performed to modify the position of the viewport widget
         *          associated to this element. This allows to update the content to make
         *          it match the desired position.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseDragEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method to detect when the
         *          wheel is used: this should trigger a zooming behavior on the content of
         *          the scrollable widget.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseWheelEvent(const core::engine::MouseEvent& e) override;

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
         *          `null`.
         * @return - the support widget attached to this element.
         */
        core::SdlWidget*
        getSupportWidget() const;

      private:

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

    };

    using ScrollableWidgetShPtr = std::shared_ptr<ScrollableWidget>;
  }
}

# include "ScrollableWidget.hxx"

#endif    /* SCROLLABLE_WIDGET_HH */
