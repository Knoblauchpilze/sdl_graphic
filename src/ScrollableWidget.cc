
# include "ScrollableWidget.hh"

namespace sdl {
  namespace graphic {

    ScrollableWidget::ScrollableWidget(const std::string& name,
                                       core::SdlWidget* parent,
                                       const utils::Sizef& area):
      core::SdlWidget(name, area, parent),

      m_supportName()
    {}

    ScrollableWidget::~ScrollableWidget() {}

    void
    ScrollableWidget::setSupport(core::SdlWidget* widget) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Try to retrieve the existing support widget and remove it.
      core::SdlWidget* sup = getChildOrNull<core::SdlWidget>(m_supportName);

      if (sup != nullptr) {
        // TODO: Handle removal from layout ?
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

        // TODO: Actual implementation and determination of the area to display.
        // To do so we should add a method like `getPreferredSize` in the base
        // `LayoutItem` class which would return either an invalid size or the
        // size currently assigned to widgets in most cases and a custom value
        // in case of specific inheriting classes: one example coming to mind is
        // the case of the `Fit` picture widgets where we want to retrieve the
        // size of the picture as loaded from disk.
        // This value would be used here instead of the size of this widget and
        // possibly in the `setupSupport` if needed.
        // Once it is done we can determine the area in here and we should work
        // on allowing the control of the displayed area of this widget.
        postEvent(
          std::make_shared<core::engine::ResizeEvent>(
            window.toOrigin(),
            support->getRenderingArea(),
            support
          )
        );
      }
    }

  }
}
