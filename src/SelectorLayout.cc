
# include <sdl_core/SdlWidget.hh>
# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    SelectorLayout::SelectorLayout(const float& margin,
                                   sdl::core::SdlWidget* widget):
      sdl::core::Layout(widget),
      m_margin(margin),
      m_activeItem(-1)
    {
      // Nothing to do.
    }

    SelectorLayout::~SelectorLayout() {}

    void
    SelectorLayout::updatePrivate(const sdl::core::Boxf& window) {
      // Check whether a child is active.
      if (m_activeItem < 0) {
        return;
      }

      // Compute the available space for the active child.
      const float cw = window.w() - 2.0f * m_margin;
      const float ch = window.h() - 2.0f * m_margin;

      // Assign the space for the active child.
      m_items[m_activeItem]->setRenderingArea(sdl::core::Boxf(m_margin + cw / 2.0f, m_margin + ch / 2.0f, cw, ch));

      // Disable other items.
      for (unsigned indexItem = 0u ; indexItem < m_items.size() ; ++indexItem) {
        m_items[indexItem]->setVisible(indexItem == m_activeItem);
      }
    }

  }
}
