
# include "SelectorWidget.hh"

namespace sdl {
  namespace graphic {

    SelectorWidget::SelectorWidget(const std::string& name,
                                   core::SdlWidget* parent,
                                   const bool switchOnLeftClick,
                                   const core::engine::Color& color,
                                   const utils::Sizef& area):
      core::SdlWidget(name, area, parent, color),

      m_propsLocker(),

      m_switchOnLeftClick(switchOnLeftClick),
      m_activeItem(0u)
    {
      SelectorLayoutShPtr layout = std::make_shared<SelectorLayout>(
        std::string("selector_layout_for_") + getName(),
        this,
        0.0f
      );
      layout->allowLog(false);

      setLayout(layout);
    }

    void
    SelectorWidget::insertWidget(core::SdlWidget* widget,
                                 int index)
    {
      if (widget == nullptr) {
        error(
          std::string("Cannot insert widget at index ") + std::to_string(index) + " in selector",
          std::string("Invalid null item")
        );
      }

      Guard guard(m_propsLocker);

      // Insert the input widget as child of this widget so that it gets redrawn.
      widget->setParent(this);

      // We rely on the internal layout method to perform the insertion.
      SelectorLayout& l = getLayout();
      l.addItem(widget, index);

      m_activeItem = l.getActiveItemId();
    }

    int
    SelectorWidget::removeItem(core::SdlWidget* widget) {
      // Check whether the item is valid.
      if (widget == nullptr) {
        error(
          std::string("Cannot remove widget from selector layout"),
          std::string("Invalid null item")
        );
      }

      Guard guard(m_propsLocker);

      // Remove the widget from the layout.
      SelectorLayout& l = getLayout();
      int logicID = l.removeItem(widget);

      // Now we can remove the input `widget` from the children' list.
      removeWidget(widget);

      m_activeItem = 0u;
      if (l.getItemsCount()) {
        m_activeItem = l.getActiveItemId();
      }

      // Return the index of this widget.
      return logicID;
    }

  }
}
