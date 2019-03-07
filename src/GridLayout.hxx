#ifndef    GRIDLAYOUT_HXX
# define   GRIDLAYOUT_HXX

# include "GridLayout.hh"
# include <limits>
# include <algorithm>
# include <sdl_core/SdlWidget.hh>
# include <sdl_core/SdlException.hh>

namespace sdl {
  namespace graphic {

    inline
    const unsigned&
    GridLayout::getColumnCount() const noexcept {
      return m_columns;
    }

    inline
    const unsigned&
    GridLayout::getRowCount() const noexcept {
      return m_rows;
    }

    inline
    const float&
    GridLayout::getMargin() const noexcept {
      return m_margin;
    }

    inline
    void
    GridLayout::setColumnHorizontalStretch(const unsigned& column, const float& stretch) {
      if (column > m_columns) {
        throw sdl::core::SdlException(
          std::string("Cannot set horizontal stretch for column ") + std::to_string(column) +
          " in " + std::to_string(m_columns) + " column(s) wide layout"
        );
      }

      m_columnsInfo[column].stretch = stretch;
    }

    inline
    void
    GridLayout::setColumnMinimumWidth(const unsigned& column, const float& width) {
      if (column > m_columns) {
        throw sdl::core::SdlException(
          std::string("Cannot set minimum width for column ") + std::to_string(column) +
          " in " + std::to_string(m_columns) + " column(s) wide layout"
        );
      }

      m_columnsInfo[column].min = width;
    }

    inline
    void
    GridLayout::setColumnsMinimumWidth(const float& width) {
      for (unsigned column = 0u ; column < m_columns ; ++column) {
        m_columnsInfo[column].min = width;
      }
    }

    inline
    void
    GridLayout::setRowVerticalStretch(const unsigned& row, const float& stretch) {
      if (row > m_rows) {
        throw sdl::core::SdlException(
          std::string("Cannot set vertical stretch for row ") + std::to_string(row) +
          " in " + std::to_string(m_rows) + " row(s) wide layout"
        );
      }

      m_rowsInfo[row].stretch = stretch;
    }

    inline
    void
    GridLayout::setRowMinimumHeight(const unsigned& row, const float& height) {
      if (row > m_rows) {
        throw sdl::core::SdlException(
          std::string("Cannot set minimum height for row ") + std::to_string(row) +
          " in " + std::to_string(m_rows) + " row(s) wide layout"
        );
      }

      m_rowsInfo[row].min = height;
    }

    inline
    void
    GridLayout::setRowsMinimumHeight(const float& height) {
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        m_rowsInfo[row].min = height;
      }
    }

    inline
    int
    GridLayout::addItem(sdl::core::SdlWidget* container,
                        const unsigned& x,
                        const unsigned& y,
                        const unsigned& w,
                        const unsigned& h)
    {
      int containerIndex = sdl::core::Layout::addItem(container);
      m_locations[containerIndex] = {
        std::min(m_columns - 1, x),
        std::min(m_rows - 1, y),
        std::min(m_columns - std::min(m_columns - 1, x), w),
        std::min(m_rows - std::min(m_rows - 1, y), h)
      };
      return containerIndex;
    }

    inline
    void
    GridLayout::setGrid(const unsigned& columns, const unsigned& rows) {
      // Resize the internal attributes.
      m_columns = columns;
      m_rows = rows;

      // Resize grid info.
      resetGridInfo();
    }

    inline
    void
    GridLayout::resetGridInfo() {
      m_columnsInfo = std::vector<LineInfo>(
        m_columns,
        LineInfo{
          0u,
          0.0f
        }
      );
      m_rowsInfo = std::vector<LineInfo>(
        m_rows,
        LineInfo{
          0u,
          0.0f
        }
      );
    }

    inline
    sdl::utils::Sizef
    GridLayout::computeAvailableSize(const sdl::utils::Boxf& totalArea) const noexcept {
      return sdl::utils::Sizef(
        totalArea.w() - 2.0f * m_margin,
        totalArea.h() - 2.0f * m_margin
      );
    }

    inline
    sdl::utils::Sizef
    GridLayout::computeDefaultWidgetBox(const sdl::utils::Sizef& area,
                                        const unsigned& columnsCount,
                                        const unsigned& rowsCount) const noexcept
    {
      return sdl::utils::Sizef(
        area.w() / columnsCount,
        area.h() / rowsCount
      );
    }

    inline
    bool
    GridLayout::locationSpanColumn(const unsigned& column,
                                   const ItemInfo& info) const noexcept
    {
      return info.x <= column && info.x + info.w > column;
    }

    inline
    bool
    GridLayout::locationSpanRow(const unsigned& row,
                                const ItemInfo& info) const noexcept
    {
      return info.y <= row && info.y + info.h > row;
    }

    inline
    std::vector<GridLayout::CellInfo>
    GridLayout::computeCellsInfo() const noexcept {
      std::vector<CellInfo> cells(m_columns * m_rows);

      // Complete the information with widgets' data.
      for (LocationsMap::const_iterator item = m_locations.cbegin() ;
           item != m_locations.cend() ;
           ++item)
      {
        for (unsigned row = item->second.y ; row < item->second.y + item->second.h ; ++row) {
          for (unsigned column = item->second.x ; column < item->second.x + item->second.w ; ++column) {
            const unsigned cellID = row * m_columns + column;

            cells[cellID].widget = item->first;
            cells[cellID].box = sdl::utils::Boxf();
          }
        }
      }

      // Retrieve basic information on columns.
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          const unsigned cellID = row * m_columns + column;

          cells[cellID].hStretch = m_columnsInfo[column].stretch;
          cells[cellID].vStretch = m_rowsInfo[row].stretch;

          if (m_columnsInfo[column].min > cells[cellID].box.w()) {
            cells[cellID].box.w() = m_columnsInfo[column].min;
          }
          if (m_rowsInfo[row].min > cells[cellID].box.h()) {
            cells[cellID].box.h() = m_rowsInfo[row].min;
          }
        }
      }

      return cells;
    }

    inline
    void
    GridLayout::consolidateDimensions(std::vector<CellInfo>& cells) const noexcept {
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

        // Assign this dimension to all the elements in this row.
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          cells[row * m_columns + column].box.h() = min;
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

        // Assign this dimension to all the elements in this column.
        for (unsigned row = 0u ; row < m_rows ; ++row) {
          cells[row * m_columns + column].box.w() = min;
        }
      }
    }

    inline
    sdl::utils::Sizef
    GridLayout::computeSizeOfCells(const std::vector<CellInfo>& cells) const {
      sdl::utils::Sizef cellsSize;

      for (unsigned row = 0u ; row < m_rows ; ++row) {
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          const unsigned cellID = row * m_columns + column;
          cellsSize.w() += cells[cellID].box.w();
          cellsSize.h() += cells[cellID].box.h();
        }
      }

      return cellsSize;
    }

  }
}

#endif    /* GRIDLAYOUT_HXX */
