#ifndef    SELECTORLAYOUT_HXX
# define   SELECTORLAYOUT_HXX

# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    int
    SelectorLayout::addItem(core::LayoutItem* item,
                            const int& index)
    {
      // Use the base method to perform the insertion.
      int realID = core::Layout::addItem(item, index);

      // Handle insertion internally.
      handleItemInsertion(item, index, realID);

      // Return the ID provided by the base class.
      return realID;
    }

    inline
    int
    SelectorLayout::addItem(core::LayoutItem* item) {
      // Use the base method to perform the insertion.
      int index = core::Layout::addItem(item);

      // Handle insertion internally.
      handleItemInsertion(item, index, index);

      // Return the ID provided by the base class.
      return index;
    }

    inline
    int
    SelectorLayout::addItem(core::LayoutItem* item,
                            const unsigned& x,
                            const unsigned& y,
                            const unsigned& w,
                            const unsigned& h)
    {
      // Use the base method to perform the insertion.
      int index = core::Layout::addItem(item, x, y, w, h);

      // Handle insertion internally.
      handleItemInsertion(item, index, index);

      // Return the ID provided by the base class.
      return index;
    }

    inline
    void
    SelectorLayout::setActiveItem(const std::string& name) {
      // Try to retrieve the index of the item with the input name.
      // Not that we will activate the first item which name corresponds
      // to the input name.
      const int id = getIndexOf(name);

      // Activate this item using the dedicated handler.
      setActiveItem(id);
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

  }
}

#endif    /* SELECTORLAYOUT_HXX */
