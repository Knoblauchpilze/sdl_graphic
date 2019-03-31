
# include "GridLayout.hh"

# include <unordered_set>
# include <sdl_core/SdlWidget.hh>

# include <iomanip>

namespace sdl {
  namespace graphic {

    GridLayout::GridLayout(const unsigned& columns,
                           const unsigned& rows,
                           const float& margin,
                           sdl::core::SdlWidget* widget):
      sdl::core::Layout(widget, margin),
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
    GridLayout::updatePrivate(const utils::Boxf& window) {
      // The `GridLayout` allows to arrange widgets using across a virtual
      // grid composed of `m_columns` columns and `m_rows` rows. The default
      // behavior is to provide an equal allocation of the available space
      // to all the widgets, but also to take into account the provided
      // information about widgets' preferred size and bounds.
      //
      // The process includes adjusting rows and columns by validating that
      // the final dimensions do correspond to the criteria applied to all
      // the widgets registered for a single column/row.

      // First, we need to compute the available size for this layout. We need
      // to take into account margins.
      const utils::Sizef internalSize = computeAvailableSize(window);

      // Compute default columns and rows dimensions.
      // TODO: Override the minimum size of each widget with the size of the
      // column/row in which it lies ?
      std::vector<CellInfo> cells = computeCellsInfo();
      std::vector<float> columnsDims(m_columns);
      std::vector<float> rowsDims(m_rows);

      // Copy the current size of widgets so that we can work with it without
      // requesting constantly information or setting information multiple times.
      std::vector<WidgetInfo> widgetsInfo = computeWidgetsInfo();

      std::cout << "[LAY] Available size: " << window.w() << "x" << window.h() << std::endl;
      std::cout << "[LAY] Internal size: " << internalSize.w() << "x" << internalSize.h() << std::endl;

      for (unsigned row = 0u ; row < m_rows ; ++row) {
        std::cout << "[LAY] row " << row << ":";
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          const unsigned cellID = row * m_columns + column;
          std::cout << " " << std::setw(6) << cells[cellID].box.w() << "x" << std::setw(6) << cells[cellID].box.h();
        }
        std::cout << std::endl;
      }

      // We now have a working set of dimensions which we can begin to apply to widgets
      // in order to build the layout.
      // Basically we will try to assign a `ðefaultBox` to all widgets, corresponding to
      // the ideal value to use to allocate fairly the space between all widgets. Widgets
      // will handle internally this assignment by checking it against internal
      // constraints (min and max size for example) and use the best size considering the
      // input request.
      // Once we're done, we can compute the space left (or missing) and start the
      // process again with the remaining size.
      // We stop the process when the space has been entirely allocated to widgets or
      // when there's no widget left to expand/shrink without violating the provided
      // size constraints.
      std::unordered_set<unsigned> widgetsToAdjust;

      // TODO: We should probably proceed by dimensions: first adjusting the width of
      // widgets and then height.
      // This would probably better to handle expanding policy and could also make
      // things converge faster.

      // In a first approach all the widgets can be adjusted.
      for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
        widgetsToAdjust.insert(index);
      }

      // Also assume that we didn't use up all the available space.
      utils::Sizef spaceToUse = internalSize;
      bool allSpaceUsed = false;

