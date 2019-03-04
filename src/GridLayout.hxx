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
    GridLayout::setColumnMinimumWidth(const unsigned& column, const float& width) {
      if (column > m_columnsMinimumWidth.size()) {
        throw sdl::core::SdlException(
          std::string("Cannot set minimum width for column ") + std::to_string(column) +
          " in " + std::to_string(m_columnsMinimumWidth.size()) + " column(s) wide layout"
        );
      }

      m_columnsMinimumWidth[column] = width;
    }

    inline
    void
    GridLayout::setRowMinimumHeight(const unsigned& row, const float& height) {
      if (row > m_rowsMinimumHeight.size()) {
        throw sdl::core::SdlException(
          std::string("Cannot set minimum height for row ") + std::to_string(row) +
          " in " + std::to_string(m_rowsMinimumHeight.size()) + " row(s) wide layout"
        );
      }

      m_rowsMinimumHeight[row] = height;
    }

    inline
    void
    GridLayout::setColumnMaximumWidth(const unsigned& column, const float& width) {
      if (column > m_columnsMaximumWidth.size()) {
        throw sdl::core::SdlException(
          std::string("Cannot set maximum width for column ") + std::to_string(column) +
          " in " + std::to_string(m_columnsMaximumWidth.size()) + " column(s) wide layout"
        );
      }

      m_columnsMaximumWidth[column] = width;
    }

    inline
    void
    GridLayout::setRowMaximumHeight(const unsigned& row, const float& height) {
      if (row > m_rowsMaximumHeight.size()) {
        throw sdl::core::SdlException(
          std::string("Cannot set maximum height for row ") + std::to_string(row) +
          " in " + std::to_string(m_rowsMaximumHeight.size()) + " column(s) wide layout"
        );
      }

      m_rowsMaximumHeight[row] = height;
    }

    inline
    void
    GridLayout::setColumnHorizontalStretch(const unsigned& column, const float& stretch) {
      if (column > m_columnsStretches.size()) {
        throw sdl::core::SdlException(
          std::string("Cannot set horizontal stretch for column ") + std::to_string(column) +
          " in " + std::to_string(m_columnsStretches.size()) + " column(s) wide layout"
        );
      }

      m_columnsStretches[column] = stretch;
    }

    inline
    void
    GridLayout::setRowVerticalStretch(const unsigned& row, const float& stretch) {
      if (row > m_rowsStretches.size()) {
        throw sdl::core::SdlException(
          std::string("Cannot set horizontal stretch for row ") + std::to_string(row) +
          " in " + std::to_string(m_rowsStretches.size()) + " row(s) wide layout"
        );
      }

      m_rowsStretches[row] = stretch;
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
        std::min(m_columns - x, w),
        std::min(m_rows - y, h)
      };
      return containerIndex;
    }

    inline
    void
    GridLayout::setGrid(const unsigned& columns, const unsigned& rows) {
      // Resize the internal attributes.
      m_columns = columns;
      m_rows = rows;

      // Resize cells stretches with default values.
      m_columnsStretches.resize(columns, 0.0f);
      m_rowsStretches.resize(columns, 0.0f);

      // Resize cells minimum dimensions with default values.
      m_columnsMinimumWidth.resize(columns, 0.0f);
      m_columnsMaximumWidth.resize(columns, std::numeric_limits<float>::max());
      m_rowsMinimumHeight.resize(columns, 0.0f);
      m_rowsMaximumHeight.resize(columns, std::numeric_limits<float>::max());
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


    // ====================
    // To be removed
    // ====================

    inline
    void
    GridLayout::computeColumnsWidth(const float& totalWidth, float& cw) const noexcept {
      // The width available for each column corresponds to the total width minus
      // the global margin.
      const float availableWidth = totalWidth - 2.0f * m_margin;

      // Now we need to split this available width among all the logical _cells_
      // defined by the columns.
      // Indeed each column can provide a `stretch` which allows to specify that
      // the column should occupy a larger portion of the available space.
      // As an example if we have two columns `A` and `B` with `A.stretch = 1`
      // and `B.stretch = 2`, the layout will allocate space so that `B` is twice
      // the size of `A`.

      // Compute the number of logical _cells_ from the components.
      float cells = 0.0f;
      for (int indexColumn = 0 ; indexColumn < m_columns ; ++indexColumn) {
        cells += m_columnsStretches[indexColumn];
      }

      // The available width can now be split evenly between all the cells.
      cw = availableWidth / cells;
    }

    inline
    void
    GridLayout::computeRowsHeight(const float& totalHeight, float& rh) const noexcept {
      // The height available for each row corresponds to the total height minus
      // the global margin.
      const float availableHeight = totalHeight - 2.0f * m_margin;

      // Now we need to split this available height among all the logical _cells_
      // defined by the rows.
      // Indeed each row can provide a `stretch` which allows to specify that
      // the row should occupy a larger portion of the available space.
      // As an example if we have two rows `A` and `B` with `A.stretch = 1`
      // and `B.stretch = 2`, the layout will allocate space so that `B` is twice
      // the size of `A`.

      // Compute the number of logical _cells_ from the components.
      float cells = 0.0f;
      for (int indexRow = 0 ; indexRow < m_rows ; ++indexRow) {
        cells += m_rowsStretches[indexRow];
      }

      // The available row can now be split evenly between all the cells.
      rh = availableHeight / cells;
    }

    inline
    std::vector<float>
    GridLayout::computeColumnsOrigin(const float& cw) const noexcept {
      // Create the return value.
      std::vector<float> columnsOrigin(m_columns, 0.0f);

      float xStart = m_margin;

      // Traverse each column and use its stretch to coppute its origin.
      for (int indexColumn = 0 ; indexColumn < m_columns ; ++indexColumn) {
        columnsOrigin[indexColumn] = xStart;

        xStart += m_columnsStretches[indexColumn] * cw;
      }

      return columnsOrigin;
    }

    inline
    std::vector<float>
    GridLayout::computeRowsOrigin(const float& rh) const noexcept {
      // Create the return value.
      std::vector<float> rowsOrigin(m_rows, 0.0f);

      float yStart = m_margin;

      // Traverse each row and use its stretch to coppute its origin.
      for (int indexRow = 0 ; indexRow < m_rows ; ++indexRow) {
        rowsOrigin[indexRow] = yStart;

        yStart += m_rowsStretches[indexRow] * rh;
      }

      return rowsOrigin;
    }

    inline
    void
    GridLayout::computeWidgetSpan(const ItemInfo& info,
                                  const float& cw,
                                  const float& rh,
                                  float& w,
                                  float& h) const noexcept
    {
      // Initialize output values.
      w = 0.0f;
      h = 0.0f;

      // Traverse each column covered by the widget and update the total width.
      for (int indexColumn = info.x ; indexColumn < info.x + info.w ; ++indexColumn) {
        w += m_columnsStretches[indexColumn] * cw;
      }

      // Traverse each row covered by the widget and update the total height.
      for (int indexRow = info.y ; indexRow < info.y + info.h ; ++indexRow) {
        h += m_rowsStretches[indexRow] * rh;
      }
    }

  }
}

#endif    /* GRIDLAYOUT_HXX */
