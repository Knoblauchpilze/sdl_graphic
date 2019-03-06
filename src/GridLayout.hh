#ifndef    GRIDLAYOUT_HH
# define   GRIDLAYOUT_HH

# include <memory>
# include <unordered_map>
# include <sdl_core/Layout.hh>

namespace sdl {
  namespace graphic {

    class GridLayout: public sdl::core::Layout {
      public:

        GridLayout(const unsigned& columns,
                   const unsigned& rows,
                   const float& margin = 0.0f,
                   sdl::core::SdlWidget* widget = nullptr);

        virtual ~GridLayout();

        const unsigned&
        getColumnCount() const noexcept;

        const unsigned&
        getRowCount() const noexcept;

        const float&
        getMargin() const noexcept;

        void
        setColumnHorizontalStretch(const unsigned& column, const float& stretch);

        void
        setColumnMinimumWidth(const unsigned& column, const float& width);

        void
        setColumnsMinimumWidth(const float& width);

        void
        setRowVerticalStretch(const unsigned& row, const float& stretch);

        void
        setRowMinimumHeight(const unsigned& row, const float& height);

        void
        setRowsMinimumHeight(const float& height);

        int
        addItem(sdl::core::SdlWidget* container,
                const unsigned& x,
                const unsigned& y,
                const unsigned& w,
                const unsigned& h) override;

        void
        setGrid(const unsigned& columns, const unsigned& rows);

      protected:

        void
        updatePrivate(const sdl::utils::Boxf& area) override;

      private:

        // Convenience record to hold the position of items in the layout.
        struct ItemInfo {
          unsigned x, y, w, h;
        };

        // Convenience record holding the information for a single column/row.
        struct LineInfo {
          unsigned stretch;
          float min;
        };

        void
        resetGridInfo();

        sdl::utils::Sizef
        computeAvailableSize(const sdl::utils::Boxf& totalArea) const noexcept;

        sdl::utils::Sizef
        computeDefaultWidgetBox(const sdl::utils::Sizef& area,
                                const unsigned& columnsCount,
                                const unsigned& rowsCount) const noexcept;

        bool
        locationSpanColumn(const unsigned& column,
                           const ItemInfo& info) const noexcept;

        bool
        locationSpanRow(const unsigned& row,
                        const ItemInfo& info) const noexcept;

        std::vector<float>
        computeColumnsDimensions() const noexcept;

        std::vector<float>
        computeRowsDimensions() const noexcept;

      private:

        unsigned m_columns;
        unsigned m_rows;

        std::vector<LineInfo> m_columnsInfo;
        std::vector<LineInfo> m_rowsInfo;

        float m_margin;
        std::unordered_map<int, ItemInfo> m_itemsLocation;

    };

    using GridLayoutShPtr = std::shared_ptr<GridLayout>;
  }
}

# include "GridLayout.hxx"

#endif    /* GRIDLAYOUT_HH */
