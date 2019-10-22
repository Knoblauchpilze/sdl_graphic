#ifndef    GRIDLAYOUT_HH
# define   GRIDLAYOUT_HH

# include <memory>
# include <unordered_map>
# include <sdl_core/Layout.hh>

namespace sdl {
  namespace graphic {

    class GridLayout: public core::Layout {
      public:

        GridLayout(const std::string& name,
                   core::SdlWidget* widget,
                   unsigned columns,
                   unsigned rows,
                   float margin = 0.0f);

        virtual ~GridLayout();

        unsigned
        getColumnCount() const noexcept;

        unsigned
        getRowCount() const noexcept;

        void
        setColumnHorizontalStretch(unsigned column, float stretch);

        void
        setColumnMinimumWidth(unsigned column, float width);

        void
        setColumnsMinimumWidth(float width);

        void
        setRowVerticalStretch(unsigned row, float stretch);

        void
        setRowMinimumHeight(unsigned row, float height);

        void
        setRowsMinimumHeight(float height);

        void
        addItem(core::LayoutItem* container,
                unsigned x,
                unsigned y,
                unsigned w,
                unsigned h) override;

        void
        setGrid(unsigned columns,
                unsigned rows);

      protected:

        void
        computeGeometry(const utils::Boxf& area) override;

        /**
         * @brief - Reimplementation of the base `Layout` method to provide update of the
         *          internal associations table between the logical id and physical id.
         * @param logicID - the logical id which has just been removed.
         * @param physID - the physical id which has just been removed.
         * @return - true as this layout always needs a rebuild when an item is removed.
         */
        bool
        onIndexRemoved(const int logicID,
                       const int physID) override;

      protected:

        // Convenience record to hold the position of items in the layout.
        struct ItemInfo {
          unsigned x, y, w, h;
          core::LayoutItem* item;
        };

        // Convenience record holding the information for a single column/row.
        struct LineInfo {
          unsigned stretch;
          float min;
        };

        // Convenience record holding the detailed information for a single cell
        // of the grid layout. If the `item` value is negative, it means that
        // no item occupy the location. In any other case, the `item` value
        // corresponds to the key to reach the item spanning this cell in the
        // `m_locations` map.
        // The `multiCell` value indicates whether the item is a multi-cell
        // item in which case the `hStretch` and `vStretch` should be ignored.
        struct CellInfo {
          unsigned hStretch;
          unsigned vStretch;
          utils::Boxf box;
          bool multiCell;
          int item;
        };

        // Convenience record holding the data representing a item during the
        // optimiwation process. It allows to represent easily multi-cell item
        // by providing a way to link cells' information.
        // The basic information corresponds to the index of the item it is
        // associated in some array.
        // The `shared` attribute allows to detezrmine whether this item is
        // shared and the `master` allows to specify whether it is the first
        // occurrence of this item's data in the array. Indeed most processes
        // want to work on a per-item basis and don't really care about
        // duplicates.
        // One can access to the total extent of a item through the `span`
        // value which counts how many `ItemData` element with a similar `item`
        // can be found.
        // For any item there's only one instance of item data which has its
        // `master` value set to true.
        // An identifier shall be provided for this data in order to be able to
        // use it correctly in unordered set. A common value corresponds to the
        // cell id (which is unique within a grid layout computed as follows:
        // `y * m_columns + x`.
        // The `suze` allows to determine which portion of the overall `box`
        // assigned for this item belongs to this piece of data. This allows
        // for a better repartition of size among the cells spanned by a item.
        struct ItemData {
          int item;
          bool shared;
          bool master;
          unsigned span;
          unsigned id;
          mutable utils::Sizef size;
        };
        using ItemDataShPtr = std::shared_ptr<ItemData>;

        // Convenience structure which allows to share data of item but still
        // keeping the identifier property.
        struct ItemDataWrapper {
          unsigned id;
          ItemDataShPtr data;
        };

        void
        resetGridInfo();

        /**
         * @brief - Updates the coordinates of the item at index `item` with the provided
         *          box.
         *          If no such item ccan be found, an error is raised.
         * @param item - the index of the item to update.
         * @param coordinates - the new coordinates to assign to this item.
         */
        void
        updateGridCoordinates(int item,
                              const utils::Boxi& coordinates);

        std::vector<CellInfo>
        computeCellsInfo() const noexcept;

        virtual void
        adjustItemToConstraints(const utils::Sizef& window,
                                std::vector<WidgetInfo>& items) const noexcept;

        utils::Sizef
        computeAchievedSize(const std::vector<ItemDataWrapper>& elements) const noexcept;

        std::vector<float>
        adjustColumnsWidth(const utils::Sizef& window,
                           const std::vector<WidgetInfo>& items,
                           std::vector<CellInfo>& cells) const;

        std::vector<float>
        adjustRowHeight(const utils::Sizef& window,
                        const std::vector<WidgetInfo>& items,
                        std::vector<CellInfo>& cells) const;

        void
        adjustMultiCellWidth(const std::vector<float>& columns,
                             const std::vector<WidgetInfo>& items,
                             std::vector<CellInfo>& cells);

        void
        adjustMultiCellHeight(const std::vector<float>& rows,
                              const std::vector<WidgetInfo>& items,
                              std::vector<CellInfo>& cells);

      private:

        friend std::hash<ItemDataWrapper>;
        friend bool operator==(const ItemDataWrapper& lhs, const ItemDataWrapper& rhs) noexcept;

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
# include "GridLayout_specialization.hxx"

#endif    /* GRIDLAYOUT_HH */
