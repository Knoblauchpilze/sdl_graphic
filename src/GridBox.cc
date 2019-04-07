
# include "GridBox.hh"

namespace sdl {
  namespace graphic {

    GridBox::GridBox(const unsigned& width,
                     const unsigned& height,
                     const std::string& name):
      utils::CoreObject(name),
      m_width(width),
      m_height(height),
      m_grid(width * height)
    {
      initGrid();
    }

    GridBox::~GridBox() {}


  }
}
