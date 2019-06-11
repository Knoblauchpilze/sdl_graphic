
# include "LinearLayout.hh"

# include <unordered_set>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    LinearLayout::LinearLayout(const std::string& name,
                               core::SdlWidget* widget,
                               const Direction& direction,
                               const float& margin,
                               const float& interMargin):
      core::Layout(name, widget, margin),
      m_direction(direction),
      m_componentMargin(interMargin),
      m_idsToPosition()
    {
      // Nothing to do.
    }

    LinearLayout::~LinearLayout() {}

    void
    LinearLayout::computeGeometry(const utils::Boxf& window) {
      // The `LinearLayout` allows to arrange items using a flow along a
      // specified axis. The default behavior is to provide an equal allocation
      // of the available space to all items, but also to take into account
      // the provided stretch factors in order to obtain a nice growing/shrinking
      // behavior if needed.
      //
      // Items are stretched to use all the space in which the layout is not
      // flowing (i.e. vertical space for horizontal layout and horizontal space
      // for vertical layout) unless other indications are specified in the
      // item's size policy.

      // First, we need to compute the available size for this layout. We need
      // to take into account margins.
      const utils::Sizef internalSize = computeAvailableSize(window);

      // Copy the current size of items so that we can work with it without
      // requesting constantly information or setting information multiple times.
      std::vector<WidgetInfo> itemsInfo = computeItemsInfo();

      log(std::string("Available size: ") + std::to_string(window.w()) + "x" + std::to_string(window.h()), utils::Level::Notice);
      log(std::string("Internal size: ") + std::to_string(internalSize.w()) + "x" + std::to_string(internalSize.h()), utils::Level::Notice);

      std::vector<utils::Boxf> outputBoxes(getItemsCount());

      // We now have a working set of dimensions which we can begin to apply to items
      // in order to build the layout.
      // In case a item cannot be assigned the `defaultBox`, we update the two values
      // declared right now in order to keep track of additional space or missing space
      // for example in case the minimum/maximum size of a item prevent it from being
      // set to the `defaultBox`.
      // After updating all items, we need to loop again to apply the space we have
      // not used up before. This process continues until we run out of space to allocate
      // (usually meaning that we could expand some items to take the space not used
      // by others) or if no more container can be expanded or shrunk without bypassing
      // the sizes provided to bound the items.
      std::unordered_set<unsigned> itemsToAdjust;

      // In a first approach all the items can be adjusted.
      for (unsigned index = 0u ; index < itemsInfo.size() ; ++index) {
        if (itemsInfo[index].visible) {
          itemsToAdjust.insert(index);
        }
      }

      // Also assume that we didn't use up all the available space.
      utils::Sizef spaceToUse = internalSize;
      bool allSpaceUsed = false;
      utils::Sizef achievedSize;

      // Loop until no more items can be used to adjust the space needed or all the
      // available space has been used up.
      // TODO: Handle cases where the items are too large to fit into the layout ?
      while (!itemsToAdjust.empty() && !allSpaceUsed) {

        // Compute the amount of space we will try to allocate to each item still
        // available for adjustment.
        // The `defaultBox` is computed by dividing equally the remaining `spaceToUse`
        // among all the available items.
        const utils::Sizef defaultBox = computeDefaultItemBox(spaceToUse, itemsToAdjust.size());

        log(std::string("Default box is ") + defaultBox.toString(), utils::Level::Info);

        // Loop on all the items that can still be used to adjust the space used by
        // this layout and perform the size adjustements.
        for (std::unordered_set<unsigned>::const_iterator item = itemsToAdjust.cbegin() ;
             item != itemsToAdjust.cend() ;
             ++item)
        {
          // Try to assign the `defaultBox` to this item: we use a dedicated handler
          // to handle the case where the provided space is too large/small/not suited
          // to the item for some reasons, in which case the handler will provide a
          // size which can be applied to the item.
          utils::Sizef area = computeSizeFromPolicy(outputBoxes[*item], defaultBox, itemsInfo[*item]);
          outputBoxes[*item].w() = area.w();
          outputBoxes[*item].h() = area.h();

          log(std::string("Item \"") + getItemAt(*item)->getName() + "\" reach size " + outputBoxes[*item].toString());
        }

        // We have tried to apply the `defaultBox` to all the items. This might have fail
        // in some cases (for example due to a `Fixed` size policy for a item) and thus
        // we might end up with a total size for all the items different from the one desired
        // and expected when the `defaultBox` has been computed.
        // In order to fix things, we must compute the deviation from the expected size and
        // try to allocate the remaining space to other items (or remove the missing space
        // from items which can give up some).

        // Compute the total size of the bounding boxes.
        achievedSize = computeSizeOfItems(outputBoxes);

        // Check whether all the space have been used.
        if (achievedSize.fuzzyEqual(internalSize, 0.5f)) {
          // We used up all the available space, no more adjustments to perform.
          allSpaceUsed = true;
          continue;
        }

        // All space has not been used. Update the relevant `spaceToUse` in order to perform
        // the next iteration.
        spaceToUse = computeSpaceAdjustmentNeeded(achievedSize, internalSize);

        // Determine the policy to apply based on the achieved size.
        const core::SizePolicy action = shrinkOrGrow(internalSize, achievedSize, 0.5f);

        log(std::string("Desired ") + window.toString() + ", achieved: " + achievedSize.toString() + ", space: " + spaceToUse.toString(), utils::Level::Info);

        // We now know what should be done to make the `achievedSize` closer to `desiredSize`.
        // Based on the `policy` provided by the base class method, we can now determine which
        // item should be used to perform the needed adjustments.
        std::unordered_set<unsigned> itemsToUse;
        for (unsigned index = 0u ; index < itemsInfo.size() ; ++index) {
          // Check whether this item can be used to grow/shrink.
          std::pair<bool, bool> usable = canBeUsedTo(getItemAt(index)->getName(), itemsInfo[index], outputBoxes[index], action);
          if ((usable.first && getDirection() == Direction::Horizontal) ||
              (usable.second && getDirection() == Direction::Vertical))
          {
            log("Item " + getItemAt(index)->getName() + " can be used to " + action.toString());
            itemsToUse.insert(index);
          }
        }

        // There's one more thing to determine: the `Expanding` flag on any item's policy should
        // mark it as priority over other items. For example if two items can grow, one having
        // the flag `Grow` and the other the `Expand` flag, we should make priority for the one
        // with `Expanding` flag.
        // Items with `Grow` flag will only grow when all `Expanding` items have been maxed out.
        // Of course this does not apply in case items should be shrunk: all items are treated
        // equally in this case and there's not preferred items to shrink.
        if ((getDirection() == Direction::Horizontal && action.canExtendHorizontally()) ||
            (getDirection() == Direction::Vertical && action.canExtendVertically()))
        {
          // Select only `Expanding` item if any.
          std::unordered_set<unsigned> itemsToExpand;

          for (std::unordered_set<unsigned>::const_iterator item = itemsToUse.cbegin() ;
               item != itemsToUse.cend() ;
               ++item)
          {
            // Check whether this item can expand.
            if (getDirection() == Direction::Horizontal && itemsInfo[*item].policy.canExpandHorizontally()) {
              std::cout << "[LAY] " << getItemAt(*item)->getName() << " can be expanded horizontally" << std::endl;
              itemsToExpand.insert(*item);
            }
            if (getDirection() == Direction::Vertical && itemsInfo[*item].policy.canExpandVertically()) {
              std::cout << "[LAY] " << getItemAt(*item)->getName() << " can be expanded vertically" << std::endl;
              itemsToExpand.insert(*item);
            }
          }

          std::cout << "[LAY] Saved " << itemsToExpand.size() << " which can expand compared to "
                    << itemsToUse.size() << " which can extend"
                    << std::endl;
          // Check whether we could select at least one item to expand: if this is not the
          // case we can proceed to extend the items with only a `Grow` flag.
          if (!itemsToExpand.empty()) {
            itemsToUse.swap(itemsToExpand);
          }
        }


        // Use the computed list of items to perform the next action in order
        // to reach the desired space.
        itemsToAdjust.swap(itemsToUse);
      }

      if (!allSpaceUsed) {
        log(
          std::string("Could only achieve size of ") + achievedSize.toString() +
          " but available space is " + window.toString(),
          utils::Level::Error
        );
      }

      // All items have suited dimensions, we can now handle the position of each
      // item. We basically just move each item side by side based on their
      // dimensions and adding margins.
      float x = getMargin().w();
      float y = getMargin().h();

      for (int index = 0u ; index < getItemsCount() ; ++index) {
        // Position the item based on the position of the previous ones.
        // In addition to this mechanism, we should handle some kind of
        // centering to allow items with sizes smaller than the provided
        // layout's dimensions to still be nicely displayed in the center
        // of the layout.
        // To handle this case we check whether the dimensions of the size
        // of the item is smaller than the dimension stored in `internalSize`
        // in which case we can center it.
        // The centering only takes place in the perpendicular direction of
        // the flow of the layout (e.g. vertical direction for horizontal
        // layout and horizontal direction for vertical layout).
        float xItem = x;
        float yItem = y;

        if (getDirection() == Direction::Horizontal && outputBoxes[index].h() < internalSize.h()) {
          yItem += ((internalSize.h() - outputBoxes[index].h()) / 2.0f);
        }
        if (getDirection() == Direction::Vertical && outputBoxes[index].w() < internalSize.w()) {
          xItem += ((internalSize.w() - outputBoxes[index].w()) / 2.0f);
        }

        outputBoxes[index].x() = xItem;
        outputBoxes[index].y() = yItem;

        // Update the position for the next item based on the layout's
        // direction.
        if (getDirection() == Direction::Horizontal) {
          x += (outputBoxes[index].w() + m_componentMargin);
        }
        else if (getDirection() == Direction::Vertical) {
          y += (outputBoxes[index].h() + m_componentMargin);
        }
        else {
          error(std::string("Unknown direction when updating linear layout"));
        }
      }

      for (unsigned index = 0u ; index < outputBoxes.size() ; ++index) {
        std::cout << "[WIG] Item \"" << getItemAt(index)->getName() << "\" has: "
                  << outputBoxes[index].x() << ", " << outputBoxes[index].y()
                  << ", dims: "
                  << outputBoxes[index].w() << ", " << outputBoxes[index].h()
                  << std::endl;
      }

      // Assign the rendering area to items.
      assignRenderingAreas(outputBoxes, window);
    }

    void
    LinearLayout::removeItemFromIndex(int item) {
      // While removing the item, we need to make sure that the internal table
      // indicating the logical position of the item stays consistent.
      // To do so, we need to perform an update of the internal table based on
      // the new real position of the items.
      // Basically let's consider the following situation:
      //
      // m_idsToPosition[0] = 1
      // m_idsToPosition[1] = 0
      // m_idsToPosition[2] = 2
      // m_idsToPosition[3] = 4
      // m_idsToPosition[4] = 3
      //
      // We want to remove the item `2`. Note that this will not correspond to
      // the real item at position `2` but rather to the item at *logical* id
      // `2`.
      // If we simply remove the item from the internal table we will get the
      // following array:
      //
      // m_idsToPosition[0] = 1
      // m_idsToPosition[1] = 0
      // m_idsToPosition[3] = 4
      // m_idsToPosition[4] = 3
      //
      // It is not hard to see that we have a problem: the real index `3` or `4`
      // do not match any widget anymore and the next time we will try to assign
      // positions to these, we will face trouble.
      // The solution is to rely on the fact that the `Layout` class will perform
      // a collapse of the remaining items with a position larger than the one we
      // want to erase. So we could just decrease by one the real index of all the
      // items which had an id greater than the index of the item we just removed.
      // Doing this we would end up with the following:
      //
      // m_idsToPosition[0] = 1
      // m_idsToPosition[1] = 0
      // m_idsToPosition[2] = 3
      // m_idsToPosition[3] = 2
      //
      // Which is now correct.
      // And we also have to remove the corresponding logical entry in the internal
      // table before considering ourselves done.

      // Handle the case where the `item` is not valid.
      if (!isValidIndex(item)) {
        // Let the base method handle that.
        core::Layout::removeItemFromIndex(item);
        return;
      }

      // Now update the real indices of the widgets with a real index greater than
      // the one we removed.
      // In the meantime we can try tofind the logical id of the item which has just
      // been removed.

      // Find the logical item corresponding to the real id `item`.
      int rmLogicID = -1;
      for (int id = 0u ; id < static_cast<int>(m_idsToPosition.size()) ; ++id) {
        if (m_idsToPosition[id] == item) {
          rmLogicID = id;
        }
      }

      if (rmLogicID < 0) {
        error(
          std::string("Could not remove item ") + std::to_string(item) + " from layout",
          std::string("No such item")
        );
      }

      // The item exactly at position `item` should be ignored.
      IdToPosition newIDs(m_idsToPosition.size() - 1);
      for (int id = 0u ; id < static_cast<int>(m_idsToPosition.size()) ; ++id) {
        if (id < rmLogicID) {
          newIDs[id] = m_idsToPosition[id];
        }
        else if (id == rmLogicID) {
          // Ignore this item as it will be deleted.
        }
        else {
          newIDs[id - 1] = m_idsToPosition[id];
        }
      }

      // Swap with the internal array.
      m_idsToPosition.swap(newIDs);
    }

    utils::Sizef
    LinearLayout::computeSizeOfItems(const std::vector<utils::Boxf>& boxes) const {
      float flowingSize = 0.0f;
      float perpendicularSize = 0.0f;

      for (unsigned index = 0u ; index < boxes.size() ; ++index) {
        float increment = 0.0f;
        float size = 0.0f;

        if (getDirection() == Direction::Horizontal) {
          // The `increment` is given by the width of the box while the
          // `size` is given by its height.
          size = boxes[index].h();
          increment = boxes[index].w();
        }
        else if (getDirection() == Direction::Vertical) {
          // The `increment` is given by the height of the box while the
          // `size` is given by its width.
          size = boxes[index].w();
          increment = boxes[index].h();
        }
        else {
          error(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
        }

        // Increase the `flowingSize` with the provided `increment` and
        // perform a comparison of the size of the item in the other
        // direction (i.e. not in the direction of the flow) against the
        // current maximum and update it if needed.
        flowingSize += increment;
        if (perpendicularSize < size) {
          perpendicularSize = size;
        }
      }

      // Create a valid size based on this layout's direction.
      if (getDirection() == Direction::Horizontal) {
        return utils::Sizef(flowingSize, perpendicularSize);
      }
      else if (getDirection() == Direction::Vertical) {
        return utils::Sizef(perpendicularSize, flowingSize);
      }
      else {
        error(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
      }

      // Return dummy value because the `error` statement already handles throwing.
      return utils::Sizef();
    }

  }
}
