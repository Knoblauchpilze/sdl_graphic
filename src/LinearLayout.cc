
# include "LinearLayout.hh"
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    LinearLayout::LinearLayout(const Direction& direction,
                               const float& margin,
                               const float& interMargin,
                               sdl::core::SdlWidget* widget):
      sdl::core::Layout(widget),
      m_direction(direction),
      m_margin(margin),
      m_componentMargin(interMargin)
    {
      // Nothing to do.
    }

    LinearLayout::~LinearLayout() {}

    void
    LinearLayout::updatePrivate(const sdl::core::Boxf& window) {
      // Split the available space according to the number of elements to space.
      float cw, ch;
      switch(m_direction) {
        case Direction::Horizontal:
          handleHorizontalLayout(window, cw, ch);
          break;
        case Direction::Vertical:
        default:
          handleVerticalLayout(window, cw, ch);
          break;
      }

      // Apply the computed dimensions to each item handled by the layout.
      for (int indexItem = 0 ; indexItem < m_items.size() ; ++indexItem) {
        // Compute the position of the component.
        const float x = m_margin + (m_direction == Direction::Horizontal ? indexItem * (cw + m_componentMargin) : 0.0f);
        const float y = m_margin + (m_direction == Direction::Vertical   ? indexItem * (ch + m_componentMargin) : 0.0f);

        // Apply the final rendering area.
        m_items[indexItem]->setRenderingArea(sdl::core::Boxf(x + cw / 2.0f, y + ch / 2.0f, cw, ch));
      }
    }

  }
}
