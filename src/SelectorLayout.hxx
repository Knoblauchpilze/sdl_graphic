#ifndef    SELECTORLAYOUT_HXX
# define   SELECTORLAYOUT_HXX

# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    int
    SelectorLayout::addItem(core::SdlWidget* item) {
      // Use the base method to perform the insertion.
      int index = core::Layout::addItem(item);

      // Handle the insertion of this item if it is valid:
      // we set the visible status of the widget and also
      // set it as active if it is the only one inserted
      // in the layout.
      if (item != nullptr) {
        handleItemInsertion(item);
      }

      // Return the produced index.
      return index;
    }

    inline
    int
    SelectorLayout::addItem(core::SdlWidget* item,
                            const unsigned& x,
                            const unsigned& y,
                            const unsigned& w,
                            const unsigned& h)
    {
      // Use the base method to perform the insertion.
      int index = core::Layout::addItem(item, x, y, w, h);

      // Handle the insertion of this item if it is valid:
      // we set the visible status of the widget and also
      // set it as active if it is the only one inserted
      // in the layout.
      if (item != nullptr) {
        handleItemInsertion(item);
      }

      // Return the produced index.
      return index;
    }

    inline
    void
    SelectorLayout::setActiveItem(const std::string& name) {
      // Try to retrieve the index of the widget with the input name.
      // Not that we will activate the first widget which name corresponds
      // to the input name.

      const int id = getIndexOf(name);

      // Check whether we could find a widget with the specified name.
      if (id < 0) {
        error(
          std::string("Cannot activate item \"") + name + "\"",
          std::string("No such element")
        );
      }

      // Activate this item using the dedicated handler.
      setActiveItem(id);
    }

    inline
    void
    SelectorLayout::setActiveItem(const int& index) {
      // Check whether the provided index is valid.
      if (!isValidIndex(index)) {
        error(
          std::string("Cannot activate child ") + std::to_string(index),
          std::string("Only ") + std::to_string(getItemsCount()) + " item(s) registered"
        );
      }

      // If the index corresponds to the currently active item we don't have to
      // do anything.
      if (index == m_activeItem) {
        return;
      }

      // Activate the input item and invalidate the layout.
      m_activeItem = index;
      makeGeometryDirty();
    }

    inline
    std::string
    SelectorLayout::getActiveItem() const {
      // Check that we can effectively access the active item.
      if (m_activeItem < 0) {
        error(
          std::string("Cannot retrieve name of active child ") + std::to_string(m_activeItem),
          std::string("No active item selected")
        );
      }
      if (m_activeItem >= getItemsCount()) {
        error(
          std::string("Cannot retrieve name of active child ") + std::to_string(m_activeItem),
          std::string("Only ") + std::to_string(getItemsCount()) + " item(s) registered"
        );
      }

      // Retrieve the active item as a widget pointer.
      const core::SdlWidget* activeItem = getWidgetAt(m_activeItem);
      if (activeItem == nullptr) {
        error(
          std::string("Cannot retrieve name of active child ") + std::to_string(m_activeItem),
          std::string("Invalid null element")
        );
      }

      // Retrieve the name of the active item.
      return activeItem->getName();
    }

    inline
    int
    SelectorLayout::getActiveItemId() const {
      if (m_activeItem < 0) {
        error(
          std::string("Cannot retrieve active child ") + std::to_string(m_activeItem),
          std::string("No active item selected")
        );
      }
      if (m_activeItem >= getItemsCount()) {
        error(
          std::string("Cannot retrieve active child ") + std::to_string(m_activeItem),
          std::string("Only ") + std::to_string(getItemsCount()) + " item(s) registered"
        );
      }

      return m_activeItem;
    }

    inline
    void
    SelectorLayout::handleItemInsertion(core::SdlWidget* item) {
      // If this item is the only one inserted in the widget,
      // use it as the active widget.
      // Otherwise, we should make it not visible.
      if (getItemsCount() == 1) {
        m_activeItem = 0u;
      }
      else {
        item->setVisible(false);
      }
    }

  }
}

#endif    /* SELECTORLAYOUT_HXX */
