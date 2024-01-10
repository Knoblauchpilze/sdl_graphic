#ifndef    SCROLLABLE_WIDGET_HXX
# define   SCROLLABLE_WIDGET_HXX

# include "ScrollableWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    utils::Sizef
    ScrollableWidget::getPreferredSize() const noexcept {
      // Protect from concurrent accesses.
      const std::lock_guard guard(m_propsLocker);

      // Use the dedicated handler.
      return getPreferredSizePrivate();
    }

    inline
    const core::SdlWidget*
    ScrollableWidget::getItemAt(const utils::Vector2f& pos) const noexcept {
      // Use the base handler to retrieve the item spanning the position.
      const core::SdlWidget* wid = core::SdlWidget::getItemAt(pos);

      // Protect from concurrent accesses.
      const std::lock_guard guard(m_propsLocker);

      // Check whether this widget has a support: if this is not the case
      // whatever we found is considered valid.
      if (!hasSupportWidget()) {
        return wid;
      }

      const utils::Vector2f local = mapFromGlobal(pos);

      // We have a valid support widget assigned to us. For all intent and
      // purposes the returned value from `getItemAt` returned an element
      // from our hierarchy: in any case we want to reduce the position of
      // the widgets which are considered valid for interaction. This is
      // important to make sure that we can safely assign a larger size to
      // the support widget without it reacting to elements outside of the
      // area defined for `this` widget.
      // Thus we want to filter the event in case the input position is not
      // inside the area assigned to this element.
      utils::Boxf b = LayoutItem::getRenderingArea().toOrigin();

      if (!b.contains(local)) {
        // The area is not contained in this widget, consider that we have
        // no valid item at this point.
        return nullptr;
      }

      // Instead of the support widget we will assume that the best fit is
      // `this` widget: this will redirect part of the events that would
      // have landed to the support directly to this widget. We will handle
      // them first and propagate them if needed to the support widget.
      // Typically we need that so that the drag events are sent to this
      // element rather than the child.
      return this;
    }

    inline
    utils::Sizef
    ScrollableWidget::getPreferredSizePrivate() const noexcept {
      // Assume the locker is already locked.

      // If no support widget is assigned, return an empty size.
      if (!hasSupportWidget()) {
        return utils::Sizef();
      }

      // Return the size hint of the support widget.
      return getSupportWidget()->getSizeHint();
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
    float
    ScrollableWidget::getPercentageThreshold() noexcept {
      return 0.001f;
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
      const std::lock_guard guard(m_propsLocker);

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

      // Return the input coordinates.
      return coords;
    }

  }
}

#endif    /* SCROLLABLE_WIDGET_HXX */
