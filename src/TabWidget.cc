
# include "TabWidget.hh"
# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    TabWidget::TabWidget(const std::string& name,
                         core::SdlWidget* parent,
                         const TabPosition& tabLayout,
                         const utils::Sizef& area):
      core::SdlWidget(name, area, parent, core::engine::Color::NamedColor::Magenta),
      m_activeTab(-1),
      m_tabLayout(tabLayout),
      m_titlesLayout(nullptr),
      m_tabCount(0u),
      m_tabs()
    {
      build();
    }

    TabWidget::~TabWidget() {}

    void
    TabWidget::insertTab(const int index,
                         core::SdlWidget* item,
                         const std::string& text)
    {
      // Check trivial case where the item is not valid.
      if (item == nullptr) {
        error(
          std::string("Cannot insert null tab in tabwidget"),
          std::string("Invalid null tab")
        );
      }

      // We only need to insert a tab title if the tab to insert is
      // not alone in the widget. Indeed if we only have one tab we
      // want to maximize the area available for the content and thus
      // we don't display any title.

      // Compute the tab's title: either the provided `text` or
      // the name of the widget.
      std::string title = text;
      if (title.empty()) {
        title = item->getName();
      }

      if (m_tabs.empty()) {
        // Insert the tab into the internal `m_tabs` array but do not
        // trigger the creation of a title widget.
        m_tabs.push_back(text);
      }
      else {
        // Check whether we should create the first item title widget:
        // this occurs when we are inserting the second tab into this
        // widget.
        if (m_tabs.size() == 1u) {
          createTitleForWidget(0u, m_tabs[0], false);
        }

        // Use the dedicated handler to create the title widget for the
        // input `index`.
        createTitleForWidget(index, title);

        // Activate the titles layout if needed (i.e. if at least two
        // tabs have been registered into this widget).
        if (m_tabs.size() > 1u) {
          m_titlesLayout->setVisible(true);
          makeGeometryDirty();
        }
      }

      // First insert the widget into the selector layout.
      getSelector().insertWidget(item, index);
    }

    void
    TabWidget::removeTab(const int index) {
      // Try to find the name associated whith the input `index`.
      if (index < 0 || index >= static_cast<int>(m_tabs.size())) {
        error(
          std::string("Cannot remove item ") + std::to_string(index) + " from tabwidget",
          std::string("No such item")
        );
      }

      // In order to remove this item we need to do two things:
      // 1. remove the item from the children of this widget.
      // 2. remove the title associated to this widget from the
      //    titles bar.
      // 3. update the internal `m_tabs` array to reflect the
      //    removed item.

      // 1. Remove the item from the selector layout.
      // TODO: Perform check for LinearLayout removal.

      // 2. Remove the item from the titles layout if needed.
      // We will also make the titles layout hidden if the tabs
      // count drop to less than 2.
      // TODO: Handle removal of the tab.

      // 3. Update the internal `m_tabs` array.
      // Create the new version of the tabs data.
      Tabs newTabs(m_tabs.size() - 1u);

      for (int i = 0u ; i < static_cast<int>(m_tabs.size()) ; ++i) {
        // We need to keep the name of the tab at the same position if its
        // index is smaller than the input `index` and move it one setp
        // closer if it is greater.
        if (i < index) {
          newTabs[i] = m_tabs[i];
        }
        else if (i == index) {
          // Ignore the tab to delete.
        }
        else {
          newTabs[i - 1] = m_tabs[i];
        }
      }

      m_tabs.swap(newTabs);
    }

    void
    TabWidget::build() {
      // This item is implemented in terms of a selector layout
      // which is associated to a linear layout where items are
      // representing the names of the widgets inserted in this
      // component.

      // This item is managed by a vertical linear layout which
      // should ever contain two items:
      // 1. the selector layout representing the content of the
      //    widgets inserted in this component.
      // 2. another linear layout representing the titles of the
      //    tab widgets inserted.
      //
      // We choose not to create an additional widget to support
      // this layout and rather use the hierarchical nesting of
      // the layout items.
      //
      // According to the tab layout associated to this widget
      // we should create the layout accordingly.

      LinearLayoutShPtr layout;

      switch (m_tabLayout) {
        case TabPosition::North:
        case TabPosition::South:
          layout = std::make_shared<LinearLayout>(
            std::string("tabwidget_layout"),
            this,
            LinearLayout::Direction::Vertical,
            0.0f,
            1.0f
          );
          break;
        case TabPosition::West:
        case TabPosition::East:
          layout = std::make_shared<LinearLayout>(
            std::string("tabwidget_layout"),
            this,
            LinearLayout::Direction::Horizontal,
            0.0f,
            1.0f
          );
          break;
        default:
          error(
            std::string("Could not create layout for tab widget"),
            std::string("Unknown tab position ") + std::to_string(static_cast<int>(m_tabLayout))
          );
          break;
      }

      // Assign the layout to this widget.
      setLayout(layout);

      // Create the secondary layout which will handle positionning
      // of widgets' titles.
      switch (m_tabLayout) {
        case TabPosition::North:
        case TabPosition::South:
          m_titlesLayout = std::make_shared<LinearLayout>(
            std::string("tabwidget_titles_layout"),
            nullptr,
            LinearLayout::Direction::Horizontal,
            0.0f,
            1.0f
          );
          break;
        case TabPosition::West:
        case TabPosition::East:
          m_titlesLayout = std::make_shared<LinearLayout>(
            std::string("tabwidget_layout"),
            this,
            LinearLayout::Direction::Vertical,
            0.0f,
            1.0f
          );
          break;
        default:
          error(
            std::string("Could not create layout for tab widget"),
            std::string("Unknown tab position ") + std::to_string(static_cast<int>(m_tabLayout))
          );
          break;
      }

      m_titlesLayout->setNested(true);

      // Create the selector layout which will contain the various
      // widgets inserted into this component.
      SelectorWidget* selector = new SelectorWidget(
        std::string("tabwidget_selector"),
        this,
        false,
        core::engine::Color::NamedColor::Olive
      );

      // Add the nested titles layout and the selector widget to
      // the general layout for this item.
      switch (m_tabLayout) {
        case TabPosition::North:
        case TabPosition::West:
          layout->addItem(m_titlesLayout.get());
          layout->addItem(selector);
          break;
        case TabPosition::South:
        case TabPosition::East:
          layout->addItem(selector);
          layout->addItem(m_titlesLayout.get());
          break;
        default:
          error(
            std::string("Could not create populate for tab widget"),
            std::string("Unknown tab position ") + std::to_string(static_cast<int>(m_tabLayout))
          );
          break;
      }

      // At first (i.e. with no tab inserted) the titles are not visible.
      m_titlesLayout->setVisible(false);
    }

    void
    TabWidget::createTitleForWidget(const int index,
                                    const std::string& text,
                                    bool updateIDs)
    {
      // Create the label widget which will represent this widget in the
      // title's bar.
      LabelWidget* titleWidget = new LabelWidget(
        std::string("title_for_") + std::to_string(m_tabCount),
        text,
        std::string("data/fonts/times.ttf"),
        10,
        LabelWidget::HorizontalAlignment::Center,
        LabelWidget::VerticalAlignment::Center,
        this
      );
      titleWidget->setPalette(getPalette());

      ++m_tabCount;

      // In order to correctly insert the item into the tabwidget we need
      // to both insert its representation into the selector layout but
      // also to insert the corresponding title in the bar.
      m_titlesLayout->addItem(titleWidget, index);

      // We also need to update the internal array of associations between
      // the names of the tab and their indices.
      // Return early if none of this is needed.
      if (!updateIDs) {
        return;
      }

      // Create the new version of the tabs data.
      Tabs newTabs(m_tabs.size() + 1u);

      for (int i = 0u ; i < static_cast<int>(m_tabs.size()) ; ++i) {
        // We need to keep the name of the tab at the same position if its
        // index is smaller than the input `index` and move it one setp
        // further if it is greater.
        if (i < index) {
          newTabs[i] = m_tabs[i];
        }
        else {
          newTabs[i + 1] = m_tabs[i];
        }
      }
      newTabs[index] = text;

      m_tabs.swap(newTabs);
    }

  }
}
