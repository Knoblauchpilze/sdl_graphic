
# include "TabWidget.hh"
# include "SelectorWidget.hh"
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

      // Compute the tab's title: either the provided `text` or
      // the name of the widget.
      std::string title = text;
      if (title.empty()) {
        title = item->getName();
      }

      // Create the label widget which will represent this widget in the
      // title's bar.
      LabelWidget* titleWidget = new LabelWidget(
        std::string("title_for_") + std::to_string(index) + "_" + item->getName(),
        title,
        std::string("data/fonts/times.ttf"),
        10,
        LabelWidget::HorizontalAlignment::Center,
        LabelWidget::VerticalAlignment::Center,
        this
      );
      titleWidget->setPalette(item->getPalette());

      // In order to correctly insert the item into the tabwidget we need
      // to both insert its representation into the selector layout but
      // also to insert the corresponding title in the bar.
      m_titlesLayout->addItem(titleWidget, index);

      // First insert the widget into the selector layout.
      getChildAs<SelectorWidget>(std::string("tabwidget_selector"))->insertWidget(item, index);
    }

    void
    TabWidget::removeTab(const int index) {
      // Try to find the name associated whith the input `index`.
      const TabsMap::const_iterator tab = m_tabs.find(index);
      if (tab == m_tabs.cend()) {
        error(
          std::string("Cannot remove item ") + std::to_string(index) + " from tabwidget",
          std::string("No such item")
        );
      }

      // In order to remove this item we need to do two things:
      // 1. remove the item from the children of this widget.
      // 2. remove the title associated to this widget from the
      //    titles bar.

      // TODO: Handle removal of the tab.
      // TODO: Perform check for LinearLayout removal.
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

      LinearLayoutShPtr layout = std::make_shared<LinearLayout>(
        std::string("tabwidget_layout"),
        this,
        LinearLayout::Direction::Vertical,
        0.0f,
        1.0f
      );

      // Assign the layout to this widget.
      setLayout(layout);

      // Create the secondary layout which will handle positionning
      // of widgets' titles.
      m_titlesLayout = std::make_shared<LinearLayout>(
        std::string("tabwidget_titles_layout"),
        nullptr,
        LinearLayout::Direction::Horizontal,
        0.0f,
        1.0f
      );
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
      layout->addItem(m_titlesLayout.get());
      layout->addItem(selector);
    }

  }
}
