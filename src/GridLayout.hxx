#ifndef    GRIDLAYOUT_HXX
# define   GRIDLAYOUT_HXX

# include "GridLayout.hh"
# include <limits>
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
      m_itemsLocation[containerIndex] = {
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
      return info.x <= column && info.x + info.w >= column;
    }

    inline
    bool
    GridLayout::locationSpanRow(const unsigned& row,
                                const ItemInfo& info) const noexcept
    {
      return info.y <= row && info.y + info.h >= row;
    }

  }
}

#endif    /* GRIDLAYOUT_HXX */
