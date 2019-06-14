#ifndef    SELECTORLAYOUT_HXX
# define   SELECTORLAYOUT_HXX

# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    int
    SelectorLayout::addItem(core::LayoutItem* item) {
      // Assume the item will be inserted at the end of the layout.
      const int logicID = getItemsCount();

      // Insert the item.
      addItem(item, logicID);

      // Return the logical id of the item.
      return logicID;
    }

    inline
    void
    SelectorLayout::addItem(core::LayoutItem* item,
                            const int& index)
    {
      // Use the base method to perform the insertion.
      int physID = core::Layout::addItem(item);

      // Handle insertion internally.
      handleItemInsertion(item, index, physID);
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

    inline
    int
    SelectorLayout::getLogicalIDFromPhysicalID(const int physID) const noexcept {
      // Assume we can't find the logical id.
      int logicID = -1;

      // Traverse the internal array of associations.
      int id = 0;
      while (id < static_cast<int>(m_idsToPosition.size()) && logicID < 0) {
        if (m_idsToPosition[id] == physID) {
          logicID = id;
        }
        ++id;
      }

      // Return the logical id, either valid or invalid.
      return logicID;
    }

    inline
    int
    SelectorLayout::getPhysicalIDFromLogicalID(const int logicID) const noexcept {
      // Check whether the input logical id seems valid.
      if (logicID < 0 || logicID >= static_cast<int>(m_idsToPosition.size())) {
        return -1;
      }

      // The physical id is directly given by the associations table.
      return m_idsToPosition[logicID];
    }

  }
}

#endif    /* SELECTORLAYOUT_HXX */
