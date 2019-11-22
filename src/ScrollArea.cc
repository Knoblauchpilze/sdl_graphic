
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

      m_hBarSignals(
        ScrollBarSignals{
          utils::Signal<const std::string, float, float, float>::NoID,
          utils::Signal<const std::string, float, float, float>::NoID
        }
      ),
      m_vBarSignals(
        ScrollBarSignals{
          utils::Signal<const std::string, float, float, float>::NoID,
          utils::Signal<const std::string, float, float, float>::NoID
        }
      ),

      m_orderData(LayoutData{nullptr, nullptr, nullptr, nullptr, nullptr})
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

        // Also we need to refresh the data for this corner widget in the virtual
        // layout so that we can still determine which elements are visible based
        // on the desired size.
        m_orderData.corner->setVisible(true);

        m_orderData.corner->setMinSize(corner->getMinSize());
        m_orderData.corner->setSizeHint(corner->getSizeHint());
        m_orderData.corner->setMaxSize(corner->getMaxSize());
      }
    }

    void
    ScrollArea::setHorizontalScrollBar(ScrollBar* scrollBar) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // First thing is to remove any existing scroll bar.
      ScrollBar* bar = getChildOrNull<ScrollBar>(m_hBarName);
      ScrollableWidget* viewport = getViewportHandler();

      // If this item is not null we need to remove it.
      if (bar != nullptr) {
        // Disconnect from the signals associated to this scroll bar.
        bar->onValueChanged.disconnect(m_hBarSignals.valueChangedID);
        viewport->onHorizontalAxisChanged.disconnect(m_hBarSignals.valueChangedID);

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

        // Also we need to refresh the data for this scroll bar in the virtual
        // layout so that we can still determine which elements are visible
        // based on the desired size.
        m_orderData.hBar->setVisible(true);

        m_orderData.hBar->setMinSize(scrollBar->getMinSize());
        m_orderData.hBar->setSizeHint(scrollBar->getSizeHint());
        m_orderData.hBar->setMaxSize(scrollBar->getMaxSize());

        // Connect the value changed and the axis changed signals to the dedicated
        // support widget's handler.
        m_hBarSignals.valueChangedID = scrollBar->onValueChanged.connect_member<ScrollableWidget>(
          viewport,
          &ScrollableWidget::onControlScrolled
        );

        m_hBarSignals.axisChangedID = viewport->onHorizontalAxisChanged.connect_member<ScrollBar>(
          scrollBar,
          &ScrollBar::setFromPercentage
        );
      }
    }

    void
    ScrollArea::setVerticalScrollBar(ScrollBar* scrollBar) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // First thing is to remove any existing scroll bar.
      ScrollBar* bar = getChildOrNull<ScrollBar>(m_vBarName);
      ScrollableWidget* viewport = getViewportHandler();

      // If this item is not null we need to remove it.
      if (bar != nullptr) {
        // Disconnect from the signal indicating a value change.
        bar->onValueChanged.disconnect(m_vBarSignals.valueChangedID);
        viewport->onVerticalAxisChanged.disconnect(m_vBarSignals.valueChangedID);

        // Remove the item from the layout.
        removeItem(bar);
      }

      // Now perform the insertion of the input corner widget
      // if it is valid.
      if (scrollBar != nullptr) {
        // Insert the input widget as child of this widget so that it gets
        // redrawn.
        scrollBar->setParent(this);

        // Assign the new name of the scroll bar.
        m_vBarName = scrollBar->getName();

        // We rely on the internal layout method to perform the insertion.
        getLayout().addItem(scrollBar, 1, 0, 1, 1);

        // Also we need to refresh the data for this scroll bar in the virtual
        // layout so that we can still determine which elements are visible
        // based on the desired size.
        m_orderData.vBar->setVisible(true);

        m_orderData.vBar->setMinSize(scrollBar->getMinSize());
        m_orderData.vBar->setSizeHint(scrollBar->getSizeHint());
        m_orderData.vBar->setMaxSize(scrollBar->getMaxSize());

        // Connect the value changed and the axis changed signals to the dedicated
        // support widget's handler.
        m_vBarSignals.valueChangedID = scrollBar->onValueChanged.connect_member<ScrollableWidget>(
          viewport,
          &ScrollableWidget::onControlScrolled
        );

        m_vBarSignals.axisChangedID = viewport->onVerticalAxisChanged.connect_member<ScrollBar>(
          scrollBar,
          &ScrollBar::setFromPercentage
        );
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

      // Note that we don't want to update the virtual item for this one because
      // it's precisely what will happen when the real layout will assign a size
      // to the support: it shoul dbe completely transparent and not request more
      // space than needed.

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

      // Remove the widget from the layout: both the real
      // layout and the virtual layout used to detect the
      // size of the elements.
      getLayout().removeItem(widget);

      if (widget->getName() == m_cornerName) {
        m_orderData.corner->setVisible(false);
      }
      if (widget->getName() == m_hBarName) {
        m_orderData.hBar->setVisible(false);
      }
      if (widget->getName() == m_vBarName) {
        m_orderData.vBar->setVisible(false);
      }

      // Now we can remove the input `widget` from the children' list.
      removeWidget(widget);
    }

    void
    ScrollArea::build() {
      // Create the layout for this widget: the general disposition makes the
      // use of a `GridLyaout` quite natural so we'll go with this. The scroll
      // bars should be clamped in size because we don't really need them to
      // become too big.
      // Also we should allocate the needed data to hold the predicted values
      // through the `m_orderData` attribute to replicate the architecture of
      // the scroll area and use the provided sizes.
      initLayoutData();

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
        scroll::Orientation::Horizontal,
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
        scroll::Orientation::Vertical,
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
    ScrollArea::initLayoutData() {
      // Initialize data.
      m_orderData.layout = nullptr;
      m_orderData.scrollable = std::make_shared<VirtualLayoutItem>(std::string("vitem_for_scrollable"));
      m_orderData.hBar = std::make_shared<VirtualLayoutItem>(std::string("vitem_for_hBar"));
      m_orderData.vBar = std::make_shared<VirtualLayoutItem>(std::string("vitem_for_vBar"));
      m_orderData.corner = std::make_shared<VirtualLayoutItem>(std::string("vitem_for_corner"));

      // Initialize the layout.
      m_orderData.layout = std::make_shared<GridLayout>(
        std::string("virtual_glayout_for_scroll_area"),
        this,
        2u,
        2u,
        0.0f
      );

      if (m_orderData.layout == nullptr) {
        error(
          std::string("Could not create scroll area"),
          std::string("Failed to allocate memory to store the layout")
        );
      }

      m_orderData.layout->addItem(m_orderData.scrollable.get(), 0, 0, 1, 1);
      m_orderData.layout->addItem(m_orderData.hBar.get(), 0, 1, 1, 1);
      m_orderData.layout->addItem(m_orderData.vBar.get(), 1, 0, 1, 1);
      m_orderData.layout->addItem(m_orderData.corner.get(), 1, 1, 1, 1);

      // The `scrollable` widget is always visible, the rest will be
      // assigned from the `build` method if needed.
      m_orderData.scrollable->setVisible(true);
      m_orderData.hBar->setVisible(false);
      m_orderData.vBar->setVisible(false);
      m_orderData.corner->setVisible(false);

      // Each component manage both the width and height of the corresponding
      // element.
      m_orderData.scrollable->setManageWidth(true);
      m_orderData.scrollable->setManageHeight(true);

      m_orderData.hBar->setManageWidth(true);
      m_orderData.hBar->setManageHeight(true);

      m_orderData.vBar->setManageWidth(true);
      m_orderData.vBar->setManageHeight(true);

      m_orderData.corner->setManageWidth(true);
      m_orderData.corner->setManageHeight(true);
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

      // Retrieve the size of each scroll bar in case we have a valid widget
      // assigned to it. To do so we need to request a new update of the
      // virtual layout: hopefully this will not happen too often and we can
      // recompute the whole size.
      m_orderData.layout->update(utils::Boxf::fromSize(internal, true));

      if (m_orderData.hBar->isVisible()) {
        sHBar = m_orderData.hBar->getRenderingArea().toSize();
      }
      if (m_orderData.vBar->isVisible()) {
        sVBar = m_orderData.vBar->getRenderingArea().toSize();
      }

      ScrollBar* hBar = getChildOrNull<ScrollBar>(m_hBarName);
      ScrollBar* vBar = getChildOrNull<ScrollBar>(m_vBarName);

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

      log(
        "Internal is " + internal.toString() + ", viewport is " + viewport.toString() + ", " +
        "hBar is " + sHBar.toString() + ", vBar is " + sVBar.toString()
      );

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

  }
}
