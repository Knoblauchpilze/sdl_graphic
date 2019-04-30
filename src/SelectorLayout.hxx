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
      int indexItem = 0;
      bool found = false;
      while (indexItem < getItemsCount() && !found) {
        if (m_items[indexItem] != nullptr && m_items[indexItem]->getName() == name) {
          found = true;
        }
        else {
          ++indexItem;
        }
      }

      if (!found) {
        error(std::string("Cannot activate item ") + name + " in selector layout, item not found");
      }

      setActiveItem(indexItem);
    }

    inline
    void
    SelectorLayout::setActiveItem(const int& index) {
      if (index >= getItemsCount()) {
        error(
          std::string("Cannot activate child ") + std::to_string(index) +
          " in selector layout only containing " + std::to_string(getItemsCount()) + " child(ren)"
        );
      }

      if (index == m_activeItem) {
        return;
      }

      m_activeItem = index;
      invalidate();
    }

    inline
    std::string
    SelectorLayout::getActiveItem() const {
      if (m_activeItem < 0) {
        error(std::string("Cannot retrieve name of active child for selector layout, no such element"));
      }
      if (m_items[m_activeItem] == nullptr) {
        error(std::string("Cannot retrieve name of active child for selector layout, invalid null element"));
      }

      return m_items[m_activeItem]->getName();
    }

    inline
    int
    SelectorLayout::getActiveItemId() const {
      if (m_activeItem < 0) {
        error(std::string("Cannot retrieve name of active child for selector layout, no such element"));
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
