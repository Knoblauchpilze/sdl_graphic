
# include "GridLayout.hh"

# include <iomanip>
# include <unordered_set>
# include <sdl_core/SdlWidget.hh>

# include <iostream>

namespace sdl {
  namespace graphic {

    GridLayout::GridLayout(const std::string& name,
                           core::SdlWidget* widget,
                           unsigned columns,
                           unsigned rows,
                           float margin):
      core::Layout(name, widget, margin),
      m_columns(columns),
      m_rows(rows),

      m_columnsInfo(),
      m_rowsInfo(),

      m_locations()
    {
      // Build default information for columns/rows.
      resetGridInfo();
    }

    GridLayout::~GridLayout() {}

    void
    GridLayout::computeGeometry(const utils::Boxf& window) {
      // The `GridLayout` allows to arrange items using across a virtual
      // grid composed of `m_columns` columns and `m_rows` rows. The default
      // behavior is to provide an equal allocation of the available space
      // to all the items, but also to take into account the provided
      // information about items' preferred size and bounds.
      //
      // The process includes adjusting rows and columns by validating that
      // the final dimensions do correspond to the criteria applied to all
      // the items registered for a single column/row.

      // First, we need to compute the available size for this layout. We need
      // to take into account margins.
      const utils::Sizef internalSize = computeAvailableSize(window);

      // Compute default columns and rows dimensions.
      std::vector<CellInfo> cells = computeCellsInfo();

      // Copy the current size of items so that we can work with it without
      // requesting constantly information or setting information multiple times.
      std::vector<WidgetInfo> itemsInfo = computeItemsInfo();

      // Once this is done, we can start applying specific behavior to this layout.
      // The first thing we want to do is handling the minimum column width and
      // minimum row height attributes. These are specified on a per column/row
      // basis and should override the minimum hint provided by the items if it
      // is allowed by the constraints.
      // Typically let's figure some items with the following properties:
      //
      // Item 1:
      //  Min  : [100, 100]
      //  Hint : [150, 150]
      //  Max  : [200, 200]
      //
      // Item 2:
      //  Min  : [300, 100]
      //  Hint : [undefined]
      //  Max  : [undefined]
      //
      // Item 3:
      //  Min  : [110, 110]
      //  Hint : [120, 120]
      //  Max  : [150, 150]
      //
      // Item 4:
      //  Min  : [50, 50]
      //  Hint : [60, 60]
      //  Max  : [90, 90]
      //
      // All three items are supposed to land in a column with a minimum column
      // width of 130.
      // Let's also assume that the total size of the item allows to fit the items
      // without problem. We will not details the optimization process and consider that
      // the first assigned size is the final one.
      //
      // The first item will see its minimum size raised (please not that it will only
      // affect the computations inside this layout and not the actual minimum size of
      // this item) to [130, 130] and be assigned its hint anyway (of [150, 150]).
      //
      // The second item already has a minimum size larger than the minimum column
      // width so nothing will happen and it will be assigned a size based on the layout
      // preferred size (as no hint is provided).
      //
      // The third item has a minimum size smaller than the minimum column size so the
      // layout will try to raise the minimum size to [130, 130]. From there, we see that
      // it conflicts with the provided hint so there's too main case:
      // 1) The item's policy does allow to grow in which case the minimum size and the
      //    hint will be set to [130, 130].
      // 2) The item's policy does not allow to grow (which would be weird considered
      //    that the item has a maximum size) in which case the minimum size will be
      //    raised to [120, 120].
      //    The rest of the items though will still try to use [130, 130].
      //
      // The fourth item has a maximum size smaller than the provided minimum column
      // width. The logic will be the same as for the third item, except that the
      // maximum size will constraint the application of the minimum column width and
      // thus the maximum reachable size will be [90, 90] (if the policy allows to grow).
      //
      // We handle these considerations before starting the optimization process as it
      // will directly impact it.
      adjustItemToConstraints(internalSize, itemsInfo);

      log(std::string("Available size: ") + std::to_string(window.w()) + "x" + std::to_string(window.h()), utils::Level::Notice);
      log(std::string("Internal size: ") + std::to_string(internalSize.w()) + "x" + std::to_string(internalSize.h()), utils::Level::Notice);

      // We now have a working set of dimensions which we can begin to apply to items
      // in order to build the layout.
      // The specification of the dimensions is divided into two main phases: first the
      // determination of a base dimension for each column and row based on single-cell
      // item, and then an adjustment phase where multi-cells items are placed.
      // In the first phase, the process is divided into two parts: the adjustment of
      // rows and columns is indeed completely independent and can be processed with no
      // particular ordering.
      // This allows for simpler algorithms.
      // In each case, the process is similar: we start by assuming an ideal size of the
      // item for each column/row and try to work with this. In case some item cannot
      // take full advantage of this size (because their constraints forbid it) we
      // allocate the remaining space between remaining items until the achieved size
      // is close enough from the target size.
      // If the items' constraints do not allow for a perfect repartition, we stop the
      // process and produce the best possible solution given the constraints.
      // Once all single-cell items have been laid out, we can try to further adjust
      // the resulting distribution to multi-cell items. At this point, the best case
      // scenario is that the items will take advantage of the computed repartition
      // and that no other adjustment will occur. This is rarely the case though and
      // we might have to redo an adjustment for single-cell items afterwards.

      // Proceed to adjust the columns' width.
      log(std::string("Adjusting columns width"), utils::Level::Notice);
      std::vector<float> columnsDims = adjustColumnsWidth(internalSize, itemsInfo, cells);

      // Adjust rows' height.
      log(std::string("Adjusting rows height"), utils::Level::Notice);
      std::vector<float> rowsDims = adjustRowHeight(internalSize, itemsInfo, cells);

      // Adjust multi-cell item to make them span the columns/rows they are spanning.
      // When shrinking the item we might indeed shrink too much some items which
      // creates some weird distribution where a multi-cell is smaller than a single cell
      // just because it was able to get one more shrinking iteration.
      log(std::string("Adjusting multi-cell width"), utils::Level::Notice);
      adjustMultiCellWidth(columnsDims, itemsInfo, cells);

      log(std::string("Adjusting multi-cell height"), utils::Level::Notice);
      adjustMultiCellHeight(rowsDims, itemsInfo, cells);

      // All items have suited dimensions, we can now handle the position of each
      // item. We basically just move each item based on the dimensions of the
      // rows and columns to reach the position of a specified item.
      std::vector<utils::Boxf> outputBoxes(getItemsCount());

      for (int index = 0u ; index < getItemsCount() ; ++index) {
        // Position the item based on the dimensions of the rows and columns
        // until the position of the item.
        // We maintained a vector to keep track of the dimensions of each row
        // and column during the adjustment process so that we can use it now
        // to assign a position to the boxes.
        // In addition to this mechanism, we should handle some kind of
        // centering to allow items with sizes smaller than the provided
        // layout's dimensions to still be nicely displayed in the center
        // of the layout.
        // To handle this case we check whether the dimensions of the size
        // of the item is smaller than the dimension stored in `internalSize`
        // in which case we can center it.
        // The centering takes place according to both the dimensions of the
        // rows and columns spanned by the item.

        // Retrieve the item's location.
        const LocationsMap::const_iterator loc = m_locations.find(index);
        if (loc == m_locations.end()) {
          error(
            std::string("Could not retrieve information for item \"") +
            getItemAt(index)->getName() + "\" while updating grid layout"
          );
        }

        float xItem = getMargin().w();
        float yItem = getMargin().h();

        // Compute the offset to apply to reach the desired column based on the
        // item's location.
        for (unsigned column = 0u ; column < loc->second.x ; ++column) {
          xItem += columnsDims[column];
        }
        for (unsigned row = 0u ; row < loc->second.y ; ++row) {
          yItem += rowsDims[row];
        }

        // Handle the centering of the item in case it is smaller than the
        // desired width or height.
        // To do so, compute the size the item _should_ have based on its
        // columns/rows span.
        float expectedWidth = 0.0f;
        float expectedHeight = 0.0f;

        for (unsigned column = loc->second.x ; column < loc->second.x + loc->second.w ; ++column) {
          expectedWidth += columnsDims[column];
        }
        for (unsigned row = loc->second.y ; row < loc->second.y + loc->second.h ; ++row) {
          expectedHeight += rowsDims[row];
        }

        if (cells[index].box.w() < expectedWidth) {
          xItem += ((expectedWidth - cells[index].box.w()) / 2.0f);
        }
        if (cells[index].box.h() < expectedHeight) {
          yItem += ((expectedHeight - cells[index].box.h()) / 2.0f);
        }

        outputBoxes[index] = utils::Boxf(
          xItem, yItem,
          cells[index].box.w(), cells[index].box.h()
        );
      }

      // Assign the rendering area to items.
      assignRenderingAreas(outputBoxes, window);
    }

