
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
      std::vector<CellInfo> cells = computeCellsInfo();

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
      // The specification of the dimensions is divided into two main phases: first the
      // determination of a base dimension for each column and row based on single-cell
      // widget, and then an adjustment phase where multi-cells widgets are placed.
      // In the first phase, the process is divided into two parts: the adjustment of
      // rows and columns is indeed completely independent and can be processed with no
      // particular ordering.
      // This allows for simpler algorithms.
      // In each case, the process is similar: we start by assuming an ideal size of the
      // widget for each column/row and try to work with this. In case some widget cannot
      // take full advantage of this size (because their constraints forbid it) we
      // allocate the remaining space between remaining widgets until the achieved size
      // is close enough from the target size.
      // If the widgets' constraints do not allow for a perfect repartition, we stop the
      // process and produce the best possible solution given the constraints.

      // Proceed to adjust the columns' width.
      std::vector<float> columnsDims = adjustColumnsWidth(internalSize, widgetsInfo);

      // Adjust rows' height.
      std::vector<float> rowsDims = adjustRowHeight(internalSize, widgetsInfo);

      // Try to adjust the computed dimensions to include multi-cells widgets.
      // TODO: Actually handle multi-cells widgets.

      // All widgets have suited dimensions, we can now handle the position of each
      // widget. We basically just move each widget based on the dimensions of the
      // rows and columns to reach the position of a specified widget.
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

    /////////
    // Old //
    /////////

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

    /////////
    // Old //
    /////////

    std::vector<float>
    GridLayout::adjustColumnsWidth(const utils::Sizef& window,
                                   const std::vector<WidgetInfo>& widgets) const
    {
      return std::vector<float>(widgets.size(), window.w() / widgets.size());
    }

    std::vector<float>
    GridLayout::adjustRowHeight(const utils::Sizef& window,
                                const std::vector<WidgetInfo>& widgets) const
    {
      return std::vector<float>(widgets.size(), window.h() / widgets.size());
    }

  }
}
