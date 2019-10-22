
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
    {
      build();
    }

    ScrollArea::~ScrollArea() {}

    void
    ScrollArea::setCornerWidget(core::SdlWidget* corner) {
      // First thing is to remove any existing corner widget.
      core::SdlWidget* wid = getChildOrNull<core::SdlWidget>(getCornerWidgetName());

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
      ScrollBar* bar = getChildOrNull<ScrollBar>(getHBarName());

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
      ScrollBar* bar = getChildOrNull<ScrollBar>(getVBarName());

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
      core::SdlWidget* wid = getChildOrNull<core::SdlWidget>(getViewportName());

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

    void
    ScrollArea::build() {
      // Create the layout for this widget: the general disposition makes the
      // use of a `GridLyaout` quite natural so we'll go with this. The scroll
      // bars should be clamped in size because we don't really need them to
      // become too big.

      // Create the layout and check for errors.
      GridLayoutShPtr grid = std::make_shared<GridLayout>(
        std::string("grid_layout_for_scroll_area"),
        this,
        2u,
        2u,
        0.0f
      );

      if (grid == nullptr) {
        error(
          std::string("Could not create scroll area"),
          std::string("Failed to allocate memory to store main layout")
        );
      }

      // Assign the layout to this widget.
      setLayout(grid);

      // Create scroll bars.
      ScrollBar* hBar = new ScrollBar(getHBarName(), ScrollBar::Orientation::Horizontal, this);
      if (hBar == nullptr) {
        error(
          std::string("Could not create scroll area"),
          std::string("Failed to allocate memory to store horizontal scroll bar")
        );
      }
      hBar->setMaxSize(utils::Sizef(std::numeric_limits<float>::max(), 100.0f));

      ScrollBar* vBar = new ScrollBar(getVBarName(), ScrollBar::Orientation::Vertical, this);
      if (vBar == nullptr) {
        error(
          std::string("Could not create scroll area"),
          std::string("Failed to allocate memory to store vertical scroll bar")
        );
      }
      vBar->setMaxSize(utils::Sizef(100.0f, std::numeric_limits<float>::max()));

      // Add scroll bars to the layout.
      grid->addItem(hBar, 0, 1, 1, 1);
      grid->addItem(vBar, 1, 0, 1, 1);
    }

  }
}
