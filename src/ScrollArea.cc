
# include "ScrollArea.hh"
# include "ScrollableWidget.hh"

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

      m_propsLocker(),

      m_cornerName(),
      m_hBarName(),
      m_vBarName()
    {
      build();
    }

    ScrollArea::~ScrollArea() {}

    void
    ScrollArea::setCornerWidget(core::SdlWidget* corner) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // First thing is to remove any existing corner widget.
      core::SdlWidget* wid = getChildOrNull<core::SdlWidget>(m_cornerName);

      // If this item is not null we need to remove it.
      if (wid != nullptr) {
        removeItem(wid);
      }

      // Now perform the insertion of the input corner widget
      // if it is valid.
      if (corner != nullptr) {
        // Insert the input widget as child of this widget so that it gets redrawn.
        corner->setParent(this);

        // Assign the new name of the corner widget.
        m_cornerName = corner->getName();

        // We rely on the internal layout method to perform the insertion.
        getLayout().addItem(corner, 1, 1, 1, 1);
      }
    }

    void
    ScrollArea::setHorizontalScrollBar(ScrollBar* scrollBar) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // First thing is to remove any existing scroll bar.
      ScrollBar* bar = getChildOrNull<ScrollBar>(m_hBarName);

      // If this item is not null we need to remove it.
      if (bar != nullptr) {
        removeItem(bar);
      }

      // Now perform the insertion of the input corner widget
      // if it is valid.
      if (scrollBar != nullptr) {
        // Insert the input widget as child of this widget so that it gets redrawn.
        scrollBar->setParent(this);

        // Assign the new name of the scroll bar.
        m_hBarName = scrollBar->getName();

        // We rely on the internal layout method to perform the insertion.
        getLayout().addItem(scrollBar, 0, 1, 1, 1);
      }
    }

    void
    ScrollArea::setVerticalScrollBar(ScrollBar* scrollBar) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // First thing is to remove any existing scroll bar.
      ScrollBar* bar = getChildOrNull<ScrollBar>(m_vBarName);

      // If this item is not null we need to remove it.
      if (bar != nullptr) {
        removeItem(bar);
      }

      // Now perform the insertion of the input corner widget
      // if it is valid.
      if (scrollBar != nullptr) {
        // Insert the input widget as child of this widget so that it gets redrawn.
        scrollBar->setParent(this);

        // Assign the new name of the scroll bar.
        m_vBarName = scrollBar->getName();

        // We rely on the internal layout method to perform the insertion.
        getLayout().addItem(scrollBar, 1, 0, 1, 1);
      }
    }

    void
    ScrollArea::setViewport(core::SdlWidget* viewport) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Note that we're not directly handling the viewport widget but instead
      // delegate most of it to the internal scrollable widget. We will do this
      // also in this case where we will just indicate to the scrollable object
      // to update its content.
      ScrollableWidget* wid = getViewportHandler();

      // Assign the viewport to the scrollable handler: it will handle whether
      // it is valid to specify a `null` widget or any kind of widget itself.
      wid->setSupport(viewport);
    }

    void
    ScrollArea::updatePrivate(const utils::Boxf& window) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Compare the input size of the window and the preferred size of the
      // viewport: if the preferred is larger than the newly assigned size,
      // we need to display the scroll bars if this is permitted by policies.
      // We only do that if this is not already the case.
      // On the other hand if the window size is now larger than the size
      // desired by the viewport we can hide the scroll bars.
      utils::Sizef current = window.toSize();

      bool needHBar = isHSBarVisible(current.w());
      bool needVBar = isVSBarVisible(current.h());

      // Handle horizontal scroll bar.
      ScrollBar* hBar = getChildOrNull<ScrollBar>(m_hBarName);
      if (hBar != nullptr && needHBar && !hBar->isVisible()) {
        hBar->setVisible(true);
      }

      // Handle vertical scroll bar.
      ScrollBar* vBar = getChildOrNull<ScrollBar>(m_vBarName);
      if (vBar != nullptr && needVBar && !vBar->isVisible()) {
        vBar->setVisible(true);
      }
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
      ScrollBar* hBar = new ScrollBar(
        getHBarName(),
        ScrollBar::Orientation::Horizontal,
        core::engine::Color::NamedColor::Magenta,
        this
      );
      if (hBar == nullptr) {
        error(
          std::string("Could not create scroll area"),
          std::string("Failed to allocate memory to store horizontal scroll bar")
        );
      }
      hBar->setMaxSize(utils::Sizef(std::numeric_limits<float>::max(), 100.0f));

      ScrollBar* vBar = new ScrollBar(
        getVBarName(),
        ScrollBar::Orientation::Vertical,
        core::engine::Color::NamedColor::Yellow,
        this
      );
      if (vBar == nullptr) {
        error(
          std::string("Could not create scroll area"),
          std::string("Failed to allocate memory to store vertical scroll bar")
        );
      }
      vBar->setMaxSize(utils::Sizef(100.0f, std::numeric_limits<float>::max()));

      // Create the scrollable widget.
      ScrollableWidget* viewport = new ScrollableWidget(getViewportName(), this);
      if (viewport == nullptr) {
        error(
          std::string("Could not create scroll area"),
          std::string("Failed to allocate memory to store the viewport")
        );
      }

      // Add scroll bars and viewport to the layout.
      grid->addItem(hBar,     0, 1, 1, 1);
      grid->addItem(vBar,     1, 0, 1, 1);
      grid->addItem(viewport, 0, 0, 1, 1);
    }

  }
}
