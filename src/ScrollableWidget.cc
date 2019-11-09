
# include "ScrollableWidget.hh"

namespace sdl {
  namespace graphic {

    ScrollableWidget::ScrollableWidget(const std::string& name,
                                       core::SdlWidget* parent,
                                       const utils::Sizef& area):
      core::SdlWidget(name, area, parent),

      m_supportName(),
      m_coordsToFollow(nullptr)
    {
      // We don't want the widget to be sensitive to hover over events.
      setFocusPolicy(core::FocusPolicy());
    }

    ScrollableWidget::~ScrollableWidget() {}

    void
    ScrollableWidget::setSupport(core::SdlWidget* widget) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Try to retrieve the existing support widget and remove it.
      core::SdlWidget* sup = getChildOrNull<core::SdlWidget>(m_supportName);

      if (sup != nullptr) {
        removeWidget(sup);
      }

      // Now insert the input widget as a support widget for this object.
      if (widget != nullptr) {
        // Insert the input widget as child of this widget so that it gets redrawn.
        widget->setParent(this);

        // Assign the new name of the scroll bar.
        m_supportName = widget->getName();

        // Perform the setup of the support widget
        setupSupport(widget);
      }
    }

    void
    ScrollableWidget::updatePrivate(const utils::Boxf& window) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Transmit the resize event to the support widget if any.
      if (hasSupportWidget()) {
        core::SdlWidget* support = getSupportWidget();

        // We want to actualize the rendering area of the support
        // widget so that it stays the same in the display area.
        // We will also consider that the initial position of the
        // support widget is on the top-left corner of the widget.
        utils::Sizef hint = support->getSizeHint();

        // Compute the position of the rendering area to assign
        // so that the top left corner of the widget coincide
        // with the top left corner of this area.
        utils::Boxf area(
          -window.w() / 2.0f + hint.w() / 2.0f,
          window.h() / 2.0f - hint.h() / 2.0f,
          hint
        );

        // Post the resize event for the support widget.
        postEvent(
          std::make_shared<core::engine::ResizeEvent>(
            area,
            support->getRenderingArea(),
            support
          )
        );
      }

      // Call the parent method to benefit from base class behavior.
      core::SdlWidget::updatePrivate(window);
    }

    bool
    ScrollableWidget::handleContentScrolling(const utils::Vector2f& posToFix,
                                             const utils::Vector2f& whereTo)
    {
      // TODO: Implementation.
      log("Should scroll content so that " + posToFix.toString() + " is displayed at " + whereTo.toString(), utils::Level::Notice);
      return false;
    }

    bool
    ScrollableWidget::mouseButtonPressEvent(const core::engine::MouseEvent& e) {
      // In case the button corresponds to the button used for scrolling
      // we want to assign new coordinates to follow based on the local
      // position of the mouse.
      utils::Vector2f local = mapFromGlobal(e.getMousePosition());

      if (e.getButton() != getScrollingButton()) {
        return core::SdlWidget::mouseButtonPressEvent(e);
      }

      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Assign the new coordinates.
      createOrGetCoordsToFollow(local, true);

      return core::SdlWidget::mouseButtonPressEvent(e);
    }

    bool
    ScrollableWidget::mouseDragEvent(const core::engine::MouseEvent& e) {
      // We only want to event which started inside this widget. Indeed
      // the point of the drag event is to bring the point that was pointed
      // at by the mouse at the moment of the click to the current position
      // of the mouse. This allows for intuitive navigation inside a large
      // document.
      // Also we only want to react to specific buttons which are able to
      // trigger the scrolling operation.
      if (!e.getButtons().isSet(getScrollingButton())) {
        return core::SdlWidget::mouseDragEvent(e);
      }

      // Retrieve the coordinate to follow as described in the input event.
      utils::Vector2f dragStart = mapFromGlobal(e.getInitMousePosition(getScrollingButton()));
      utils::Boxf area = LayoutItem::getRenderingArea().toOrigin();

      if (!area.contains(dragStart)) {
        // The drag event did not originated from our widget, do not start
        // a scrolling operation.
        return core::SdlWidget::mouseDragEvent(e);
      }

      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      utils::Vector2f start = createOrGetCoordsToFollow(dragStart);
      utils::Vector2f localEnd = mapFromGlobal(e.getMousePosition());

      // Call the dedicated handler to do the necessary work in order to
      // handle scrolling: if the return value indicates that some changes
      // where made to this widget we should issue a repaint.
      if (handleContentScrolling(start, localEnd)) {
        requestRepaint();
      }

      // Use the base handler to provide the return value.
      return core::SdlWidget::mouseDragEvent(e);
    }

    bool
    ScrollableWidget::mouseWheelEvent(const core::engine::MouseEvent& e) {
      // We want to trigger some page step actions when the wheel is rolled
      // on the scroll bar. We only want to do so if the mouse is inside
      // this widget though as otherwise it means that we currently react to
      // the wheel event on an application wide basis.
      if (!isMouseInside()) {
        return core::SdlWidget::mouseWheelEvent(e);
      }

      // TODO: Implementation, we want to zoom in/out of the widget.
      log("Should perform zooming in/out", utils::Level::Warning);

      return core::SdlWidget::mouseWheelEvent(e);
    }

  }
}
