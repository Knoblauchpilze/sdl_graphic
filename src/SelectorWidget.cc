
# include "SelectorWidget.hh"

namespace sdl {
  namespace graphic {

    SelectorWidget::SelectorWidget(const std::string& name,
                                   core::SdlWidget* parent,
                                   const bool switchOnLeftClick,
                                   const core::engine::Color& color,
                                   const utils::Sizef& area):
      core::SdlWidget(name, area, parent, color),
      m_switchOnLeftClick(switchOnLeftClick)
    {
      setLayout(std::make_shared<SelectorLayout>(std::string("selector_layout_for_") + getName(), this, 0.0f));
    }

    void
    SelectorWidget::insertWidget(core::SdlWidget* widget,
                                 const int& index)
    {
      // We rely on the internal layout method to perform the insertion.
      getLayout().addItem(widget, index);
    }

  }
}