    bool
    GridLayout::onIndexRemoved(int /*logicID*/,
                               int /*physID*/)
    {
      // We need to update the local information about items. This means basically updating the
      // `m_locations` attribute. In order to do so, we need to rely on some invariant properties
      // of the item which have been updated. We will use the address in order to maintain some
      // consistency between items.

      // So first copy the internal locations table so that we can build a new one right away.
      LocationsMap old;
      old.swap(m_locations);

      // Traverse the old locations and try to build the new table.
      for (LocationsMap::const_iterator oldItem = old.cbegin() ;
           oldItem != old.cend() ;
           ++oldItem)
      {
        // Try to find the index of this item.
        const int newID = getIndexOf(oldItem->second.item);

        // From there, we have two main cases: either the item still exists in the layout,
        // or it doesn't. This is defined by the fact that the `newID` is positive or negative.
        // If the item still exists, we need to update the information contained in the
        // locations map so that further update of the layout yields correct results. If the
        // item does not exist anymore in the layout, we have to ignore this item and not
        // add it to the new locations map.

        // Insert the item with its new information only if it still exists in the layout.
        if (isValidIndex(newID)) {
          m_locations[newID] = oldItem->second;
        }
      }

      // The layout need to be rebuilt.
      return true;
    }

    void
    GridLayout::updateGridCoordinates(int item,
                                      const utils::Boxi& coordinates)
    {
      // Try to retrieve the desired item.
      LocationsMap::iterator itemToUpdate = m_locations.find(item);

      if (itemToUpdate == m_locations.end()) {
        error(
          std::string("Could not update grid coordinates for item ") + std::to_string(item),
          std::string("Item not found")
        );
      }

      itemToUpdate->second.x = coordinates.x();
      itemToUpdate->second.y = coordinates.y();
      itemToUpdate->second.w = coordinates.w();
      itemToUpdate->second.h = coordinates.h();
    }

    std::vector<GridLayout::CellInfo>
    GridLayout::computeCellsInfo() const noexcept {
      // Allocate vector where all cells are by default empty (no stretch,
      // no dimensions and no associated item).
      // Note that we create as many cells as items, and not a single cell
      // per element of the global `m_columns * m_rows` cells defined by the
      // layout.
      // This does not allow exhaustive traversal of all the cells of the
      // layout but it allows for efficient mapping between a item ID and
      // the corresponding cell information.
      std::vector<CellInfo> cells(
        getItemsCount(),
        CellInfo{
          0,
          0,
          utils::Boxf(),
          false,
          -1
        }
      );

      // Complete the information with items' data: if a cell contains
      // a item, fill in the corresponding stretches and associate it
      // with the item's identifier. Note that a cell is said to `contain`
      // a item as long as it is in the area used by the item. This
      // means that mult-cell items will be assigned several entries in
      // the vector produced by this function.

      // Traverse each item's location information and update the relevant
      // information.
      for (LocationsMap::const_iterator item = m_locations.cbegin() ;
           item != m_locations.cend() ;
           ++item)
      {
        cells[item->first].hStretch = m_columnsInfo[item->second.x].stretch;
        cells[item->first].vStretch = m_rowsInfo[item->second.y].stretch;
        cells[item->first].box = utils::Boxf();
        cells[item->first].multiCell = (item->second.w > 1) || (item->second.h > 1);
        cells[item->first].item = item->first;
      }

      // Return the built-in vector.
      return cells;
    }

