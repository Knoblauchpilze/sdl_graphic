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
    int
    SelectorLayout::removeItem(core::SdlWidget* item) {
      // We need to determine whether this item corresponds to the active item.
      // If it is the case we need to update this layout so that it does not
      // have any active widget anymore. Otherwise we have to handle the update
      // of the internal `m_activeItem` si that it still reflects the active
      // item if any.

      if (item == nullptr) {
        // Use the base handler, we cannot do anything on our side.
        return Layout::removeItem(item);
      }

      // Let's retrieve the index of this layout in the container.
      const int idItem = getIndexOf(item);

      // Check whether this item was found in the layout.
      if (idItem < 0) {
        // Use the base handler, we cannot do anything useful in here.
        return Layout::removeItem(item);
      }

      // Check whether this item is the active item.
      if (idItem == m_activeItem) {
        // No more active item.
        m_activeItem = -1;

        // Use the base handler to remove the item and we're done.
        return Layout::removeItem(item);
      }

      // The item is not the currently selected item. We need to update it
      // so that the active item stays the same even after removing the input
      // item.
      // To do so, we will first retrieve the widget associated to the active
      // item, then proceed to the removal of the input item and finally update
      // the active item with its new index.
      // Of course this only applies if there is an active item in the first
      // place.

      // Check whether there is an active item.
      if (m_activeItem < 0) {
        // We only have to perform the removal of the item using the base handler.
        return Layout::removeItem(item);
      }

      // First retrieve the widget corresponding to the active item.
      core::SdlWidget* activeItem = getWidgetAt(m_activeItem);

      // Remove the item using the dedicated handler.
      int removedID = Layout::removeItem(item);

      // Retrieve the index of the active item.
      const int newID = getIndexOf(activeItem);
      if (!isValidIndex(newID)) {
        // Should not occur: something must have gone wrong during the removal.
        m_activeItem = -1;

        error(
          std::string("Could not update active item after removing item \"") + item->getName() + "\"",
          std::string("Active item could not be retrieve")
        );
      }

      // Update the index of the active item.
      m_activeItem = newID;

      // Returnt he value provided by the base handler.
      return removedID;
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
      invalidate();
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
