
# include "ScrollArea.hh"

namespace sdl {
  namespace graphic {

    ScrollArea::ScrollArea(const std::string& name,
                            core::SdlWidget* parent,
                            const utils::Sizef& area,
                            const BarPolicy& hBar,
                            const BarPolicy& vBar):
      core::SdlWidget(name, area, parent),

      m_hBarPolicy(hBar),
      m_vBarPolicy(vBar),

      m_propsLocker()
    {}

    ScrollArea::~ScrollArea() {}

    void
    ScrollArea::setCornerWidget(core::SdlWidget* corner) {
      // First thing is to remove any existing corner widget.
      core::SdlWidget* wid = getChildAs<core::SdlWidget>(getCornerWidgetName());

      // If this item is not null we need to remove it.
      if (wid != nullptr) {
        removeItem(wid);
      }

      // Now perform the insertion of the input corner widget
      // if it is valid.
      if (corner != nullptr) {
        // Insert the input widget as child of this widget so that it gets redrawn.
        corner->setParent(this);

        // We rely on the internal layout method to perform the insertion.
        getLayout().addItem(corner, 1, 1, 1, 1);
      }
    }

    void
    ScrollArea::setHorizontalScrollBar(ScrollBar* scrollBar) {
      // First thing is to remove any existing scroll bar.
      ScrollBar* bar = getChildAs<ScrollBar>(getHBarName());

      // If this item is not null we need to remove it.
      if (bar != nullptr) {
        removeItem(bar);
      }

      // Now perform the insertion of the input corner widget
      // if it is valid.
      if (scrollBar != nullptr) {
        // Insert the input widget as child of this widget so that it gets redrawn.
        scrollBar->setParent(this);

        // We rely on the internal layout method to perform the insertion.
        getLayout().addItem(scrollBar, 0, 1, 1, 1);
      }
    }

    void
    ScrollArea::setVerticalScrollBar(ScrollBar* scrollBar) {
      // First thing is to remove any existing scroll bar.
      ScrollBar* bar = getChildAs<ScrollBar>(getVBarName());

      // If this item is not null we need to remove it.
      if (bar != nullptr) {
        removeItem(bar);
      }

      // Now perform the insertion of the input corner widget
      // if it is valid.
      if (scrollBar != nullptr) {
        // Insert the input widget as child of this widget so that it gets redrawn.
        scrollBar->setParent(this);

        // We rely on the internal layout method to perform the insertion.
        getLayout().addItem(scrollBar, 1, 0, 1, 1);
      }
    }

    void
    ScrollArea::setViewport(core::SdlWidget* viewport) {
      // First thing is to remove any existing corner widget.
      core::SdlWidget* wid = getChildAs<core::SdlWidget>(getViewportName());

      // If this item is not null we need to remove it.
      if (wid != nullptr) {
        removeItem(wid);
      }

      // Now perform the insertion of the input corner widget
      // if it is valid.
      if (viewport != nullptr) {
        // Insert the input widget as child of this widget so that it gets redrawn.
        viewport->setParent(this);

        // We rely on the internal layout method to perform the insertion.
        getLayout().addItem(viewport, 0, 0, 1, 1);
      }
    }

    utils::Sizef
    ScrollArea::getMaximumViewportSize() const noexcept {
      return utils::Sizef();
    }

    void
    ScrollArea::removeItem(core::SdlWidget* widget) {
      // Check whether the item is valid.
      if (widget == nullptr) {
        error(
          std::string("Cannot remove widget from selector layout"),
          std::string("Invalid null item")
        );
      }

      // Remove the widget from the layout.
      getLayout().removeItem(widget);

      // Now we can remove the input `widget` from the children' list.
      removeWidget(widget);
    }

  }
}
