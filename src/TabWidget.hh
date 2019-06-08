#ifndef    TAB_WIDGET_HH
# define   TAB_WIDGET_HH

# include <memory>
# include <unordered_map>
# include <sdl_core/SdlWidget.hh>
# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    class TabWidget: public core::SdlWidget {
      public:

        /**
         * @brief - Describes the position of the tab indication in this widget relatively
         *          to the content of the widgets.
         */
        enum class TabPosition {
          North, //<! - Tab indication will be displayed above the content.
          South, //<! - Tab indication will be displayed below the content.
          West,  //<! - Tab indication will be displayed on the left of the content.
          East   //<! - Tab indication will be displayed on the right of the content.
        };

      public:

        TabWidget(const std::string& name,
                 core::SdlWidget* parent = nullptr,
                 const TabPosition& tabLayout = TabPosition::North,
                 const utils::Sizef& area = utils::Sizef());

        virtual ~TabWidget();

        /**
         * @brief - Used to insert the specified widget at the index `index` under the
         *          specified title. We use the index as a loose description of the
         *          desired position of the widget and value like `-1` or `size + 1`
         *          are accepted and will result respectively in the tab being inserted
         *          before the first tab or after the last one.
         *          Note that if the user does not provide any `text` for this tab widget
         *          to be referenced, the name of the item will be used instead.
         * @param index - the position where the tab widget should be inserted.
         * @param item - a pointer providing the description of the widget to insert.
         * @param text - a string representing the name under which the item should be
         *               referenced in the title bar. If no name is provided the widget's
         *               name will be used.
         */
        void
        insertTab(const int index,
                  core::SdlWidget* item,
                  const std::string& text = std::string());

        /**
         * @brief - Used to remove the tab widget located at index `index`. If no such
         *          index exists in this component an error is raised.
         * @param index - the position of the tab widget to remove from this layout.
         */
        void
        removeTab(const int index);

      private:

        /**
         * @brief - Used to build this component by creating the adequate layout and the component
         *          to use to represent each item of the tab widget.
         */
        void
        build();

      private:

        using TabsMap = std::unordered_map<int, std::string>;

        // TODO: Handle tab activation.
        int m_activeTab;

        // TODO: Handle tab layout.
        TabPosition m_tabLayout;

        /**
         * @brief - Contains the description of the layout positionning titles of items inserted in
         *          this tab widget. This basically corresponds to the way we allow the user to
         *          switch from one tab to another.
         */
        LinearLayoutShPtr m_titlesLayout;

        /**
         * @brief - Associates the index of the tab with its name. This allows the user to reference
         *          tabs by index instead of using their name (which might not have been provided by
         *          the user).
         */
        TabsMap m_tabs;

    };

    using TabWidgetShPtr = std::shared_ptr<TabWidget>;
  }
}

# include "TabWidget.hxx"

#endif    /* TAB_WIDGET_HH */
