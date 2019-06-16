#ifndef    SELECTOR_WIDGET_HH
# define   SELECTOR_WIDGET_HH

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
        setActiveWidget(const int& index);

        /**
         * @brief - Used to switch to the next item in the layout and activate it.
         *          According to the order into which the items have been inserted
         *          in this widget this method will activate the next available
         *          item in the list.
         *          If no item is currently selected the first one will be selected
         *          and if no more items can be found, it will loop over to the first
         *          item.
         */
        void
        switchToNext();

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
                     const int& index);

        /**
         * @brief - Removes the widget at the specified `index`. The active item is updated
         *          to the previous available widget (if any) if the removed index was the
         *          active one.
         *          Note that if no such index exists in the selector an error is raised.
         * @param index - the index of the widget to remove.
         */
        void
        removeItem(const int index);

        /**
         * @brief - Removes the specified item frmo this widget. Just like the `removeWidget`
         *          overload the active item is updated if needed.
         *          Note that if the widget does not exist an error is raised. Also note that
         *          this function calls the `removeWidget` internally.
         * @param widget - the widget to remove.
         * @return - the index of this widget in the layout.
         */
        int
        removeItem(core::SdlWidget* widget);

      protected:

        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

      private:

        bool
        switchOnClick() const noexcept;

        SelectorLayout&
        getLayout();

      private:

        bool m_switchOnLeftClick;

    };

    using SelectorWidgetShPtr = std::shared_ptr<SelectorWidget>;
  }
}

# include "SelectorWidget.hxx"

#endif    /* SELECTOR_WIDGET_HH */
