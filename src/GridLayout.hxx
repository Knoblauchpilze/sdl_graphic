#ifndef    GRIDLAYOUT_HXX
# define   GRIDLAYOUT_HXX

# include "GridLayout.hh"

# include <limits>
# include <algorithm>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    inline
    unsigned
    GridLayout::getColumnCount() const noexcept {
      return m_columns;
    }

    inline
    unsigned
    GridLayout::getRowCount() const noexcept {
      return m_rows;
    }

    inline
    void
    GridLayout::setColumnHorizontalStretch(unsigned column,
                                           float stretch)
    {
      if (column > m_columns) {
        error(
          std::string("Cannot set horizontal stretch for column ") + std::to_string(column) +
          " in " + std::to_string(m_columns) + " column(s) wide layout"
        );
      }

      m_columnsInfo[column].stretch = stretch;
    }

    inline
    void
    GridLayout::setColumnMinimumWidth(unsigned column,
                                      float width)
    {
      if (column > m_columns) {
        error(
          std::string("Cannot set minimum width for column ") + std::to_string(column) +
          " in " + std::to_string(m_columns) + " column(s) wide layout"
        );
      }

      m_columnsInfo[column].min = width;
    }

    inline
    void
    GridLayout::setColumnsMinimumWidth(float width) {
      for (unsigned column = 0u ; column < m_columns ; ++column) {
        m_columnsInfo[column].min = width;
      }
    }

    inline
    void
    GridLayout::setRowVerticalStretch(unsigned row,
                                      float stretch)
    {
      if (row > m_rows) {
        error(
          std::string("Cannot set vertical stretch for row ") + std::to_string(row) +
          " in " + std::to_string(m_rows) + " row(s) wide layout"
        );
      }

      m_rowsInfo[row].stretch = stretch;
    }

    inline
    void
    GridLayout::setRowMinimumHeight(unsigned row,
                                    float height)
    {
      if (row > m_rows) {
        error(
          std::string("Cannot set minimum height for row ") + std::to_string(row) +
          " in " + std::to_string(m_rows) + " row(s) wide layout"
        );
      }

      m_rowsInfo[row].min = height;
    }

    inline
    void
    GridLayout::setRowsMinimumHeight(float height) {
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        m_rowsInfo[row].min = height;
      }
    }

    inline
    void
    GridLayout::addItem(core::LayoutItem* container,
                        unsigned x,
                        unsigned y,
                        unsigned w,
                        unsigned h)
    {
      // Use the base handler to add the item and retrieve its index.
      int physID = core::Layout::addItem(container);

      // Add the item to the internal array if a valid index was generated.
      if (physID >= 0) {
        m_locations[physID] = ItemInfo{
          std::min(m_columns - 1, x),
          std::min(m_rows - 1, y),
          std::min(m_columns - std::min(m_columns - 1, x), w),
          std::min(m_rows - std::min(m_rows - 1, y), h),
          container
        };
      }
    }

    inline
    void
    GridLayout::setGrid(unsigned columns,
                        unsigned rows)
    {
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

  }
}

#endif    /* GRIDLAYOUT_HXX */
