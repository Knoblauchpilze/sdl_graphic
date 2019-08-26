#ifndef    LINEARLAYOUT_HXX
# define   LINEARLAYOUT_HXX

# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    int
    LinearLayout::addItem(core::LayoutItem* item) {
      // Assume the item will be inserted at the end of the layout.
      const int logicID = getItemsCount();

      // Insert the item.
      addItem(item, logicID);

      // Return the logical id of the item.
      return logicID;
    }

    inline
    const LinearLayout::Direction&
    LinearLayout::getDirection() const noexcept {
      return m_direction;
    }

    inline
    const float&
    LinearLayout::getComponentMargin() const noexcept {
      return m_componentMargin;
    }

    inline
    int
    LinearLayout::getLogicalIDFromPhysicalID(const int physID) const noexcept {
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
    LinearLayout::getPhysicalIDFromLogicalID(const int logicID) const noexcept {
      // Check whether the input logical id seems valid.
      if (logicID < 0 || logicID >= static_cast<int>(m_idsToPosition.size())) {
        return -1;
      }

      // The physical id is directly given by the associations table.
      return m_idsToPosition[logicID];
    }

    inline
    utils::Sizef
    LinearLayout::computeAvailableSize(const utils::Boxf& totalArea) const noexcept {
      // We assume the layout is not empty.

      // Use the base class method to provide a first rough estimation.
      utils::Sizef internalSize = Layout::computeAvailableSize(totalArea);

      // Handle horizontal layout.
      if (getDirection() == Direction::Horizontal) {
        return internalSize - utils::Sizef((getItemsCount() - 1.0f) * m_componentMargin, 0.0f);
      }

      // Assume the layout is vertical.
      return internalSize - utils::Sizef(0.0f, (getItemsCount() - 1.0f) * m_componentMargin);
    }

    inline
    utils::Sizef
    LinearLayout::computeDefaultItemBox(const utils::Sizef& area,
                                        const unsigned& itemsCount) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return utils::Sizef(
          allocateFairly(area.w(), itemsCount),
          area.h()
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return utils::Sizef(
          area.w(),
          allocateFairly(area.h(), itemsCount)
        );
      }
      error(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");

      // Make compiler quiet about reaching end of non void function.
      return utils::Sizef();
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
