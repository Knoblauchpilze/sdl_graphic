
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
      sdl::core::Layout(widget),
      m_columns(columns),
      m_rows(rows),

      m_columnsInfo(),
      m_rowsInfo(),

      m_margin(margin),
      m_locations()
    {
      // Build default information for columns/rows.
      resetGridInfo();
    }

    GridLayout::~GridLayout() {}

    void
    GridLayout::updatePrivate(const sdl::utils::Boxf& window) {
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
      const sdl::utils::Sizef internalSize = computeAvailableSize(window);

      // Compute default columns and rows dimensions.
      // TODO: Override the minimum size of each widget with the size of the
      // column/row in which it lies ?
      std::vector<CellInfo> cells = computeCellsInfo();

      // Copy the current size of widgets so that we can work with it without
      // requesting constantly information or setting information multiple times.
      std::vector<WidgetInfo> widgetsInfo = computeWidgetsInfo();

      std::cout << "[LAY] Available size: " << window.w() << "x" << window.h() << std::endl;
      std::cout << "[LAY] Internal size: " << internalSize.w() << "x" << internalSize.h() << std::endl;

      // ====
      // Debug
      // for (unsigned row = 0u ; row < m_rows ; ++row) {
      //   for (unsigned column = 0u ; column < m_columns ; ++column) {
      //     const unsigned cellID = row * m_columns + column;
      //     std::cout << "[LAY] Cell " << column << "x" << row
      //               << ": stretch=" << cells[cellID].hStretch << "x" << cells[cellID].vStretch
      //               << ", min: " << cells[cellID].box.w() << "x" << cells[cellID].box.h()
      //               << ", wid: " << (cells[cellID].widget < 0 ? "no widget" : std::to_string(cells[cellID].widget))
      //               << std::endl;
      //   }
      // }
      // Debug
      // ====

      for (unsigned row = 0u ; row < m_rows ; ++row) {
        std::cout << "[LAY] row " << row << ":";
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          const unsigned cellID = row * m_columns + column;
          std::cout << " " << std::setw(3) << cells[cellID].box.w() << "x" << std::setw(3) << cells[cellID].box.h();
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

      // In a first approach all the widgets can be adjusted.
      for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
        widgetsToAdjust.insert(index);
      }

      // Also assume that we didn't use up all the available space.
      sdl::utils::Sizef spaceToUse = internalSize;
      bool allSpaceUsed = false;

      // Loop until no more widgets can be used to adjust the space needed or all the
      // available space has been used up.
      // TODO: Handle cases where the space cannot be reached because no more widgets
      // can be used ?
      // TODO: Handle cases where the widgets are too large to fit into the widget ?
      while (!widgetsToAdjust.empty() && !allSpaceUsed) {
      
        // Compute the amount of space we will try to allocate to each widget still
        // available for adjustment.
        // The `defaultBox` is computed by dividing equally the remaining `workingSize`
        // among all the available widgets.
        const sdl::utils::Sizef defaultBox = computeDefaultWidgetBox(internalSize, m_columns, m_rows);

        std::cout << "[LAY] Default box is " << defaultBox.w() << "x" << defaultBox.h() << std::endl;

        for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
          // Retrieve the `ItemInfo` struct for this widget.
          const LocationsMap::const_iterator itemInfo = m_locations.find(index);
          if (itemInfo == m_locations.cend()) {
            throw sdl::core::SdlException(
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
          const sdl::utils::Sizef widgetBox(defaultBox.w() * itemInfo->second.w, defaultBox.h() * itemInfo->second.h);

          // Apply the policy for this widget.
          sdl::utils::Sizef area = computeSizeFromPolicy(widgetBox, cells[cellID].box, widgetsInfo[index]);
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
        consolidateDimensions(cells);

        for (unsigned row = 0u ; row < m_rows ; ++row) {
          std::cout << "[LAY] row " << row << ":";
          for (unsigned column = 0u ; column < m_columns ; ++column) {
            const unsigned cellID = row * m_columns + column;
            std::cout << " " << std::setw(3) << cells[cellID].box.w() << "x" << std::setw(3) << cells[cellID].box.h();
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
        sdl::utils::Sizef achievedSize = computeSizeOfCells(cells);

        // Check whether all the space have been used.
        if (achievedSize == internalSize) {
          // We used up all the available space, no more adjustments to perform.
          allSpaceUsed = true;
          continue;
        }

        // All space has not been used. Update the relevant `spaceToUse` in order to perform
        // the next iteration.
        spaceToUse = computeSpaceAdjustmentNeeded(achievedSize, internalSize);

        // Determine the policy to apply based on the achieved size.
        const sdl::core::SizePolicy action = shrinkOrGrow(internalSize, achievedSize);

        std::cout << "[LAY] Desired: " << internalSize.w() << ", " << internalSize.h()
                  << " achieved: " << achievedSize.w() << ", " << achievedSize.h()
                  << " space: " << spaceToUse.w() << ", " << spaceToUse.h()
                  << std::endl;

        // We now know what should be done to make the `achievedSize` closer to `desiredSize`.
        // Based on the `policy` provided by the base class method, we can now determine which
        // widget should be used to perform the needed adjustments.
        std::unordered_set<unsigned> widgetsToUse;
        for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
          // Retrieve the `ItemInfo` struct for this widget.
          const LocationsMap::const_iterator itemInfo = m_locations.find(index);
          if (itemInfo == m_locations.cend()) {
            throw sdl::core::SdlException(
              std::string("Could not retrieve information for widget \"") +
              m_items[index]->getName() + "\" while updating grid layout"
            );
          }
          const ItemInfo& loc = itemInfo->second;
          const unsigned cellID = loc.y * m_columns + loc.x;

          // Check whether this widget can be used to grow/shrink.
          if (canBeUsedTo(widgetsInfo[index], cells[cellID].box, action)) {
            std::cout << "[LAY] " << m_items[index]->getName() << " can be used to "
                      << std::to_string(static_cast<int>(action.getHorizontalPolicy()))
                      << " and "
                      << std::to_string(static_cast<int>(action.getVerticalPolicy()))
                      << std::endl;
            widgetsToUse.insert(index);
          }
        }

        // Use the computed list of widgets to perform the next action in order
        // to reach the desired space.
        widgetsToAdjust.swap(widgetsToUse);

        // TODO: Debug infinite loop.
      }

      // All widgets have suited dimensions, we can now handle the position of each
      // widget. We basically just move each widget based on the dimensions of the
      // rows and columns to reach the position of each widget.
      // dimensions and adding margins.
      float x = m_margin;
      float y = m_margin;

      std::vector<sdl::utils::Boxf> outputBoxes(m_items.size());

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
          throw sdl::core::SdlException(
            std::string("Could not retrieve information for widget \"") +
            m_items[index]->getName() + "\" while updating grid layout"
          );
        }

        float xWidget = m_margin;
        float yWidget = m_margin;

        const unsigned cellID = loc->second.y * m_columns + loc->second.x;

        // Compute the offset to apply to reach the desired column based on the
        // item's location.
        for (unsigned column = 0u ; column < loc->second.x ; ++column) {
          xWidget += cells[loc->second.y * m_columns + column].box.w();
        }
        for (unsigned row = 0u ; row < loc->second.y ; ++row) {
          yWidget += cells[row * m_columns + loc->second.x].box.h();
        }

        // Center the position (because `Boxf` are centered).
        xWidget += (cells[cellID].box.w() / 2.0f);
        yWidget += (cells[cellID].box.h() / 2.0f);

        // Handle the centering of the widget in case it is smaller than the
        // desired width or height.
        // To do so, compute the size the widget _should_ have based on its
        // columns/rows span.
        // TODO: Handle centerin.
        // float expectedWidth = 0.0f;
        // float expectedHeight = 0.0f;

        // for (unsigned column = loc->second.x ; column < loc->second.x + loc->second.w ; ++column) {
        //   expectedWidth += columnsBoxes[column];
        // }
        // for (unsigned row = loc->second.y ; row < loc->second.y + loc->second.h ; ++row) {
        //   expectedHeight += rowsBoxes[row];
        // }

        // if (outputBoxes[index].w() < expectedWidth) {
        //   xWidget += ((expectedWidth - outputBoxes[index].w()) / 2.0f);
        // }
        // if (outputBoxes[index].h() < expectedHeight) {
        //   yWidget += ((expectedHeight - outputBoxes[index].h()) / 2.0f);
        // }

        outputBoxes[index] = sdl::utils::Boxf(
          xWidget, yWidget,
          cells[cellID].box.w(), cells[cellID].box.h()
        );
      }

      // Assign the rendering area to widgets.
      assignRenderingAreas(outputBoxes);
    }

  }
}
