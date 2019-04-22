#ifndef    SELECTOR_WIDGET_HXX
# define   SELECTOR_WIDGET_HXX

# include "SelectorWidget.hh"
# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    SelectorWidget::SelectorWidget(const std::string& name,
                                   core::SdlWidget* parent,
                                   const core::engine::Color& color,
                                   const utils::Sizef& area):
      core::SdlWidget(name, area, parent, color)
    {
      setLayout(std::make_shared<SelectorLayout>(0.0f, this));
    }

    inline
    void
    SelectorWidget::setActiveWidget(const std::string& name) {
      getLayoutAs<SelectorLayout>()->setActiveItem(name);
      makeContentDirty();
      makeGeometryDirty();
    }

    inline
    void
    SelectorWidget::setActiveWidget(const int& index) {
      getLayoutAs<SelectorLayout>()->setActiveItem(index);
      makeContentDirty();
      makeGeometryDirty();
    }

  }
}

#endif    /* SELECTOR_WIDGET_HXX */
