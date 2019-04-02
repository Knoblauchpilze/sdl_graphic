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

        // TODO: Allow widgets to span multiple cells.
        // Right now we only provide support for widget
        // spanning a single cell, which could be enough
        // for our needs.
        // Note that from this link:
        // https://github.com/openwebos/qt/blob/master/src/gui/kernel/qgridlayout.cpp
        // And more precisely in this function: `distributeMultiBox`
        // It seems that Qt does proceed in several passes:
        // one to place and layout single cell widgets and
        // then a second pass to layout multi-cell widgets.
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
        updatePrivate(const utils::Boxf& area) override;

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

        // Convenience record holding the detailed information for a single cell
        // of the grid layout. If the `widget` value is negative, it means that
        // no widget occupy the location. In any other case, the `widget` value
        // corresponds to the key to reach the widget spanning this cell in the
        // `m_locations` map.
        // The `multiCell` value indicates whether the widget is a multi-cell
        // widget in which case the `hStretch` and `vStretch` should be ignored.
        struct CellInfo {
          unsigned hStretch;
          unsigned vStretch;
          utils::Boxf box;
          bool multiCell;
          int widget;
        };

        void
        resetGridInfo();

        /////////
        // Old //
        /////////

        utils::Sizef
        computeDefaultWidgetBox(const utils::Sizef& area,
                                const unsigned& columnsCount,
                                const unsigned& rowsCount) const noexcept;

        void
        consolidateDimensions(std::vector<CellInfo>& cells,
                              std::vector<float>& columnsDims,
                              std::vector<float>& rowsDims) const noexcept;

        utils::Sizef
        computeSizeOfCells(const std::vector<CellInfo>& cells) const;

        /////////
        // Old //
        /////////

        float
        allocateFairly(const float& space,
                       const unsigned& count) const noexcept;

        std::vector<CellInfo>
        computeCellsInfo() const noexcept;

        void
        adjustWidgetToConstraints(std::vector<WidgetInfo>& widgets) const noexcept;

        utils::Sizef
        computeAchievedSize(const std::vector<unsigned>& elements,
                            const std::vector<CellInfo>& cells) const noexcept;

        std::vector<float>
        adjustColumnsWidth(const utils::Sizef& window,
                           const std::vector<WidgetInfo>& widgets,
                           std::vector<CellInfo>& cells) const;

        std::vector<float>
        adjustRowHeight(const utils::Sizef& window,
                        const std::vector<WidgetInfo>& widgets,
                        std::vector<CellInfo>& cells) const;

      private:

        using LocationsMap = std::unordered_map<int, ItemInfo>;

        unsigned m_columns;
        unsigned m_rows;

        std::vector<LineInfo> m_columnsInfo;
        std::vector<LineInfo> m_rowsInfo;

        LocationsMap m_locations;

    };

    using GridLayoutShPtr = std::shared_ptr<GridLayout>;
  }
}

# include "GridLayout.hxx"

#endif    /* GRIDLAYOUT_HH */