      // Loop until no more widgets can be used to adjust the space needed or all the
      // available space has been used up.
      // TODO: Handle cases where the widgets are too large to fit into the widget ?
      while (!widgetsToAdjust.empty() && !allSpaceUsed) {
      
        // Compute the amount of space we will try to allocate to each widget still
        // available for adjustment.
        // The `defaultBox` is computed by dividing equally the remaining `workingSize`
        // among all the available widgets.
        const utils::Sizef defaultBox = computeDefaultWidgetBox(spaceToUse, m_columns, m_rows);

        std::cout << "[LAY] Default box is " << defaultBox << std::endl;

        for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
          // Check whether this widget need adjustment.
          if (widgetsToAdjust.find(index) == widgetsToAdjust.cend()) {
            // Move on to the next widget.
            continue;
          }

          // Retrieve the `ItemInfo` struct for this widget.
          const LocationsMap::const_iterator itemInfo = m_locations.find(index);
          if (itemInfo == m_locations.cend()) {
            error(
              std::string("Could not retrieve information for widget \"") +
              m_items[index]->getName() + "\" while updating grid layout"
            );
          }
          const ItemInfo& loc = itemInfo->second;
          const unsigned cellID = loc.y * m_columns + loc.x;

          // Try to assign the `defaultBox` to this widget: we use a dedicated handler
          // to handle the case where the provided space is too large/small/not suited
          // to the widget for some reasons, in which case the handler will provide a
          // size which can be applied to the widget.
          // The process is not completely straightforward though as we need to account
          // for widgets spanning multiple columns/rows. The `defaultBox` should thus
          // be scaled to account for this.

          // Scale the `defaultBox`.
          const utils::Sizef widgetBox(defaultBox.w() * itemInfo->second.w, defaultBox.h() * itemInfo->second.h);

          // Apply the policy for this widget.
          utils::Sizef area = computeSizeFromPolicy(cells[cellID].box, widgetBox, widgetsInfo[index]);
          cells[cellID].box.w() = area.w();
          cells[cellID].box.h() = area.h();

          std::cout << "[LAY] Widget \"" << m_items[index]->getName() << "\": "
                    << cells[cellID].box.x() << ", " << cells[cellID].box.y()
                    << ", dims: "
                    << cells[cellID].box.w() << ", " << cells[cellID].box.h()
                    << std::endl;
        }

        // Once all widgets have been assigned dimensions based on the `ðefaultBox`, we need to
        // consolidate the dimensions of the cell to determine whether the adjustment is done.
        consolidateDimensions(cells, columnsDims, rowsDims);

        for (unsigned row = 0u ; row < m_rows ; ++row) {
          std::cout << "[LAY] row " << row << ":";
          for (unsigned column = 0u ; column < m_columns ; ++column) {
            const unsigned cellID = row * m_columns + column;
            std::cout << " " << std::setw(7) << cells[cellID].box.w() << "x" << std::setw(7) << cells[cellID].box.h();
          }
          std::cout << std::endl;
        }

        // We have tried to apply the `defaultBox` to all the widgets. This might have fail
        // in some cases (for example due to a `Fixed` size policy for a widget) and thus
        // we might end up with a total size for all the widget different from the one desired
        // and expected when the `defaultBox` has been computed.
        // In order to fix things, we must compute the deviation from the expected size and
        // try to allocate the remaining space to other widgets (or remove the missing space
        // from widgets which can give up some).

        // Compute the total size of the bounding boxes.
        utils::Sizef achievedSize = computeSizeOfCells(cells);

        // Check whether all the space have been used.
        if (achievedSize.fuzzyEqual(internalSize, 1.0f)) {
          // We used up all the available space, no more adjustments to perform.
          allSpaceUsed = true;
          continue;
        }

        // All space has not been used. Update the relevant `spaceToUse` in order to perform
        // the next iteration.
        spaceToUse = computeSpaceAdjustmentNeeded(achievedSize, internalSize);

        // Determine the policy to apply based on the achieved size.
        const sdl::core::SizePolicy action = shrinkOrGrow(internalSize, achievedSize, 0.5f);

        std::cout << "[LAY] Desired: " << internalSize
                  << " achieved: " << achievedSize
                  << " space: " << spaceToUse
                  << std::endl;

        // We now know what should be done to make the `achievedSize` closer to `desiredSize`.
        // Based on the `policy` provided by the base class method, we can now determine which
        // widget should be used to perform the needed adjustments.
        std::unordered_set<unsigned> widgetsToUse;
        for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
          // Retrieve the `ItemInfo` struct for this widget.
          const LocationsMap::const_iterator itemInfo = m_locations.find(index);
          if (itemInfo == m_locations.cend()) {
            error(
              std::string("Could not retrieve information for widget \"") +
              m_items[index]->getName() + "\" while updating grid layout"
            );
          }
          const ItemInfo& loc = itemInfo->second;
          const unsigned cellID = loc.y * m_columns + loc.x;

