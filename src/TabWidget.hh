#ifndef    TAB_WIDGET_HH
# define   TAB_WIDGET_HH

# include <memory>
# include <vector>
# include <sdl_core/SdlWidget.hh>
# include "LinearLayout.hh"
# include "SelectorWidget.hh"

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
         * @brief - Retrieves the number of tabs existing in this widget.
         * @return - a value describing the number of tabs for this widget.
         */
        int
        getTabsCount() const noexcept;

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

        /**
         * @brief - Used to remove the tab widget specified by the input pointer. If no such
         *          item exists in this component an error is raised.
         * @param widget - a pointer to the tab to remove from this layout.
         */
        void
        removeTab(core::SdlWidget* widget);

      private:

        /**
         * @brief - Used to build this component by creating the adequate layout and the component
         *          to use to represent each item of the tab widget.
         */
        void
        build();

        /**
         * @brief - Used to create a title widget from the input `text` and insert it
         *          at the desired `index` in the internal titles layout.
         *          One can specify whether this method should update the internal array
         *          registering all tabs `m_tabs`. This is useful because of the way wa
         *          handle the non-creation of the title layout for the first tab widget.
         * @param index - the index at which the title should be inserted inside the
         *                internal titles layout.
         * @param text - the text which should be displayed for this title.
         * @param updateIDs - true if the internal `m_tabs` array should be updated, false
         *                    otherwise.
         */
        void
        createTitleForWidget(const int index,
                             const std::string& text,
                             bool updateIDs = true);

        /**
         * @brief - Wrapper around the parent `getChildAs` method for better convenience.
         *          Note that if the selector widget cannot be found an error is raised.
         * @return - a reference to the selector widget associated to this tab layout.
         */
        SelectorWidget&
        getSelector();

        /**
         * @brief - Used to connect this method as receiver of the titles widget so that we
         *          can update the internal widget displayed when the user clicks on the
         *          title.
         * @param name - the name of the widget which has been clicked.
         */
        void
        onTitleClicked(const std::string& name);

        /**
         * @brief - Used to retrieve the name of the title widget representing the tab from
         *          the internal tab id.
         * @return - a string representing the name of the title widget.
         */
        std::string
        getTitleNameFromTabID();

      private:

        using Tabs = std::vector<std::string>;

        // TODO: Handle tab activation.
        int m_activeTab;

        /**
         * @brief - Describes the position of the tab titles relatively to the tab content. Based
         *          on the orientation of the tabs, the titles may be displayed horizontally or
         *          vertically. This allows to always use the less space possible to display tabs
         *          titles so that we get maximum space for the content.
         */
        TabPosition m_tabLayout;

        /**
         * @brief - Contains the description of the layout positionning titles of items inserted in
         *          this tab widget. This basically corresponds to the way we allow the user to
         *          switch from one tab to another.
         */
        LinearLayoutShPtr m_titlesLayout;

        /**
         * @brief - A counter which is incremented at each tab creation: allows to provide unique
         *          naming for each tab of this widget thus making sure that they will not be
         *          merged or overriden when added as children.
         */
        unsigned m_tabCount;

        /**
         * @brief - Associates the index of the tab with its name. This allows the user to reference
         *          tabs by index instead of using their name (which might not have been provided by
         *          the user).
         */
        Tabs m_tabs;

        /**
         * @brief - Is either empty or contains the text to assign to the unique tab for this widget.
         */
        std::string m_lonelyTab;

    };

    using TabWidgetShPtr = std::shared_ptr<TabWidget>;
  }
}

# include "TabWidget.hxx"

#endif    /* TAB_WIDGET_HH */
