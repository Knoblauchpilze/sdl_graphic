
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

    void
    SelectorWidget::addWidget(core::SdlWidget* widget) {
      // In order not to break the encapsulation and keep the fact that this
      // item uses a `SelectorLayout`, we prefer to reimplement this method so
      // that it is still possible to keep the standard way of parenting
      // widgets: add this item as parent when creating the children.
      // Of course we still need to perform the base operations.
      // Also note that this method does not allow to choose the order of the
      // inserted widget: it will be inserted at the last position of the
      // layout.

      core::SdlWidget::addWidget(widget);

      // Add the input `widget` to the layout.
      SelectorLayout* layout = getLayoutAs<SelectorLayout>();
      if (layout == nullptr) {
        error(
          std::string("Could not insert widget \"") + widget->getName() + "\" into selector widget",
          std::string("Invalid layout")
        );
      }

      layout->addItem(widget);
    }

  }
}
