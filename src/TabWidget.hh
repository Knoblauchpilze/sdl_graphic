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
        insertTab(int index,
                  core::SdlWidget* item,
                  const std::string& text = std::string());

        /**
         * @brief - Used to remove the tab widget located at index `index`. If no such
         *          index exists in this component an error is raised.
         * @param index - the position of the tab widget to remove from this layout.
         */
        void
        removeTab(int index);

        /**
         * @brief - Used to remove the tab widget specified by the input pointer. If no such
         *          item exists in this component an error is raised.
         *          Note that this function calls the `removeTab` overloaded method when the
         *          index for this widget has been determined.
         * @param widget - a pointer to the tab to remove from this layout.
         */
        void
        removeTab(core::SdlWidget* widget);

      private:

        /**
         * @brief - Used to define a maximum size for the titles that can be added to describe
         *          the elements of a tab widget (i.e. the individual tabs). Defining a maximum
         *          size helps to keep most of the available space for the tab itself. Note that
         *          based on the orientation of the tab widget the size will either be applied
         *          to the width or height of each title.
         * @return - a value to use when defining any title so that it does not grow past a certain
         *           point.
         */
        static
        float
        getMaximumSizeForTitle() noexcept;

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
         * @param item - the name of the widget associated with this title.
         * @param palette - the palette to assign to the widget representing the title.
         * @param updateIDs - true if the internal `m_tabs` array should be updated, false
         *                    otherwise.
         */
        void
        createTitleForWidget(int index,
                             const std::string& text,
                             const std::string& item,
                             const core::engine::Palette& palette,
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

        /**
         * @brief - Retrieves the name of the selector to use either to create it or retrieve
         *          it when needed.
         * @return - the string representing the selector's name.
         */
        std::string
        getSelectorName() const noexcept;

        /**
         * @brief - Removes the specified `index` from the internal `m_tabs` list. No controls
         *          are performed to verify whether the `index` is valid.
         * @param index - the index to remove from the internal list.
         */
        void
        removeIndexFromInternal(int index);

      private:

        /**
         * @brief - Convenience structure recording information about a tab.
         */
        struct TabInfo {
          std::string itemName;
          std::string titleWidgetName;
          std::string tabName;
        };

        using Tabs = std::vector<TabInfo>;

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
    };

    using TabWidgetShPtr = std::shared_ptr<TabWidget>;
  }
}

# include "TabWidget.hxx"

#endif    /* TAB_WIDGET_HH */
