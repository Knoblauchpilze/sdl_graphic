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
        setColumnMinimumWidth(const unsigned& column, const float& width);

        void
        setRowMinimumHeight(const unsigned& row, const float& height);

        void
        setColumnMaximumWidth(const unsigned& column, const float& width);

        void
        setRowMaximumHeight(const unsigned& row, const float& height);

        void
        setColumnHorizontalStretch(const unsigned& column, const float& stretch);

        void
        setRowVerticalStretch(const unsigned& row, const float& stretch);

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

        sdl::utils::Sizef
        computeAvailableSize(const sdl::utils::Boxf& totalArea) const noexcept;

        sdl::utils::Sizef
        computeDefaultWidgetBox(const sdl::utils::Sizef& area,
                                const unsigned& columnsCount,
                                const unsigned& rowsCount) const noexcept;

        // Convenience record to hold the position of items in the layout.
        struct ItemInfo {
          unsigned x, y, w, h;
        };

        void
        computeColumnsWidth(const float& totalWidth, float& cw) const noexcept;

        void
        computeRowsHeight(const float& totalHeight, float& rh) const noexcept;

        std::vector<float>
        computeColumnsOrigin(const float& cw) const noexcept;

        std::vector<float>
        computeRowsOrigin(const float& rh) const noexcept;

        void
        computeWidgetSpan(const ItemInfo& info,
                          const float& cw,
                          const float& rh,
                          float& w,
                          float& h) const noexcept;

      private:

        unsigned m_columns;
        unsigned m_rows;

        std::vector<float> m_columnsMinimumWidth;
        std::vector<float> m_columnsMaximumWidth;
        std::vector<float> m_rowsMinimumHeight;
        std::vector<float> m_rowsMaximumHeight;

        std::vector<float> m_columnsStretches;
        std::vector<float> m_rowsStretches;

        float m_margin;
        std::unordered_map<int, ItemInfo> m_itemsLocation;

    };

    using GridLayoutShPtr = std::shared_ptr<GridLayout>;
  }
}

# include "GridLayout.hxx"

#endif    /* GRIDLAYOUT_HH */