          // Check whether this widget can be used to grow/shrink.
          std::pair<bool, bool> usable = canBeUsedTo(m_items[index]->getName(), widgetsInfo[index], cells[cellID].box, action);
          if (usable.first || usable.second) {
            // std::cout << "[LAY] " << m_items[index]->getName() << " can be used to "
            //           << std::to_string(static_cast<int>(action.getHorizontalPolicy()))
            //           << " and "
            //           << std::to_string(static_cast<int>(action.getVerticalPolicy()))
            //           << std::endl;
            widgetsToUse.insert(index);
          }
        }

        // There's one more thing to determine: the `Expanding` flag on any widget's policy should
        // mark it as priority over other widgets. For example if two widgets can grow, one having
        // the flag `Grow` and the other the `Expand` flag, we should make priority for the one
        // with `Expanding` flag.
        // Widgets with `Grow` flag will only grow when all `Expanding` widgets have been maxed out.
        // Of course this does not apply in case widgets should be shrunk: all widgets are treated
        // equally in this case and there's not preferred widgets to shrink.
        if (action.canExtendHorizontally() || action.canExtendVertically()) {
          // Select only `Expanding` widget if any.
          std::unordered_set<unsigned> widgetsToExpand;

          for (std::unordered_set<unsigned>::const_iterator widget = widgetsToUse.cbegin() ;
               widget != widgetsToUse.cend() ;
               ++widget)
          {
            // Check whether this widget can expand.
            if (action.canExtendHorizontally() && widgetsInfo[*widget].policy.canExpandHorizontally()) {
              std::cout << "[LAY] " << m_items[*widget]->getName() << " can be expanded horizontally" << std::endl;
              widgetsToExpand.insert(*widget);
            }
            if (action.canExtendVertically() && widgetsInfo[*widget].policy.canExpandVertically()) {
              std::cout << "[LAY] " << m_items[*widget]->getName() << " can be expanded vertically" << std::endl;
              widgetsToExpand.insert(*widget);
            }
          }

          std::cout << "[LAY] Saved " << widgetsToExpand.size() << " which can expand compared to "
                    << widgetsToUse.size() << " which can extend"
                    << std::endl;
          // Check whether we could select at least one widget to expand: if this is not the
          // case we can proceed to extend the widget with only a `Grow` flag.
          if (!widgetsToExpand.empty()) {
            widgetsToUse.swap(widgetsToExpand);
          }
        }

        // Use the computed list of widgets to perform the next action in order
        // to reach the desired space.
        widgetsToAdjust.swap(widgetsToUse);
      }

      // All widgets have suited dimensions, we can now handle the position of each
      // widget. We basically just move each widget based on the dimensions of the
      // rows and columns to reach the position of each widget.
      // dimensions and adding margins.

      std::vector<utils::Boxf> outputBoxes(m_items.size());

      for (unsigned index = 0u ; index < m_items.size() ; ++index) {
        // Position the widget based on the dimensions of the rows and columns
        // until the position of the widget.
        // We maintained a vector to keep track of the dimensions of each row
        // and column during the adjustment process so that we can use it now
        // to assign a position to the boxes.
        // In addition to this mechanism, we should handle some kind of
        // centering to allow widgets with sizes smaller than the provided
        // layout's dimensions to still be nicely displayed in the center
        // of the layout.
        // To handle this case we check whether the dimensions of the size
        // of the widget is smaller than the dimension stored in `internalSize`
        // in which case we can center it.
        // The centering takes place according to both the dimensions of the
        // rows and columns spanned by the widget.

        // Retrieve the item's location.
        const LocationsMap::const_iterator loc = m_locations.find(index);
        if (loc == m_locations.end()) {
          error(
            std::string("Could not retrieve information for widget \"") +
            m_items[index]->getName() + "\" while updating grid layout"
          );
        }

        float xWidget = getMargin().w();
        float yWidget = getMargin().h();

        const unsigned cellID = loc->second.y * m_columns + loc->second.x;

        // Compute the offset to apply to reach the desired column based on the
        // item's location.
        for (unsigned column = 0u ; column < loc->second.x ; ++column) {
          xWidget += columnsDims[column];
        }
        for (unsigned row = 0u ; row < loc->second.y ; ++row) {
          yWidget += rowsDims[row];
        }

        // Center the position (because `Boxf` are centered).
        xWidget += (cells[cellID].box.w() / 2.0f);
        yWidget += (cells[cellID].box.h() / 2.0f);

        // Handle the centering of the widget in case it is smaller than the
        // desired width or height.
        // To do so, compute the size the widget _should_ have based on its
        // columns/rows span.
        float expectedWidth = 0.0f;
        float expectedHeight = 0.0f;

        for (unsigned column = loc->second.x ; column < loc->second.x + loc->second.w ; ++column) {
          expectedWidth += columnsDims[column];
        }
        for (unsigned row = loc->second.y ; row < loc->second.y + loc->second.h ; ++row) {
          expectedHeight += rowsDims[row];
        }

        if (cells[cellID].box.w() < expectedWidth) {
          xWidget += ((expectedWidth - cells[cellID].box.w()) / 2.0f);
        }
        if (cells[cellID].box.h() < expectedHeight) {
          yWidget += ((expectedHeight - cells[cellID].box.h()) / 2.0f);
        }

        outputBoxes[index] = utils::Boxf(
          xWidget, yWidget,
          cells[cellID].box.w(), cells[cellID].box.h()
        );
      }

      // Assign the rendering area to widgets.
      assignRenderingAreas(outputBoxes);
    }

    std::vector<GridLayout::CellInfo>
    GridLayout::computeCellsInfo() const noexcept {
      std::vector<CellInfo> cells(
        m_columns * m_rows,
        CellInfo{
          0,
          0,
          utils::Boxf(),
          -1
        }
      );

      // Complete the information with widgets' data.
      for (LocationsMap::const_iterator item = m_locations.cbegin() ;
           item != m_locations.cend() ;
           ++item)
      {
        for (unsigned row = item->second.y ; row < item->second.y + item->second.h ; ++row) {
          for (unsigned column = item->second.x ; column < item->second.x + item->second.w ; ++column) {
            const unsigned cellID = row * m_columns + column;

            cells[cellID].widget = item->first;
            cells[cellID].box = utils::Boxf();
          }
        }
      }

      return cells;
    }

    void
    GridLayout::consolidateDimensions(std::vector<CellInfo>& cells,
                                      std::vector<float>& columnsDims,
                                      std::vector<float>& rowsDims) const noexcept
    {
      // Equalize columns and rows information.
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        // Determine the max value of the minimum height of this row.
        float min = 0.0f;
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          const unsigned cellID = row * m_columns + column;
          if (cells[cellID].box.h() > min) {
            min = cells[cellID].box.h();
          }
        }

        rowsDims[row] = min;

        // Assign this dimension to all the elements in this row.
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          if (cells[row * m_columns + column].widget < 0) {
            cells[row * m_columns + column].box.h() = min;
          }
        }
      }

      for (unsigned column = 0u ; column < m_columns ; ++column) {
        // Determine the max value of the minimum width of this column.
        float min = 0.0f;
        for (unsigned row = 0u ; row < m_rows ; ++row) {
          const unsigned cellID = row * m_columns + column;
          if (cells[cellID].box.w() > min) {
            min = cells[cellID].box.w();
          }
        }

        columnsDims[column] = min;

        // Assign this dimension to all the elements in this column.
        for (unsigned row = 0u ; row < m_rows ; ++row) {
          if (cells[row * m_columns + column].widget < 0) {
            cells[row * m_columns + column].box.w() = min;
          }
        }
      }
    }

    utils::Sizef
    GridLayout::computeSizeOfCells(const std::vector<CellInfo>& cells) const {
      // Compute the maximum size of any column.
      float maxColumnSize = 0.0f;
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        float columnSize = 0.0f;
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          const unsigned cellID = row * m_columns + column;
          columnSize += cells[cellID].box.w();
        }
        
        if (maxColumnSize < columnSize) {
          maxColumnSize = columnSize;
        }
      }

      // Compute the maximum size of any row.
      float maxRowSize = 0.0f;
      for (unsigned column = 0u ; column < m_columns ; ++column) {
        float rowSize = 0.0f;
        for (unsigned row = 0u ; row < m_rows ; ++row) {
          const unsigned cellID = row * m_columns + column;
          rowSize += cells[cellID].box.h();
        }
        
        if (maxRowSize < rowSize) {
          maxRowSize = rowSize;
        }
      }

      return utils::Sizef(maxColumnSize, maxRowSize);
    }

  }
}
