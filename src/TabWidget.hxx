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
      SelectorWidget* selector = getChildAs<SelectorWidget>(std::string("tabwidget_selector"));

      if (selector == nullptr) {
        error(
          std::string("Cannot retrieve selector widget from tabwidget"),
          std::string("No such widget")
        );
      }

      return *selector;
    }

  }
}

#endif    /* TAB_WIDGET_HXX */
