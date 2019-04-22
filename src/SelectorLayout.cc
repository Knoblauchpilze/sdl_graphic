
# include <sdl_core/SdlWidget.hh>
# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    SelectorLayout::SelectorLayout(const float& margin,
                                   core::SdlWidget* widget):
      core::Layout(widget, margin),
      m_activeItem(-1)
    {
      // Nothing to do.
    }

    SelectorLayout::~SelectorLayout() {}

    void
    SelectorLayout::updatePrivate(const utils::Boxf& window) {
      // Check whether a child is active.
      if (m_activeItem < 0) {
        return;
      }

      // Compute the available space for the active child.
      const utils::Sizef componentSize = computeAvailableSize(window);

      // Assign the space for the active child.
      m_items[m_activeItem]->setRenderingArea(
        utils::Boxf(
          getMargin().w() + componentSize.w() / 2.0f,
          getMargin().h() + componentSize.h() / 2.0f,
          componentSize.w(),
          componentSize.h()
        )
      );

      // Disable other items.
      for (unsigned indexItem = 0u ; indexItem < getItemsCount() ; ++indexItem) {
        m_items[indexItem]->setVisible(static_cast<int>(indexItem) == m_activeItem);
      }
    }

  }
}