    void
    GridLayout::adjustItemToConstraints(const utils::Sizef& /*window*/,
                                        std::vector<WidgetInfo>& items) const noexcept
    {
      // The aim of this function is to provide for the minimum column width and minimum
      // row height adjustments needed to override the properties of the items.
      // This is not as simple as it is because we cannot just override the minimum size
      // of all items with the provided size based on their columns and rows: indeed
      // this might conflict with the size hint or maximum size provided.
      // We can only grow it as much as possible without overriding other attributes.

      // Traverse each item and update the relevant constraints.
      for (unsigned item = 0u ; item < items.size() ; ++item) {
        // Retrieve the location information for this item.
        const LocationsMap::const_iterator info = m_locations.find(item);
        if (info == m_locations.cend()) {
          error(
            std::string("Could not adjust item ") + std::to_string(item) + " to minimum constraints",
            std::string("Inexisting item")
          );
        }

        const ItemInfo& loc = info->second;

        // Compute the minimum dimensions of this item based on its location.
        utils::Sizef desiredMin;

        for (unsigned row = loc.y ; row < loc.y + loc.h ; ++row) {
          for (unsigned column = loc.x ; column < loc.x + loc.w ; ++column) {
            desiredMin.w() += m_columnsInfo[column].min;
          }
          desiredMin.h() += m_rowsInfo[row].min;
        }

        // We computed the minimum size for this item from the internal constraints. We have to
        // check whether this conflicts with some property of the item. The best case scenario
        // is that this size is already smaller than the provided minimum size in which case there's
        // nothing to be done.
        // Otherwise, we want to check whether we can increase the minimum size of the item
        // without breaking other constraints (like size hint or maximum size).
        // Whether we should consider the size hint (if provided) or the maximum size is based on the
        // policy for this item: however we cannot really assume anything except to use the size
        // hint. Modifying this behavior would require to search for other items and check whether
        // some other items can expand. This does not even include considering whether the expanding
        // process would even occur in the first place because we have no idea of the total size of
        // the items. So let's keep this simple right now and just focus on whether the size hint
        // is provided or not.
        // As usual we will process separately width and height to keep it simple.
        utils::Sizef& min = items[item].min;
        const utils::Sizef& hint = items[item].hint;
        const utils::Sizef& max = items[item].max;
        const core::SizePolicy& policy = items[item].policy;

        // Assume we can assign the desired minimum width for this item.
        float adjustedMinWidth = desiredMin.w();

        // Check whether it conflicts with the provided minimum size if any.
        // Basically we have the three following scenario:
        // 1) The min size is valid and:
        // 1.a) The min size is smaller than the desired size: keep the min size.
        // 1.b) The min size is equal to the desired size: keep the min size.
        // 1.c) The min size is larger than the desired size: keep the min size.
        // 2) The min size is not valid (no requirements) so we can keep the desired
        //    size.
        // We see that the only relevant check is to see whether a minimum size is
        // provided.
        if (min.isValid()) {
          adjustedMinWidth = min.w();
        }

        // Check whether the size hint conflicts with the desired minimum width.
        // These are the possible scenarii:
        // 1) The size hint is valid and:
        // 1.a) The size hint is smaller than the desired size: check with the policy to
        //      determine whether we can grow.
        // 1.b) The size hint is equal to the desired size: keep the desired size.
        // 1.c) The size hint is larger than the desired size: keep the size hint.
        // 2) The size hint is not valid (no requirements) so we can keep the desired
        //    size.
        if (hint.isValid()) {
          // Chewk whether the size hint is larger than the desired width.
          if (adjustedMinWidth < hint.w()) {
            // Override with the size hint.
            adjustedMinWidth = hint.w();
          }

          // The size hint is smaller than the desired size: check whether the policy allows
          // to grow.
          if (hint.w() < adjustedMinWidth && !policy.canExtendHorizontally()) {
            // Override with the size hint as the policy does not allow to grow.
            adjustedMinWidth = hint.w();
          }
        }

        // Check whether the maximum size conflicts with the desired minimum width.
        // At this point we already checked that the minimum size allowed for such a
        // width (otherwise it would have been clamped already). Same goes for the size
        // hint (which is right above) so we know that from these two constraints
        // standpoint the size is valid. The only remaining problem might be a size
        // too large compared to the maximum size.
        // Below are listed the possible scenarii:
        // 1) The max size is valid and:
        // 1.a) The max size is smaller than the desired size: keep the max size.
        // 1.b) The max size is equal to the desired size: keep the desired size.
        // 1.c) The max size is larger than the desired size: keep the desired size.
        // 2) The max size is not valid (no requirements) so we can keep the desired
        //    size.
        if (max.isValid() && max.w() < adjustedMinWidth) {
          // Override with the maximum size.
          adjustedMinWidth = max.w();
        }

        // Now proceed to adjustment for the height of the item: similar reasoning can be
        // applied to width so we will not duplicate information here.
        float adjustedMinHeight = desiredMin.h();

        if (min.isValid()) {
          adjustedMinHeight = min.h();
        }

        if (hint.isValid()) {
          // Chewk whether the size hint is larger than the desired height.
          if (adjustedMinHeight < hint.h()) {
            // Override with the size hint.
            adjustedMinHeight = hint.h();
          }

          // The size hint is smaller than the desired size: check whether the policy
          // allows to grow.
          if (hint.h() < adjustedMinHeight && !policy.canExtendVertically()) {
            // Override with the size hint as the policy does not allow to grow.
            adjustedMinHeight = hint.h();
          }
        }

        // Check whether the maximum size conflicts with the desired minimum width.
        if (max.isValid() && max.h() < adjustedMinHeight) {
          adjustedMinHeight = max.h();
        }

        // Assign the new min size for this item.
        min.w() = adjustedMinWidth;
        min.h() = adjustedMinHeight;
      }

    }

    utils::Sizef
    GridLayout::computeAchievedSize(const std::vector<ItemDataWrapper>& elements) const noexcept {
      // Assume empty dimensions.
      utils::Sizef achieved;

      // Traverse the input set of elements.
      for (unsigned element = 0u ; element < elements.size() ; ++element) {
        // Here, we want to determine the achieved size for the item based
        // on the input `elements`. The input vector contains data for each
        // item registered in a single column/row. This data may be related
        // to either a single-cell item or a multi-cell item.
        // In the first scenario, we have all the information about the item
        // available right away so there's no real tricks to hide some width
        // or height of a item: we can safely add the size of the item to
        // the achieved size.
        // The case o fmulti-cell item is a bit trickier: even if we can
        // compute the *total* size of a item, how can we easily determine
        // which size goes in which column/row ?
        // To solve this problem we added an information directly in the
        // `ItemData` structure which describes a size for each particular
        // item's data element. The optimization process ensures that the
        // sum of all individual item's data size is consistent with the
        // total size of the item and this allows us to easily distribute
        // the total size among columns/rows spanned by the item: it is just
        // the available value in each item's data element.
        // The interest of this system is that it supplants completely the
        // box system at least for the achieved size purpose as all the needed
        // information is directly available.

        if (elements[element].data->size.w() > achieved.w()) {
          achieved.w() = elements[element].data->size.w();
        }
        if (elements[element].data->size.h() > achieved.h()) {
          achieved.h() = elements[element].data->size.h();
        }
      }

      // Return the achieved size.
      return achieved;
    }

