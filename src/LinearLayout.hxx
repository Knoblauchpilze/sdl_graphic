#ifndef    LINEARLAYOUT_HXX
# define   LINEARLAYOUT_HXX

# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    int
    LinearLayout::addItem(core::LayoutItem* item) {
      // Use the specialized handler and assume that we add this item
      // at the end of the layout.
      return addItem(item, getItemsCount());
    }

    inline
    int
    LinearLayout::addItem(core::LayoutItem* item,
                          const int& index)
    {
      // We want to insert the `item` at logical position `index`. This
      // includes registering the item in the layout as usual but we also
      // need to register it into the internal `m_idsToPosition` table.
      //
      // Also we should update position of existing items so that we still
      // have consistent ids ranging from `0` all the way to `getItemsCount()`.

      // The first thing is to add the item using the base handler: this will
      // provide us a first index to work with.
      int id = Layout::addItem(item);

      // Check whether the insertion was successful.
      if (id < 0) {
        return id;
      }

      // At this point we know that the item could successfully be added to
      // the layout. We still need to account for its logical position
      // described by the input `index`.
      // We have three main cases:
      // 1. `index < 0` in which case we insert the `item` before the first
      //    element of the layout.
      // 2. `index >= size` in which case we insert the `item` after the
      //    last element of the layout.
      // 3. We insert the element in the middle of the layout.
      //
      // In all 3 cases we need to relabel the items which come after the
      // newly inserted item so that we keep some kind of consistency.

      // First, normalize the index: don't forget that the current size of
      // the layout *includes* the item we want to insert (because `addItem`
      // has already been called).
      int normalized = std::min(std::max(0, index), getItemsCount() - 1);

      // Update the label of existing items if it exceeds the new desired
      // logical index.
      for (unsigned id = 0u ; id < m_idsToPosition.size() ; ++id) {
        if (m_idsToPosition[id] >= normalized) {
          ++m_idsToPosition[id];
        }
      }

      // Now we have a valid set of labels with a hole at the position the
      // new `item` should be inserted: let's fix that.
      m_idsToPosition.insert(m_idsToPosition.cbegin() + normalized, id);

      // Return the value provided by the base handler.
      return id;
    }

    inline
    const core::Layout::Direction&
    LinearLayout::getDirection() const noexcept {
      return m_direction;
    }

    inline
    const float&
    LinearLayout::getComponentMargin() const noexcept {
      return m_componentMargin;
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
