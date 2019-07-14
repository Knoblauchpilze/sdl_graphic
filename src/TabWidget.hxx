#ifndef    TAB_WIDGET_HXX
# define   TAB_WIDGET_HXX

# include "TabWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    int
    TabWidget::getTabsCount() const noexcept {
      return m_tabs.size();
    }

    inline
    SelectorWidget&
    TabWidget::getSelector() {
      SelectorWidget* selector = getChildAs<SelectorWidget>(getSelectorName());

      if (selector == nullptr) {
        error(
          std::string("Cannot retrieve selector widget from tabwidget"),
          std::string("No such widget")
        );
      }

      return *selector;
    }

    inline
    std::string
    TabWidget::getTitleNameFromTabID() {
      // Retrieve an identifier for the tab.
      const int id = m_tabCount;
      ++m_tabCount;

      return std::string("title_for_") + std::to_string(id);
    }

    inline
    std::string
    TabWidget::getSelectorName() const noexcept {
      return getName() + "_selector";
    }

  }
}

#endif    /* TAB_WIDGET_HXX */