    std::vector<float>
    GridLayout::adjustColumnsWidth(const utils::Sizef& window,
                                   const std::vector<WidgetInfo>& items,
                                   std::vector<CellInfo>& cells) const
    {
      // This method needs to combine several constraints in order to converge to final
      // columns' dimensions:
      // 1) The user might have specified `minimum column width` through the dedicated
      //    handler.
      // 2) items might have policies describing minimum or maximum sizes.
      // 3) Several items in a single column might have conflicting expectations.
      //
      // The aim is thus to combine all these constraints in order to produce the final
      // columns' dimensions vector.
      // Note that only item spanning a single column are considered in this function
      // so that we get simpler algorithms.
      // Multi-cells items will be considered in a second time and will only further
      // adapt the dimensions produced by this function.

      // Start by creating the return value: in addition to the individual width for
      // each item this function return a global columns' dimensions vector where
      // the maximum width for each column is registered. This allows to easily
      // iterate over columns without needing to extract the largest item in each
      // one.
      std::vector<float> columns(m_columns, 0.0f);

      // Now, we need to retrieve for each column the list of items related to it:
      // this allows for quick access when iterating to determine columns' dimensions.
      // We also want to provide easy access to multi-cell item by inserting each
      // one of them in all the columns where it appear.
      // For that to happen we want to first initialize this map with empty vectors
      // and then dynamically populate it.

      std::unordered_map<unsigned, std::vector<ItemDataWrapper>> itemsForColumns;
      for (unsigned column = 0u ; column < m_columns ; ++column) {
        itemsForColumns[column] = std::vector<ItemDataWrapper>();
      }

      for (LocationsMap::const_iterator item = m_locations.cbegin() ;
            item != m_locations.cend() ;
            ++item)
      {
        // Insert the item in each column spanned by it.
        const ItemInfo& info = item->second;

        // Only if the item is visible.
        if (!items[item->first].visible) {
          continue;
        }

        for (unsigned column = 0u ; column < info.w ; ++column) {
          // Create the item's data.
          ItemDataShPtr data = std::make_shared<ItemData>(
            ItemData{
              item->first,
              info.w > 1,
              column == 0u,
              info.w,
              (info.y) * m_columns + info.x + column,
              utils::Sizef()
            }
          );

          // Insert a wrapper in the corresponding vector.
          itemsForColumns[info.x + column].push_back(
            ItemDataWrapper{
              data->id,
              data
            }
          );
        }
      }

      // There's a first part of the optimization process which should be handled
      // right away: the user is allowed to specify a minimum column width for any
      // column of the layout.
      // We can handle the columns where there's at least a item later on (it's
      // actually part of the optimization process) but here we should handle the
      // columns which do not contain any items. As no item is there to take
      // space such columns will be ignored by the optimization process.
      // In order to handle these nonetheless, we figured that the best way is to
      // simply subtract the required width from the available total `window`: this
      // makes sense as it would have been the width applied anyway because no
      // item is there to modify it so it would have been accepted right away.

      float widthForEmptyColumns = 0.0f;

      // Traverse the `itemsForColumns` map and search for columns with no items.
      for (std::unordered_map<unsigned, std::vector<ItemDataWrapper>>::const_iterator column = itemsForColumns.cbegin() ;
           column != itemsForColumns.cend() ;
           ++column)
      {
        // Search empty columns.
        if (column->second.empty()) {
          // Assign the minimum width to this column and retain the used space.
          columns[column->first] = m_columnsInfo[column->first].min;
          widthForEmptyColumns += columns[column->first];
        }
      }

      // Let's start the optimization process.
      // We start with an available space budget described by the value of `window.w()`.
      // We will first try to allocate fairly this space among all columns. Each column
      // will perform checks to determine whether the proposed dimension is valid based
      // on the criteria of each item registered for this column.
      // If this is the case the dimension will be adopted. If not, the dimension will
      // be adapted so that it fits all the constraingts for this column.
      // Once all the columns have been processed, we start all over again: the remaining
      // or missing space is computed, and we then try to allocate the difference among
      // all the columns.
      // This process continues until either all the space has been successfully allocated
      // or there's no more columns to use to adjust the size.
      // In order to make things easier, we use a little trick here. Indeed even though
      // we're reasoning with columns here, the individual unit inside a column remains
      // a item. Thus, when an adjustment should be performed for a column, it is
      // ultimately a item which should be adjusted. Also, during the process columns
      // are interchangeable which means that any operation can and will be applied
      // indistinctly on all the columns available for adjustments.
      // For all these reasons, it makes more sense to keep a list of items to adjust
      // instead of a list of columns: we avoid an indirection to fetch the item linked
      // to an identifier in a column.
      // We still keep the global `itemsForColumns` which allows to cherry-pick the
      // single-cell item for a given column, but the whole process is done in terms
      // of items.
      // A small drawbacks is that we need to know how many columns can still be used to
      // perform adjustment: indeed this is what defines how we will split fairly the
      // remanining space.

      std::unordered_set<ItemDataWrapper> itemsToAdjust;
      unsigned columnsRemaining = itemsForColumns.size();
      std::unordered_set<unsigned> emptyColumns;

      // In a first approach all the columns can be adjusted (except empty ones).
      for (unsigned column = 0u ; column < m_columns ; ++column) {
        if (itemsForColumns[column].empty()) {
          emptyColumns.insert(column);
          --columnsRemaining;
        }
        else {
          itemsToAdjust.insert(itemsForColumns[column].cbegin(), itemsForColumns[column].cend());
        }
      }

      // Also assume that we didn't use up all the available space. The remaining space is
      // the difference between the provided space from `window` minus the space used for
      // columns with no items (see two paragraphs above).
      float spaceToUse = window.w() - widthForEmptyColumns;
      bool allSpaceUsed = false;

      float achievedWidth = widthForEmptyColumns;

      // Loop until no more items can be used to adjust the space needed or all the
      // available space has been used up. As discussed, this is roughly equivalent to
      // checking whether a column is available for adjustment: we only avoid another
      // indirection to fetch the item when performing the adjustments.
      while (!itemsToAdjust.empty() && !allSpaceUsed) {

        // Compute the amount of space we will try to allocate to each column still
        // available for adjustment.
        // The `defaultWidth` is computed by dividing equally the remaining `spaceToUse`
        // among all the available columns.
        // The available columns for adjustment is retrieved using the `columnsRemaining`
        // variable and not directly the `itemsToAdjust.size()` as a single column can
        // have several items needing adjustments.
        const float defaultWidth = allocateFairly(spaceToUse, columnsRemaining);

        // Allocate this space on each item: as all columns are equivalent, this means
        // that we can directly work on individual items.
        for (std::unordered_set<ItemDataWrapper>::const_iterator dataWrapper = itemsToAdjust.cbegin() ;
             dataWrapper != itemsToAdjust.cend() ;
             ++dataWrapper)
        {
          const unsigned item = dataWrapper->data->item;

          // Try to assign the `defaultWidth` to this item: we use a dedicated handler
          // to handle the case where the provided space is too large/small/not suited
          // to the item for some reasons, in which case the handler will provide a
          // size which can be applied to the item.
          // The process is not completely straightforward though as we need to account
          // for items spanning multiple cells. We could either account for the span of
          // the item to multiply the `defaultWidth` or we can add as many instance of
          // the item as needed for a particular column. This second method has a lot
          // of advantages compared to the first one se we chose it.
          // This means that there's no need anymore to account for the item's span in
          // this function as each cell spanned by the item is handled individually
          // which allows to make the item grow more on columns which can account for it.

          // Apply the policy for this item.
          float width = computeWidthFromPolicy(cells[item].box, defaultWidth, items[item]);

          // We now need to distribute this width to the current `data`: in order to do
          // so, let's compute the size increase provided for this item by the current
          // column: this is the size which belongs to the column.
          dataWrapper->data->size.w() += (width - cells[item].box.w());

          // Now register the new size of the item.
          cells[item].box.w() = width;
        }

        // We have tried to apply the `defaultWidth` to all the remaining items available
        // for adjustments. This might have fail in some cases (for example due to a `Fixed`
        // size policy for a item in a given column) and thus we might end up with a total
        // size for all the item different from the one desired and expected when the
        // `defaultWidth` has been computed.
        // In order to fix things, we must compute the deviation from the expected size and
        // try to allocate the remaining space to other items (or remove the missing space
        // from items which can give up some).
        // For single-cell item, it is rather easy: the achieved size only contributes to
        // a single column.
        // For multi-cell items it is a bit different though: we need to distribute the
        // width across several columns: this is usually handled by the `size` variable inside
        // each item's data instance: the instance located in a each column contains
        // information for this column which should be okay as we ensure that it always stay
        // up to date with the total size (i.e. the sum of all individual instances matches
        // the global size).
        // We perform this operation after each optimization operation in order to guarantee
        // that the final value of the `rows` vector will be usable as a valid return value.
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          // Only handle non empty columns.
          if (emptyColumns.find(column) == emptyColumns.cend()) {
            columns[column] = computeAchievedSize(itemsForColumns[column]).w();
          }
        }

        // Compute the achieved size from consolidated dimensions.
        achievedWidth = 0.0f;
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          log(std::string("Column ") + std::to_string(column) + " has size " + std::to_string(columns[column]));
          achievedWidth += columns[column];
        }

