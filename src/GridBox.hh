#ifndef    GRIDBOX_HH
# define   GRIDBOX_HH

# include <memory>
# include <vector>
# include <unordered_set>
# include <core_utils/CoreObject.hh>
# include <maths_utils/Box.hh>

namespace sdl {
  namespace graphic {

    class GridBox: public utils::CoreObject {
      public:

        GridBox(const unsigned& width,
                const unsigned& height,
                const std::string& name = std::string("gridbox"));

        virtual ~GridBox();

        std::size_t
        width() const noexcept;

        std::size_t
        height() const noexcept;

        void
        insert(const int& id,
               const unsigned& x,
               const unsigned& y,
               const unsigned& spanX,
               const unsigned& spanY);

      private:

        std::size_t
        size() const noexcept;

        void
        initGrid() noexcept;

      private:

        // Convenience record holding the detailed information for a single cell
        // of the grid box. Each cell has its own coordinates registered in the
        // `x` and `y` attribute.
        // A cell can be assigned to a widget through the `widget` value: if no
        // widget is associated to a cell (i.e. the cell is empty) the `widget`
        // value is negative (usually `-1`).
        // All the other information are then irrelevant except the `box` value.
        //
        // The `box` value describes the current geometry of the cell, and is
        // updated based on the constraints applied to the cell.
        //
        // The cell can be linked to other cells in case a multi-cell widget is
        // assiged to it.
        // The root cell has the `master` field set to `true`: it corresponds to
        // the top left cell spanned by the widget.
        // If a single-cell widget is assigned to the cell this value is always
        // set to `true`.
        // The `multiCell` value controls whether the `refX` and `refY` values
        // are relevant: these values correspond to the coordinate of the cell
        // where the cell is assigned to the same widget but the `master` value
        // is set to true.
        struct CellInfo {
          unsigned x;
          unsigned y;
          int widget;
          bool master;
          bool multiCell;
          unsigned refX;
          unsigned refY;
          utils::Boxf box;
        };

        const CellInfo&
        cell(const unsigned& x,
             const unsigned& y) const noexcept;

        CellInfo&
        cell(const unsigned& x,
             const unsigned& y) noexcept;

        const CellInfo&
        cell(const unsigned& id) const noexcept;

        CellInfo&
        cell(const unsigned& id) noexcept;

      private:

        using Grid = std::vector<CellInfo>;

        unsigned m_width;
        unsigned m_height;
        Grid m_grid;
    };

    using GridBoxShPtr = std::shared_ptr<GridBox>;
  }
}

# include "GridBox.hxx"

#endif    /* GRIDBOX_HH */
