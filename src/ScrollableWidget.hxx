#ifndef    SCROLLABLE_WIDGET_HXX
# define   SCROLLABLE_WIDGET_HXX

# include "ScrollableWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    utils::Sizef
    ScrollableWidget::getPreferredSize() const noexcept {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // If no support widget is assigned, return an empty size.
      if (!hasSupportWidget()) {
        return utils::Sizef();
      }

      // Return the size hint of the support widget.
      return getSupportWidget()->getSizeHint();
    }

    inline
    const core::SdlWidget*
    ScrollableWidget::getItemAt(const utils::Vector2f& pos) const noexcept {
      // Use the base handler to retrieve the item spanning the position.
      const core::SdlWidget* wid = core::SdlWidget::getItemAt(pos);

      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Check whether the widget corresponds to the support widget.
      if (!hasSupportWidget()) {
        return wid;
      }

      core::SdlWidget* support = getSupportWidget();

      if (wid != support) {
        // The best fit is not the support widget, we can return it.
        return wid;
      }

      // The best fit is the support widget: we need to make sure that the
      // area does correspond to the area specified for this widget. We do
      // check it against the area of `this` widget.
      utils::Boxf b = LayoutItem::getRenderingArea();

      if (!b.contains(pos)) {
        // The area is not contained in this widget, consider that we have
        // no valid item at this point.
        return nullptr;
      }

      // The support widget is valid.
      return wid;
    }

    inline
    bool
    ScrollableWidget::filterDragAndDropEvents(const core::engine::EngineObject* watched,
                                              const core::engine::DropEventShPtr e) const noexcept
    {
      // Protect from concurrent accesses.
      // TODO: We should maybe filter more stuff from the `getItemAt` method.
      // And also probably reimplement the filtering of mouse events to filter
      // the drag events for the support widget.
      Guard guard(m_propsLocker);

      // In case the `watched` element is the support widget we want to filter the
      // drag events. Indeed they should be interpreted as such.
      if (!hasSupportWidget()) {
        return core::SdlWidget::filterDragAndDropEvents(watched, e);
      }

      core::SdlWidget* support = getSupportWidget();
      if (support != watched) {
        return core::SdlWidget::filterDragAndDropEvents(watched, e);
      }

      // The support widget should not receive drop events.
      return false;
    }

    inline
    void
    ScrollableWidget::setupSupport(core::SdlWidget* /*widget*/) {
      // Empty implementation.
    }

    inline
    bool
    ScrollableWidget::dropEvent(const core::engine::DropEvent& e) {
      // Use the dedicated handler to clear the coordinates to
      // follow if needed.
      attemptToClearCoords(e.getButton());

      // Use the base handler to provide a return value.
      return core::SdlWidget::dropEvent(e);
    }

    inline
    bool
    ScrollableWidget::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // Use the dedicated handler to clear the coordinates to
      // follow if needed.
      attemptToClearCoords(e.getButton());

      // Use the base handler to provide a return value.
      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    inline
    core::engine::mouse::Button
    ScrollableWidget::getScrollingButton() noexcept {
      return core::engine::mouse::Button::Left;
    }

    inline
    bool
    ScrollableWidget::hasSupportWidget() const noexcept {
      return hasChild(m_supportName);
    }

    inline
    core::SdlWidget*
    ScrollableWidget::getSupportWidget() const {
      // Use the base handler to retrieve the support widget if any.
      return getChildAs<core::SdlWidget>(m_supportName);
    }

    inline
    void
    ScrollableWidget::attemptToClearCoords(const core::engine::mouse::Button& button) noexcept {
      // In case the button provided as inputdoes not corresponds to the one
      // used for scrolling we won't do anything. Otherwise we need to reset
      // the coordinates to follow.
      if (button != getScrollingButton()) {
        return;
      }

      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Reset the coordinates to follow.
      m_coordsToFollow.reset();
    }

    inline
    utils::Vector2f
    ScrollableWidget::createOrGetCoordsToFollow(const utils::Vector2f& coords,
                                                bool force)
    {
      // Assume the locker is already locked.

      // If some coordinates are already set to be followed, use it. A
      // special case comes when we want to force the update of these
      // coordinates.
      if (m_coordsToFollow != nullptr && !force) {
        return *m_coordsToFollow;
      }

      // Othewise create the new coordinates to follow from the input
      // argument.
      m_coordsToFollow = std::make_shared<utils::Vector2f>(coords);

      log("Scrollable widget should follow " + coords.toString() + " on " + m_supportName, utils::Level::Notice);

      // Return the input coordinates.
      return coords;
    }

  }
}

#endif    /* SCROLLABLE_WIDGET_HXX */