        const utils::Sizef achievedSize(achievedWidth, window.h());

        // Check whether all the space have been used.
        if (achievedSize.compareWithTolerance(window, 1.0f)) {
          // We used up all the available space, no more adjustments to perform.
          allSpaceUsed = true;
          continue;
        }

        // All space has not been used. Update the relevant `spaceToUse` in order to perform
        // the next iteration.
        spaceToUse = computeSpaceAdjustmentNeeded(achievedSize, window).w();

        // Determine the policy to apply based on the achieved size.
        const core::SizePolicy action = shrinkOrGrow(window, achievedSize, 0.5f);

        // We now know what should be done to make the `achievedWidth` closer to `desiredWidth`.
        // Based on the `policy` provided by the base class method, we can now determine which
        // column should be used to perform the needed adjustments.
        // Even if in the end we will substitute the corresponding items to the individual
        // columns, right now we need to keep the columns.
        // This will prove useful in a second phase where we will try to make column which can
        // `Expand` have priority over column which can only `Grow`. This can only be done by
        // keeping the column's id: if we keep only items, we don't have any means (except
        // through exhaustive seach) to find the associated columns to a item. Thus we will
        // not be easily able to discard entire columns if it can only `Grow`.
        std::unordered_set<unsigned> columnsToUse;

        // Traverse each column to determine whether a item in this column can be used to
        // perform the required `action`.
        // Based on the `action`, the way we select columns is a bit different. If we need to
        // shrink some columns, we need all items inside a column to be shrinkable in order
        // for the column to be declared usable: indeed if only some items can be shrunk
        // it also means some cannot shrink and the overall size of the column will not be
        // modified even though the shrinkable item are shrunk.
        // If the action is meant to gorw the situation is a bit different though: as we still
        // have the possibility to center items which are smaller than the total width of
        // the column, a column can be grown as soon as a single item can be grown inside
        // it.
        // In order to allow for easy detection of columns which can be expanded, we also
        // keep the column's identifiers instead of substituting right away the corresponding
        // items. More details can be found in the next comment section.
        for (unsigned column = 0u ; column < itemsForColumns.size() ; ++column) {
          // Retrieve the items associated to this column.
          const std::vector<ItemDataWrapper>& itemsForColumn = itemsForColumns[column];

          // Distinguish based on the action. Furhtermore we are processing columns so
          // we only care about horizontal behavior.
          if (action.canExtendHorizontally()) {
            // Traverse the items for this column: if at least one can be used to `Grow`,
            // consider this column usable to perform the required action.
            for (unsigned item = 0u ; item < itemsForColumn.size() ; ++item) {
              // Compute the status of the item for this action.
              const unsigned itemID = itemsForColumn[item].data->item;

              std::pair<bool, bool> usable = canBeUsedTo(items[itemID], cells[itemID].box, action);
              if (usable.first) {
                // This column can be used to `Grow` thanks to this item. No need to go
                // further.
                columnsToUse.insert(column);
                break;
              }
            }
          }
          else if (action.canShrinkHorizontally()) {
            // Traverse the items for this column: all items need to be able to shrink
            // in order for this colum to be marked as shrinkable.

            // Assume this column can be shrunk.
            bool canShrink = true;

            for (unsigned item = 0u ; item < itemsForColumn.size() ; ++item) {
              // Compute the status of the item for this action.
              const unsigned itemID = itemsForColumn[item].data->item;

              std::pair<bool, bool> usable = canBeUsedTo(items[itemID], cells[itemID].box, action);
              if (!usable.first) {
                // As this item cannot shrink, it means that even if other items shrink
                // the width for this column will remain unchanged so we cannot use it. This
                // is the general idea but we can refine on it: the current item cannot
                // shrink given its current size but it does not mean that the column as a
                // whole cannot shrink. Indeed if the current achieved size of the column
                // is larger than the size required for this item we might still be able
                // to shrink the column without needing to reduce the size of this item.
                // Thus before setting the `canShrink` boolean to false, let's first check
                // whether the width of the column allows to shrink.
                if (columns[column] <= cells[itemID].box.w()) {
                  canShrink = false;
                  break;
                }
              }
            }

            // Register this column for shrinking if needed.
            if (canShrink) {
              columnsToUse.insert(column);
            }
          }
        }

        // We have a list of columns which can be used to perform the required `action`. There's
        // a last filtering to apply though: if the action requires to make some items larger, we need
        // to give priority to items which have the `Expand` flag over items having `Grow` flag.
        // The tricky part is that we're working on columns here so there's no simple way to assign a
        // global policy to a column.
        // In order to determine whether a column needs to `Expand`, we will check each individual item
        // registered for this column and if at least one has the corresponding flag we will assume that
        // the column as a whole can be `Expand`ed.
        // Of course this only applies if the `action` includes growing at all.
        if (action.canExtendHorizontally()) {
          // Select only `Expanding` columns if any.
          std::unordered_set<unsigned> columnsToExpand;

          for (std::unordered_set<unsigned>::const_iterator column = columnsToUse.cbegin() ;
               column != columnsToUse.cend() ;
               ++column)
          {
            // Check whether this column can expand: this is done by checking each registered
            // item in this column for the appropriate flag.
            const std::vector<ItemDataWrapper>& itemsForColumn = itemsForColumns[*column];

            for (unsigned item = 0u ; item < itemsForColumn.size() ; ++item) {
              // Only consider horizontal direction as we're processing columns.
              const unsigned itemID = itemsForColumn[item].data->item;

              if (items[itemID].policy.canExpandHorizontally()) {
                columnsToExpand.insert(*column);
                // No need to continue further, the column can be `Expand`ed.
                break;
              }
            }
          }

          // Check whether we could select at least one item to expand: if this is not the
          // case we can proceed to extend the item with only a `Grow` flag.
          if (!columnsToExpand.empty()) {
            columnsToUse.swap(columnsToExpand);
          }
        }

