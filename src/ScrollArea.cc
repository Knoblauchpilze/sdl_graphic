
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
      m_vBarName(),

      m_hBarSignalID(utils::Signal<const std::string, float, float, float>::NoID),
      m_vBarSignalID(utils::Signal<const std::string, float, float, float>::NoID)
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
        // Disconnect from the signal indicating a value change.
        bar->onValueChanged.disconnect(m_hBarSignalID);

        // Remove the item from the layout.
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

        // Connect the value changed signal to the local handler.
        m_hBarSignalID = scrollBar->onValueChanged.connect_member<ScrollArea>(this, &ScrollArea::onControlScrolled);
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
        // Disconnect from the signal indicating a value change.
        bar->onValueChanged.disconnect(m_vBarSignalID);

        // Remove the item from the layout.
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

        // Connect the value changed signal to the local handler.
        m_vBarSignalID = scrollBar->onValueChanged.connect_member<ScrollArea>(this, &ScrollArea::onControlScrolled);
      }
    }

    void
    ScrollArea::setViewport(core::SdlWidget* viewport) {
      // Check consistency.
      if (viewport == nullptr) {
        error(
          std::string("Could not assign new viewport to scroll area"),
          std::string("New viewport is null")
        );
      }

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

      // Perform the update of the controls.
      updateControls(getRenderingArea().toSize());
    }

    void
    ScrollArea::updatePrivate(const utils::Boxf& window) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Use the dedicated handler.
      updateControls(window.toSize());


      // Also call the parent method to benefit from the base class behavior.
      core::SdlWidget::updatePrivate(window);
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

      // Create the scrollable widget.
      ScrollableWidget* viewport = new ScrollableWidget(getViewportName(), this);
      if (viewport == nullptr) {
        error(
          std::string("Could not create scroll area"),
          std::string("Failed to allocate memory to store the viewport")
        );
      }

      // Add scroll bars and viewport to the layout.
      setHorizontalScrollBar(hBar);
      setVerticalScrollBar(vBar);
      grid->addItem(viewport, 0, 0, 1, 1);
    }

    void
    ScrollArea::updateControls(const utils::Sizef& internal) {
      // This method should update the visiblility status of the scroll bars
      // based on whether they should be visible and also update their range
      // so that the indication is correct compared to how the size of the
      // scrollable widget compares to the size of the scroll area.

      // Assign visibility statuses for scroll bars.
      float leftW, leftH;
      bool needHBar = isHSBarVisible(internal.w(), &leftW);
      bool needVBar = isVSBarVisible(internal.h(), &leftH);

      utils::Sizef sHBar;
      utils::Sizef sVBar;

      ScrollBar* hBar = getChildOrNull<ScrollBar>(m_hBarName);
      ScrollBar* vBar = getChildOrNull<ScrollBar>(m_vBarName);

      // Disclaimer: note that retrieving the size of the scroll bars here might
      // not be the best strategy as we might modify it right away because we're
      // processing a resize event here. So we should maybe try to determine in
      // some other ways the size that the scroll bars will reach given the input
      // `internal` size (which *is* applied to this component).
      // As for now the small size of the scroll bars makes it very probable that
      // no matter the input `internal` size the dimensions of the scroll bars
      // will not be modified, hence the fact that we can use it and still get
      // accurate results.
      if (hBar != nullptr) {
        sHBar = hBar->getRenderingArea().toSize();
      }
      if (vBar != nullptr) {
        sVBar = vBar->getRenderingArea().toSize();
      }

      // Update visibility status in case one of the status bar makes the
      // available space too small for the other. Typically imagine the
      // following scenario:
      //  - available space is 1500x300
      //  - size needed for the widget is 1550x280
      //  - size of scroll bars are (h: 1500x30, v: 30x300)
      //  - there is enough space to display the widget vertically
      //  - but not enough horizontally
      //  - we thus display the horizontal scroll bar
      //  - and now there's not enough space vertically
      if (needHBar && sHBar.h() > leftH) {
        needVBar = true;
      }
      if (needVBar && sVBar.w() > leftW) {
        needHBar = true;
      }

      // Handle horizontal scroll bar.
      if (hBar != nullptr && needHBar) {
        if (!hBar->isVisible()) {
          hBar->setVisible(true);
        }
      }
      else {
        if (hBar->isVisible()) {
          hBar->setVisible(false);
        }
      }

      // Handle vertical scroll bar.
      if (vBar != nullptr && needVBar) {
        if (!vBar->isVisible()) {
          vBar->setVisible(true);
        }
      }
      else {
        if (vBar->isVisible()) {
          vBar->setVisible(false);
        }
      }

      // Also set the dimensions of the scroll bars based on the ratio between
      // the size of this component and the size of the displayed widget.
      utils::Sizef viewport = getMaximumViewportSize();

      float hRatio = (internal.w() - sVBar.w()) / viewport.w();
      float vRatio = (internal.h() - sHBar.h()) / viewport.h();

      // Now that we have a ratio we can deduce the expected range of the scroll
      // bar and the corresponding page step.
      int minH = 0;
      int maxH = static_cast<int>(viewport.w());
      int stepH = static_cast<int>(hRatio * viewport.w());

      int minV = 0;
      int maxV = static_cast<int>(viewport.h());
      int stepV = static_cast<int>(vRatio * viewport.h());

      // Assign the size of the controls to each scroll bar.
      if (hBar != nullptr && needHBar) {
        log(
          "Setting range to [" + std::to_string(minH) + " - " + std::to_string(stepH) +
          " - " + std::to_string(maxH) + "] for " + hBar->getName()
        );
        hBar->setRange(minH, stepH, maxH);
      }

      if (vBar != nullptr && needVBar) {
        log(
          "Setting range to [" + std::to_string(minV) + " - " + std::to_string(stepV) +
          " - " + std::to_string(maxV) + "] for " + hBar->getName()
        );
        vBar->setRange(minV, stepV, maxV);
      }
    }

    void
    ScrollArea::onControlScrolled(const std::string& name,
                                  float min,
                                  float max)
    {
      // TODO: Handle modification of the viewport by changing its rendering area.
      log(
        "Should handle scrolling from \"" + name + "\" to [" +
        std::to_string(min) + " - " + std::to_string(max) + "]",
        utils::Level::Warning
      );
    }

  }
}
