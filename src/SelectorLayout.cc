
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

      // Assign the space for the active child: as this is the only
      // child, use all the available space.
      std::vector<utils::Boxf> bboxes(getItemsCount(), utils::Boxf());

      bboxes[m_activeItem] = utils::Boxf(
        getMargin().w() + componentSize.w() / 2.0f,
        getMargin().h() + componentSize.h() / 2.0f,
        componentSize.w(),
        componentSize.h()
      );

      // Use the base handler to assign bbox.
      assignRenderingAreas(bboxes, window);

      // Disable other items.
      std::vector<bool> visible(getItemsCount(), false);
      visible[m_activeItem] = true;
      assignVisibilityStatus(visible);
    }

  }
}