        // Update the remaining columns so that we can compute correctly the way to allocate space.
        columnsRemaining = columnsToUse.size();

        // We now have a working set of columns which can be used to perform the required `action` and we
        // took into consideration precedence of `Expand` flag over `Grow` flag. We now only need for each
        // column to substitute the corresponding items.
        std::unordered_set<ItemDataWrapper> itemsToUse;

        for (std::unordered_set<unsigned>::const_iterator column = columnsToUse.cbegin() ;
             column != columnsToUse.cend() ;
             ++column)
        {
          itemsToUse.insert(itemsForColumns[*column].cbegin(), itemsForColumns[*column].cend());
        }

        // Use the computed list of items to perform the next action in order
        // to reach the desired space.
        itemsToAdjust.swap(itemsToUse);
      }

      // Warn the user in case we could not use all the space.
      if (!allSpaceUsed) {
        log(
          std::string("Could only achieve width of ") + std::to_string(achievedWidth) +
          " but available space is " + std::to_string(window.w()),
          utils::Level::Error
        );
      }

      // Return the consolidated columns' dimensions vector.
      return columns;
    }

    std::vector<float>
    GridLayout::adjustRowHeight(const utils::Sizef& window,
                                const std::vector<WidgetInfo>& items,
                                std::vector<CellInfo>& cells) const
    {
      // This method needs to combine several constraints in order to converge to final
      // rows' dimensions:
      // 1) The user might have specified `minimum row height` through the dedicated
      //    handler.
      // 2) items might have policies describing minimum or maximum sizes.
      // 3) Several items in a single row might have conflicting expectations.
      //
      // The aim is thus to combine all these constraints in order to produce the final
      // rows' dimensions vector.
      // Note that only item spanning a single row are considered in this function
      // so that we get simpler algorithms.
      // Multi-cells items will be considered in a second time and will only further
      // adapt the dimensions produced by this function.

      // Start by creating the return value: in addition to the individual height for
      // each item this function return a global rows' dimensions vector where
      // the maximum height for each row is registered. This allows to easily
      // iterate over rows without needing to extract the largest item in each
      // one.
      std::vector<float> rows(m_rows, 0.0f);

      // Now, we need to retrieve for each row the list of items related to it:
      // this allows for quick access when iterating to determine rows' dimensions.
      // We also want to provide easy access to multi-cell item by inserting each
      // one of them in all the rows where it appear.
      // For that to happen we want to first initialize this map with empty vectors
      // and then dynamically populate it.

      std::unordered_map<unsigned, std::vector<ItemDataWrapper>> itemsForRows;
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        itemsForRows[row] = std::vector<ItemDataWrapper>();
      }

      for (LocationsMap::const_iterator item = m_locations.cbegin() ;
            item != m_locations.cend() ;
            ++item)
      {
        // Insert the item in each row spanned by it.
        const ItemInfo& info = item->second;

        // Only if the item is visible.
        if (!items[item->first].visible) {
          continue;
        }

        for (unsigned row = 0u ; row < info.h ; ++row) {
          // Create the item's data.
          ItemDataShPtr data = std::make_shared<ItemData>(
            ItemData{
              item->first,
              info.h > 1,
              row == 0u,
              info.h,
              (info.y + row) * m_columns + info.x,
              utils::Sizef()
            }
          );

          // Insert a wrapper in the corresponding vector.
          itemsForRows[info.y + row].push_back(
            ItemDataWrapper{
              data->id,
              data
            }
          );
        }
      }

      // There's a first part of the optimization process which should be handled
      // right away: the user is allowed to specify a minimum row height for any
      // row of the layout.
      // We can handle the rows where there's at least a wiitemdget later on (it's
      // actually part of the optimization process) but here we should handle the
      // rows which do not contain any items. As no item is there to take
      // space such rows will be ignored by the optimization process.
      // In order to handle these nonetheless, we figured that the best way is to
      // simply subtract the required height from the available total `window`: this
      // makes sense as it would have been the height applied anyway because no
      // item is there to modify it so it would have been accepted right away.

      float heightForEmptyRows = 0.0f;

      // Traverse the `itemsForRows` map and search for rows with no items.
      for (std::unordered_map<unsigned, std::vector<ItemDataWrapper>>::const_iterator row = itemsForRows.cbegin() ;
           row != itemsForRows.cend() ;
           ++row)
      {
        // Search empty rows.
        if (row->second.empty()) {
          // Assign the minimum height to this row and retain the used space.
          rows[row->first] = m_rowsInfo[row->first].min;
          heightForEmptyRows += rows[row->first];
        }
      }

      // Let's start the optimization process.
      // We start with an available space budget described by the value of `window.h()`.
      // We will first try to allocate fairly this space among all rows. Each row will
      // perform checks to determine whether the proposed dimension is valid based on
      // the criteria of each item registered for this row.
      // If this is the case the dimension will be adopted. If not, the dimension will
      // be adapted so that it fits all the constraingts for this row.
      // Once all the rows have been processed, we start all over again: the remaining
      // or missing space is computed, and we then try to allocate the difference among
      // all the rows.
      // This process continues until either all the space has been successfully allocated
      // or there's no more rows to use to adjust the size.
      // In order to make things easier, we use a little trick here. Indeed even though
      // we're reasoning with rows here, the individual unit inside a row remains
      // a item. Thus, when an adjustment should be performed for a row, it is
      // ultimately a item which should be adjusted. Also, during the process rows
      // are interchangeable which means that any operation can and will be applied
      // indistinctly on all the rows available for adjustments.
      // For all these reasons, it makes more sense to keep a list of items to adjust
      // instead of a list of rows: we avoid an indirection to fetch the item linked
      // to an identifier in a row.
      // We still keep the global `itemsForRows` which allows to cherry-pick the
      // single-cell item for a given row, but the whole process is done in terms
      // of items.
      // A small drawbacks is that we need to know how many rows can still be used to
      // perform adjustment: indeed this is what defines how we will split fairly the
      // remanining space.

      std::unordered_set<ItemDataWrapper> itemsToAdjust;
      unsigned rowsRemaining = itemsForRows.size();
      std::unordered_set<unsigned> emptyRows;

      // In a first approach all the rows can be adjusted (except empty ones).
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        if (itemsForRows[row].empty()) {
          emptyRows.insert(row);
          --rowsRemaining;
        }
        else {
          itemsToAdjust.insert(itemsForRows[row].cbegin(), itemsForRows[row].cend());
        }
      }

      // Also assume that we didn't use up all the available space. The remaining space is
      // the difference between the provided space from `window` minus the space used for
      // rows with no items (see two paragraphs above).
      float spaceToUse = window.h() - heightForEmptyRows;
      bool allSpaceUsed = false;

      float achievedHeight = heightForEmptyRows;

      // Loop until no more items can be used to adjust the space needed or all the
      // available space has been used up. As discussed, this is roughly equivalent to
      // checking whether a row is available for adjustment: we only avoid another
      // indirection to fetch the item when performing the adjustments.
      while (!itemsToAdjust.empty() && !allSpaceUsed) {

        // Compute the amount of space we will try to allocate to each row still
        // available for adjustment.
        // The `defaultHeight` is computed by dividing equally the remaining `spaceToUse`
        // among all the available rows.
        // The available rows for adjustment is retrieved using the `rowsRemaining`
        // variable and not directly the `itemsToAdjust.size()` as a single row can
        // have several items needing adjustments.
        const float defaultHeight = allocateFairly(spaceToUse, rowsRemaining);

        // Allocate this space on each item: as all rows are equivalent, this means
        // that we can directly work on individual items.
        for (std::unordered_set<ItemDataWrapper>::const_iterator dataWrapper = itemsToAdjust.cbegin() ;
             dataWrapper != itemsToAdjust.cend() ;
             ++dataWrapper)
        {
          const unsigned item = dataWrapper->data->item;

          // Try to assign the `defaultHeight` to this item: we use a dedicated handler
          // to handle the case where the provided space is too large/small/not suited
          // to the item for some reasons, in which case the handler will provide a
          // size which can be applied to the item.
          // The process is not completely straightforward though as we need to account
          // for items spanning multiple cells. We could either account for the span of
          // the item to multiply the `defaultHeight` or we can add as many instance of
          // the item as needed for a particular row. This second method has a lot
          // of advantages compared to the first one se we chose it.
          // This means that there's no need anymore to account for the item's span in
          // this function as each cell spanned by the item is handled individually
          // which allows to make the item grow more on rows which can account for it.

          // Apply the policy for this item.
          float height = computeHeightFromPolicy(cells[item].box, defaultHeight, items[item]);

          // We now need to distribute this height to the current `data`: in order to do
          // so, let's compute the size increase provided for this item by the current
          // row: this is the size which belongs to the row.
          dataWrapper->data->size.h() += (height - cells[item].box.h());

          // Now register the new size of the item.
          cells[item].box.h() = height;
        }

        // We have tried to apply the `defaultHeight` to all the remaining items available
        // for adjustments. This might have fail in some cases (for example due to a `Fixed`
        // size policy for a item in a given row) and thus we might end up with a total
        // size for all the item different from the one desired and expected when the
        // `defaultHeight` has been computed.
        // In order to fix things, we must compute the deviation from the expected size and
        // try to allocate the remaining space to other items (or remove the missing space
        // from items which can give up some).
        // For single-cell item, it is rather easy: the achieved size only contributes to
        // a single row.
        // For multi-cell items it is a bit different though: we need to distribute the
        // height across several rows: this is usually handled by the `size` variable inside
        // each item's data instance: the instance located in a each row contains information
        // for this row which should be okay as we ensure that it always stay up to date with
        // the total size (i.e. the sum of all individual instances matches the global size).
        // We perform this operation after each optimization operation in order to guarantee
        // that the final value of the `rows` vector will be usable as a valid return value.
        for (unsigned row = 0u ; row < m_rows ; ++row) {
          // Only handle non empty rows.
          if (emptyRows.find(row) == emptyRows.cend()) {
            rows[row] = computeAchievedSize(itemsForRows[row]).h();
          }
        }

        // Compute the achieved size from consolidated dimensions.
        achievedHeight = 0.0f;
        for (unsigned row = 0u ; row < m_rows ; ++row) {
          log(std::string("Row ") + std::to_string(row) + " has size " + std::to_string(rows[row]));
          achievedHeight += rows[row];
        }

        const utils::Sizef achievedSize(window.w(), achievedHeight);

        // Check whether all the space have been used.
        if (achievedSize.compareWithTolerance(window, 1.0f)) {
          // We used up all the available space, no more adjustments to perform.
          allSpaceUsed = true;
          continue;
        }

        // All space has not been used. Update the relevant `spaceToUse` in order to perform
        // the next iteration.
        spaceToUse = computeSpaceAdjustmentNeeded(achievedSize, window).h();

        // Determine the policy to apply based on the achieved size.
        const core::SizePolicy action = shrinkOrGrow(window, achievedSize, 0.5f);

        // We now know what should be done to make the `achievedHeight` closer to `desiredHeight`.
        // Based on the `policy` provided by the base class method, we can now determine which
        // row should be used to perform the needed adjustments.
        // Even if in the end we will substitute the corresponding items to the individual
        // rows, right now we need to keep the rows.
        // This will prove useful in a second phase where we will try to make row which can
        // `Expand` have priority over row which can only `Grow`. This can only be done by
        // keeping the row's id: if we keep only items, we don't have any means (except
        // through exhaustive seach) to find the associated rows to a item. Thus we will
        // not be easily able to discard entire rows if it can only `Grow`.
        std::unordered_set<unsigned> rowsToUse;

        // Traverse each row to determine whether a item in this row can be used to
        // perform the required `action`.
        // Based on the `action`, the way we select rows is a bit different. If we need to
        // shrink some rows, we need all items inside a row to be shrinkable in order
        // for the row to be declared usable: indeed if only some items can be shrunk
        // it also means some cannot shrink and the overall size of the row will not be
        // modified even though the shrinkable item are shrunk.
        // If the action is meant to gorw the situation is a bit different though: as we still
        // have the possibility to center items which are smaller than the total height of
        // the row, a row can be grown as soon as a single item can be grown inside
        // it.
        // In order to allow for easy detection of rows which can be expanded, we also
        // keep the row's identifiers instead of substituting right away the corresponding
        // items. More details can be found in the next comment section.
        for (unsigned row = 0u ; row < itemsForRows.size() ; ++row) {
          // Retrieve the items associated to this row.
          const std::vector<ItemDataWrapper>& itemsForRow = itemsForRows[row];

          // Distinguish based on the action. Furhtermore we are processing rows so
          // we only care about vertical behavior.
          if (action.canExtendVertically()) {
            // Traverse the items for this row: if at least one can be used to `Grow`,
            // consider this row usable to perform the required action.
            for (unsigned item = 0u ; item < itemsForRow.size() ; ++item) {
              // Compute the status of the item for this action.
              const unsigned itemID = itemsForRow[item].data->item;

              std::pair<bool, bool> usable = canBeUsedTo(items[itemID], cells[itemID].box, action);
              if (usable.second) {
                // This row can be used to `Grow` thanks to this item. No need to go
                // further.
                rowsToUse.insert(row);
                break;
              }
            }
          }
          else if (action.canShrinkVertically()) {
            // Traverse the items for this row: all items need to be able to shrink
            // in order for this colum to be marked as shrinkable.

            // Assume this row can be shrunk.
            bool canShrink = true;

            for (unsigned item = 0u ; item < itemsForRow.size() ; ++item) {
              // Compute the status of the item for this action.
              const unsigned itemID = itemsForRow[item].data->item;

              std::pair<bool, bool> usable = canBeUsedTo(items[itemID], cells[itemID].box, action);
              if (!usable.second) {
                // As this item cannot shrink, it means that even if other items shrink
                // the height for this row will remain unchanged so we cannot use it. This
                // is the general idea but we can refine on it: the current item cannot
                // shrink given its current size but it does not mean that the row as a
                // whole cannot shrink. Indeed if the current achieved size of the row is
                // larger than the size required for this item we might still be able to
                // shrink the row without needing to reduce the size of this item.
                // Thus before setting the `canShrink` boolean to false, let's first check
                // whether the height of the row allows to shrink.
                if (rows[row] <= cells[itemID].box.h()) {
                  canShrink = false;
                  break;
                }
              }
            }

            // Register this row for shrinking if needed.
            if (canShrink) {
              rowsToUse.insert(row);
            }
          }
        }

        // We have a list of rows which can be used to perform the required `action`. There's
        // a last filtering to apply though: if the action requires to make some items larger, we need
        // to give priority to items which have the `Expand` flag over items having `Grow` flag.
        // The tricky part is that we're working on rows here so there's no simple way to assign a
        // global policy to a row.
        // In order to determine whether a row needs to `Expand`, we will check each individual item
        // registered for this row and if at least one has the corresponding flag we will assume that
        // the row as a whole can be `Expand`ed.
        // Of course this only applies if the `action` includes growing at all.
        if (action.canExtendVertically()) {
          // Select only `Expanding` rows if any.
          std::unordered_set<unsigned> rowsToExpand;

          for (std::unordered_set<unsigned>::const_iterator row = rowsToUse.cbegin() ;
               row != rowsToUse.cend() ;
               ++row)
          {
            // Check whether this row can expand: this is done by checking each registered
            // item in this row for the appropriate flag.
            const std::vector<ItemDataWrapper>& itemsForRow = itemsForRows[*row];

            for (unsigned item = 0u ; item < itemsForRow.size() ; ++item) {
              // Only consider vertical direction as we're processing rows.
              const unsigned itemID = itemsForRow[item].data->item;

              if (items[itemID].policy.canExpandVertically()) {
                rowsToExpand.insert(*row);
                // No need to continue further, the row can be `Expand`ed.
                break;
              }
            }
          }

          // Check whether we could select at least one item to expand: if this is not the
          // case we can proceed to extend the item with only a `Grow` flag.
          if (!rowsToExpand.empty()) {
            rowsToUse.swap(rowsToExpand);
          }
        }

        // Update the remaining rows so that we can compute correctly the way to allocate space.
        rowsRemaining = rowsToUse.size();

        // We now have a working set of rows which can be used to perform the required `action` and we
        // took into consideration precedence of `Expand` flag over `Grow` flag. We now only need for each
        // row to substitute the corresponding items.
        std::unordered_set<ItemDataWrapper> itemsToUse;

        for (std::unordered_set<unsigned>::const_iterator row = rowsToUse.cbegin() ;
             row != rowsToUse.cend() ;
             ++row)
        {
          itemsToUse.insert(itemsForRows[*row].cbegin(), itemsForRows[*row].cend());
        }

        // Use the computed list of items to perform the next action in order
        // to reach the desired space.
        itemsToAdjust.swap(itemsToUse);
      }

      // Warn the user in case we could not use all the space.
      if (!allSpaceUsed) {
        log(
          std::string("Could only achieve height of ") + std::to_string(achievedHeight) +
          " but available space is " + std::to_string(window.h()),
          utils::Level::Error
        );
      }

      // Return the consolidated rows' dimensions vector.
      return rows;
    }

    void
    GridLayout::adjustMultiCellWidth(const std::vector<float>& columns,
                                     const std::vector<WidgetInfo>& items,
                                     std::vector<CellInfo>& cells)
    {
      // Here we want to perform the last adjustments to the multi-cell items
      // registered in the input `cells` vector.
      // Due to the mechanism in place to determine the optimal repartition of
      // items, we might end up during shrinking phases with multi-cell items
      // taking less space than actually available.
      // In order to fix that, we chose to add an additional phase to account for
      // an adjustment of the multi-cell items so that they take up all the
      // available space.

      // Traverse the list of item and process each multi-cell one.
      for (unsigned item = 0u ; item < cells.size() ; ++item) {
        // Check whether the item is a multi-cell item.
        if (!cells[item].multiCell) {
          // Move on to the next item.
          continue;
        }

        const unsigned itemID = cells[item].item;

        // Retrieve the area spanned by this item.
        LocationsMap::const_iterator locIt = m_locations.find(itemID);
        if (locIt == m_locations.cend()) {
          error(
            std::string("Could not retrieve information for item \"") +
            getItemAt(itemID)->getName() + "\" while updating grid layout"
          );
        }
        const ItemInfo& loc = locIt->second;

        // Determine the total width of the columns spanned by this item.
        float totalWidth = 0.0f;
        for (unsigned column = 0u ; column < loc.w ; ++column) {
          totalWidth += columns[loc.x + column];
        }

        // Now try to assign this width to the item: as the `computeWidthFromPolicy`
        // method tries to *add* the provided width to the existing size of the item
        // we need to account only for the width difference and not the total width.
        const float widthIncrement = totalWidth - cells[item].box.w();
        float width = computeWidthFromPolicy(cells[item].box, widthIncrement, items[itemID]);
        cells[item].box.w() = width;
      }
    }

    void
    GridLayout::adjustMultiCellHeight(const std::vector<float>& rows,
                                      const std::vector<WidgetInfo>& items,
                                      std::vector<CellInfo>& cells)
    {
      // Here we want to perform the last adjustments to the multi-cell items
      // registered in the input `cells` vector.
      // Due to the mechanism in place to determine the optimal repartition of
      // items, we might end up during shrinking phases with multi-cell items
      // taking less space than actually available.
      // In order to fix that, we chose to add an additional phase to account for
      // an adjustment of the multi-cell items so that they take up all the
      // available space.

      // Traverse the list of item and process each multi-cell one.
      for (unsigned item = 0u ; item < cells.size() ; ++item) {
        // Check whether the item is a multi-cell item.
        if (!cells[item].multiCell) {
          // Move on to the next item.
          continue;
        }

        const unsigned itemID = cells[item].item;

        // Retrieve the area spanned by this item.
        LocationsMap::const_iterator locIt = m_locations.find(itemID);
        if (locIt == m_locations.cend()) {
          error(
            std::string("Could not retrieve information for item \"") +
            getItemAt(itemID)->getName() + "\" while updating grid layout"
          );
        }
        const ItemInfo& loc = locIt->second;

        // Determine the total width of the columns spanned by this item.
        float totalHeight = 0.0f;
        for (unsigned row = 0u ; row < loc.h ; ++row) {
          totalHeight += rows[loc.y + row];
        }

        // Now try to assign this height to the item: as the `computeHeightFromPolicy`
        // method tries to *add* the provided height to the existing size of the item
        // we need to account only for the height difference and not the total height.
        const float heightIncrement = totalHeight - cells[item].box.h();
        float height = computeHeightFromPolicy(cells[item].box, heightIncrement, items[itemID]);
        cells[item].box.h() = height;
      }
    }

  }
}
