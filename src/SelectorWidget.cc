
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
      // Insert the input widget as child of this widget so that it gets redrawn.
      if (widget != nullptr) {
        widget->setParent(this);
      }
      // We rely on the internal layout method to perform the insertion.
      getLayout().addItem(widget, index);
    }

    void
    SelectorWidget::removeItem(const int index) {
      // Check whether this index is valid.
      if (index < 0 || index >= getChildrenCount()) {
        error(
          std::string("Cannot remove widget ") + std::to_string(index) + " from selector widget",
          std::string("No such item, only ") + std::to_string(getChildrenCount()) + " item(s) available"
        );
      }

      // Remove the widget from the layout.
      getLayout().removeItemFromIndex(index);

      // TODO: Remove the widget from children.
      // To do so we need to determine the widget associated to this
      // item. This has to be done using the `SelectorLayout` for now,
      // as we don't save any information about the indices and the
      // corresponding items in the `SelectorWidget` itself.
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

      // Remove the widget from the layout.
      int logicID = getLayout().removeItem(widget);

      // Now we can remove the input `widget` from the children' list.
      removeWidget(widget);

      // Return the index of this widget.
      return logicID;
    }

  }
}
