
# include <sdl_core/SdlWidget.hh>
# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    SelectorLayout::SelectorLayout(const std::string& name,
                                   core::SdlWidget* widget,
                                   const float& margin):
      core::Layout(name, widget, margin),
      m_activeItem(-1),
      m_idsToPosition()
    {
      // Nothing to do.
    }

    SelectorLayout::~SelectorLayout() {}

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

    void
    SelectorLayout::switchToNext() {
      // If no items are registered in the layout, return early.
      if (empty()) {
        return;
      }

      // If no active item are set, assign the first one.
      if (m_activeItem < 0) {
        setActiveItem(0);
      }

      // In any other case, activate the next item and loop to
      // the first if needed.
      setActiveItem((m_activeItem + 1) % getItemsCount());
    }

    void
    SelectorLayout::computeGeometry(const utils::Boxf& window) {
      // Check whether a child is active.
      if (m_activeItem < 0) {
        return;
      }

      // Retrieve the realID of the desired active item from the
      // internal array.
      const int realID = m_idsToPosition[m_activeItem];

      // Disable other items.
      std::vector<bool> visible(getItemsCount(), false);
      visible[realID] = true;
      assignVisibilityStatus(visible);

      // Compute the available space for the active child.
      const utils::Sizef componentSize = computeAvailableSize(window);

      // Compute item's properties.
      std::vector<WidgetInfo> itemsInfo = computeItemsInfo();

      // Assign the space for the active child: as this is the only
      // child, use all the available space.
      std::vector<utils::Boxf> bboxes(getItemsCount(), utils::Boxf());

      // Assign the maximum size for this item based on its internal
      // size policy. We also account for the offset to apply in case
      // the size does not occupy fully the available space.
      utils::Sizef area = computeSizeFromPolicy(bboxes[realID], componentSize, itemsInfo[realID]);

      if (!area.compareWithTolerance(componentSize, 0.5f)) {
        log(
          std::string("Could only achieve size of ") + area.toString() +
          " but available space is " + componentSize.toString(),
          utils::Level::Error
        );
      }

      const float x = getMargin().w() + (componentSize.w() - area.w()) / 2.0f;
      const float y = getMargin().h() + (componentSize.h() - area.h()) / 2.0f;

      bboxes[realID] = utils::Boxf(x, y, area);

      // Use the base handler to assign bbox.
      assignRenderingAreas(bboxes, window);
    }

    bool
    SelectorLayout::onIndexRemoved(const int logicID,
                                   const int /*physID*/)
    {
      log("Removing item " + std::to_string(logicID) + " from selector layout");

      // Now update the local information by removing the input item from the internal
      // table. We basically copy all the information except for the deleted item.
      // Note that the internal `m_idsToPosition` will be left unchanged for values
      // smaller than `rmLogicID` and shifted by one for value larger than that.
      IdToPosition newIDs(m_idsToPosition.size() - 1);
      for (int id = 0 ; id < static_cast<int>(m_idsToPosition.size()) ; ++id) {
        if (id < logicID) {
          newIDs[id] = m_idsToPosition[id];
        }
        else if (id == logicID) {
          // Ignore this item as it will be deleted.
        }
        else {
          newIDs[id - 1] = m_idsToPosition[id];
        }
      }

      // Swap with the internal array.
      m_idsToPosition.swap(newIDs);

      // Now we need to handle the active item. Note that we do not rely on the
      // input real ID `item` but rather on the corresponding logical id fetched
      // from the intial version of the internal association array.
      // If no active item is set, we're all
      // good.
      // If the active item points to an item before the removed one, we're all good
      // as well.
      // Only in the case of an active item larger or equal to the removed one we need
      // to try to maintain some kind of order: to do so we will select the same item
      // based on its index.
      if (m_activeItem < logicID) {
        // No need to rebuild the layout, the active item has not been modified.
        return false;
      }

      // If there's no more item, do not bother.
      if (getItemsCount() == 0) {
        m_activeItem = -1;
        return false;
      }

      // We need to update the active item to be one less that initially: this will
      // account for the deletion of the input `item`.
      // Note that as we removed the item, the items count is now accurate.
      const int newActive = (m_activeItem - 1 + getItemsCount()) % getItemsCount();
      setActiveItem(newActive);

      // Update the layout as an item has been removed.
      return true;
    }

    void
    SelectorLayout::handleItemInsertion(core::LayoutItem* item,
                                        const int& logicalID,
                                        const int& realID)
    {
      // Return early if some basic assumptions are not verified.
      if (item == nullptr) {
        return;
      }

      // Clamp the input `logicalID` so that it fits nicely in the
      // bounds provided by this item. This also allows for _safe_
      // insertion by specifying either a negative value as `logicalID`
      // (in which case the item will be inserted in the first position)
      // or a value larger than the items count (in which case the
      // item will be inserted after the last element).
      // We're much less resilient for the `realID` though.

      if (realID < 0 || realID >= getItemsCount()) {
        error(
          std::string("Could not handle insertion of item \"") + item->getName() + " at index " +
          std::to_string(logicalID),
          std::string("Invalid ID returned by layout (id: ") + std::to_string(realID) + ")"
        );
      }

      // Don't forget to subtract one from the `items count` as at this step we
      // already inserted the new item.
      int logicID = std::max(0, std::min(logicalID, getItemsCount() - 1));

      // We now need to update the internal `m_idsToPosition` array. This array
      // contains for each logical id the real position of the item. Of course
      // the insertion of the item `logicalID` might disrupt the existing values
      // so we need to account for that.
      // To do so we need to traverse the existing internal array and update each
      // logical id greater than the input `logicalID`.

      // Update logical ids.
      IdToPosition newIDs(m_idsToPosition.size() + 1);
      for (int id = 0 ; id < static_cast<int>(m_idsToPosition.size()) ; ++id) {
        if (id < logicID) {
          // The item is before the `logicalID` in order: keep it at the same spot.
          newIDs[id] = m_idsToPosition[id];
        }
        else {
          // This item is after the newly inserted `logicalID`: move it back in the
          // list.
          newIDs[id + 1] = m_idsToPosition[id];
        }
      }

      // Assign the new `logicID`.
      newIDs[logicID] = realID;

      // Swap with the internal array.
      m_idsToPosition.swap(newIDs);

      // Now we need to handle automatic activation of the first item when it is
      // inserted.
      if (getItemsCount() == 1) {
        setActiveItem(logicID);
        return;
      }

      // Also handle the update of the `m_activeItem` if needed: basically if it
      // was already assigned and it was greater than the newly inserted item,
      // we need to update it.
      // Otherwise we do not.
      if (m_activeItem >= logicID) {
        // Theoretically we just need to modify the internal index: the item was
        // already activated and we will keep it active so we're good.
        ++m_activeItem;

        // However we need to make the newly inserted item hidden.
        item->setVisible(false);
      }
    }

  }
}
