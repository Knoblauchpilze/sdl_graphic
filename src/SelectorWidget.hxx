#ifndef    SELECTORWIDGET_HXX
# define   SELECTORWIDGET_HXX

# include "SelectorWidget.hh"
# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    SelectorWidget::SelectorWidget(const std::string& name,
                                   sdl::core::SdlWidget* parent,
                                   const SDL_Color& backgroundColor):
      sdl::core::SdlWidget(name,
                           sdl::core::Boxf(),
                           parent,
                           backgroundColor)
    {
      setLayout(std::make_shared<SelectorLayout>(0.0f, this));
    }

    inline
    void
    SelectorWidget::setActiveWidget(const std::string& name) {
      std::lock_guard<std::mutex> guard(getLocker());
      getLayoutAs<SelectorLayout>()->setActiveItem(name);
    }

    inline
    void
    SelectorWidget::setActiveWidget(const int& index) {
      std::lock_guard<std::mutex> guard(getLocker());
      getLayoutAs<SelectorLayout>()->setActiveItem(index);
    }

    inline
    void
    SelectorWidget::onMouseButtonReleasedEvent(const SDL_MouseButtonEvent& mouseButtonEvent) {
      std::lock_guard<std::mutex> guard(getLocker());
      SelectorLayout* layout = getLayoutAs<SelectorLayout>();
      if (layout != nullptr) {
        layout->setActiveItem((layout->getActiveItemId() + 1) % layout->getItemsCount());
      }
      makeDirty();
    }

  }
}

#endif    /* SELECTORWIDGET_HXX */
