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

      protected:

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method in order
         *          to provide a custom implementation for the size update process
         *          We actually want to transmit the information to the underlying
         *          support widget so that it can update itself if needed.
         * @param window - the available size to perform the update.
         */
        void
        updatePrivate(const utils::Boxf& window) override;

      private:

        /**
         * @brief - Used to determine whether this widget has a support widget
         *          attached to it. This allows to simplify a lot of methods
         *          by easily detecting when we should or shouldn't perform
         *          some operations.
         * @return - `true` if this widget as a support widget attached to it
         *           and `false` otherwise.
         */
        bool
        hasSupportWidget() noexcept;

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
        getSupportWidget();

      private:

        /**
         * @brief - Used to protect concurrent accesses to the internal data of
         *          this scrollable widget.
         */
        std::mutex m_propsLocker;

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
