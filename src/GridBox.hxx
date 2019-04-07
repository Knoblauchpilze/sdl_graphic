#ifndef    GRIDBOX_HXX
# define   GRIDBOX_HXX

# include "GridBox.hh"
# include <iomanip>

namespace sdl {
  namespace graphic {

    inline
    std::size_t
    GridBox::width() const noexcept {
      return m_width;
    }

    inline
    std::size_t
    GridBox::height() const noexcept {
      return m_height;
    }

    inline
    void
    GridBox::insert(const int& id,
                    const unsigned& x,
                    const unsigned& y,
                    const unsigned& spanX,
                    const unsigned& spanY)
    {
      // Check dimensions against internal structure.
      if (x >= m_width) {
        error(
          std::string("Could not insert grid box item ") + std::to_string(id) +
          " at " + std::to_string(x) + "x" + std::to_string(y),
          std::string("Out of range dimensions")
        );
      }

      if (y >= m_height) {
        error(
          std::string("Could not insert grid box item ") + std::to_string(id) +
          " at " + std::to_string(x) + "x" + std::to_string(y),
          std::string("Out of range dimensions")
        );
      }

      if (x + spanX > m_width) {
        error(
          std::string("Could not insert grid box item ") + std::to_string(id) +
          " at " + std::to_string(x) + "x" + std::to_string(y),
          std::string("Out of range dimensions")
        );
      }

      if (y + spanY >= m_height) {
        error(
          std::string("Could not insert grid box item ") + std::to_string(id) +
          " at " + std::to_string(x) + "x" + std::to_string(y),
          std::string("Out of range dimensions")
        );
      }

      // Register each cell spanned by the widget.
      for (unsigned row = 0u ; row < spanY ; ++row) {
        for (unsigned column = 0u ; column < spanX ; ++column) {
          CellInfo& data = cell(column, row);

          data.widget = id;
          data.master = (row == 0u && column == 0u);
          data.multiCell = (spanX > 1 || spanY > 1);
          data.refX = x;
          data.refY = y;
        }
      }
    }

    inline
    std::size_t
    GridBox::size() const noexcept {
      return m_grid.size();
    }

    inline
    void
    GridBox::initGrid() noexcept {
      for (unsigned id = 0u ; id < size() ; ++id) {
        const unsigned x = id % m_width;
        const unsigned y = id / m_width;
        cell(id) = CellInfo{
          x,
          y,
          -1,
          true,
          false,
          x,
          y,
          utils::Boxf()
        };
      }
    }

    inline
    const GridBox::CellInfo&
    GridBox::cell(const unsigned& x,
                  const unsigned& y) const noexcept
    {
      return cell(y * m_width + x);
    }

    inline
    GridBox::CellInfo&
    GridBox::cell(const unsigned& x,
                  const unsigned& y) noexcept
    {
      return cell(y * m_width + x);
    }

    inline
    const GridBox::CellInfo&
    GridBox::cell(const unsigned& id) const noexcept {
      return m_grid[id];
    }

    inline
    GridBox::CellInfo&
    GridBox::cell(const unsigned& id) noexcept {
      return m_grid[id];
    }

  }
}

#endif    /* GRIDBOX_HXX */
