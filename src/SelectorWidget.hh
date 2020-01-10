#ifndef    SELECTOR_WIDGET_HH
# define   SELECTOR_WIDGET_HH

# include <mutex>
# include <memory>
# include <string>
# include <vector>
# include <unordered_map>
# include <sdl_core/SdlWidget.hh>
# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    class SelectorWidget: public core::SdlWidget {
      public:

        SelectorWidget(const std::string& name,
                       core::SdlWidget* parent = nullptr,
                       const bool switchOnLeftClick = false,
                       const core::engine::Color& color = core::engine::Color(),
                       const utils::Sizef& area = utils::Sizef());

        ~SelectorWidget() = default;

        void
        setActiveWidget(const std::string& name);

        void
        setActiveWidget(int index);

        /**
         * @brief - Used to insert the input `widget` at the specified `index` in the
         *          internal layout. Raises an error if the insertion cannot be performed
         *          for some reason.
         * @param widget - the widget to insert, should not be null.
         * @param index - the position at which the widget should be inserted, might be
         *                negative (in which case the widget will be inserted in first
         *                position in the layout) or larger than the actual widgets' count
         *                (in which case the widget will be inserted in last position).
         */
        void
        insertWidget(core::SdlWidget* widget,
                     int index);

        /**
         * @brief - Removes the specified item from this widget. Just like the `removeWidget`
         *          overload the active item is updated if needed.
         *          Note that if the widget does not exist an error is raised. Also note that
         *          this function calls the `removeWidget` internally.
         * @param widget - the widget to remove.
         * @return - the index of this widget in the layout.
         */
        int
        removeItem(core::SdlWidget* widget);

        /**
         * @brief - Return the current active widget in the selector. A value of `0` is returned
         *          in case there are no items registered in this element.
         * @return - a value indicating the current active widget.
         */
        int
        getActiveItem();

      protected:

        /**
         * @brief - Redefinition of the base `SdlWidget` method to provide a custom behavior
         *          upon clicking on any child of this widget. Basically when the user clicks
         *          on a child of this widget it triggers a gain focus chain which can be
         *          intercepted by this widget in order to switch to another registered child
         *          if activated. This allows to effectively provide some kind of carousel
         *          behavior really easily.
         *          Note that this interfere with the standard processing and the rest of the
         *          widgets hierarchy will not be notified from any focus events if the gain
         *          focus produce a switch to the next child. This is intended and perfectly
         *          expected behavior.
         *          In case the gain focus event cannot be turned into a switch to the next
         *          active child the behavior is similar to the existing one defined in the
         *          base class (i.e. `SdlWidget`).
         * @param e - the gain focus event to process.
         * @return - `true` if the event has been recognized, `false` otherwise.
         */
        bool
        gainFocusEvent(const core::engine::FocusEvent& e) override;

        /**
         * @brief - Used to switch to the next item in the layout and activate it.
         *          According to the order into which the items have been inserted
         *          in this widget this method will activate the next available
         *          item in the list.
         *          If no item is currently selected the first one will be selected
         *          and if no more items can be found, it will loop over to the first
         *          item.
         *          Assumes that the locker is already acquired.
         */
        void
        switchToNext();

      private:

        bool
        switchOnClick() const noexcept;

        SelectorLayout&
        getLayout();

      private:

        /**
         * @brief - Protects this widget from concurrent accesses.
         */
        std::mutex m_propsLocker;


        /**
         * @brief - Describes whether a left click on this widget should select the next
         *          registered item.
         */
        bool m_switchOnLeftClick;

        /**
         * @brief - Holds an indication of the current active item for this selector. Note
         *          that it is a copy of the data kept internally by the related layout so
         *          there's not much to be gain from this info.
         */
        int m_activeItem;
    };

    using SelectorWidgetShPtr = std::shared_ptr<SelectorWidget>;
  }
}

# include "SelectorWidget.hxx"

#endif    /* SELECTOR_WIDGET_HH */
