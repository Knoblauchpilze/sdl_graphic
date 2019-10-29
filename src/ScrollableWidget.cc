
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
          -window.h() / 2.0f + hint.h() / 2.0f,
          hint
        );

        log(
          "Scrollabe size is " + window.toString() + ", size hint is " + hint.toString() +
          " area is " + area.toString()
        );

        // Post the resize event for the support widget.
        // TODO: Make sure that the resize event does not trigger a
        // repaint if the width and height does not change.
        postEvent(
          std::make_shared<core::engine::ResizeEvent>(
            area,
            support->getRenderingArea(),
            support
          )
        );
      }
    }

  }
}
