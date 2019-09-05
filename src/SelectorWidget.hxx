#ifndef    SELECTOR_WIDGET_HXX
# define   SELECTOR_WIDGET_HXX

# include "SelectorWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    SelectorWidget::setActiveWidget(const std::string& name) {
      getLayout().setActiveItem(name);
    }

    inline
    void
    SelectorWidget::setActiveWidget(const int& index) {
      getLayout().setActiveItem(index);
    }

    inline
    void
    SelectorWidget::switchToNext() {
      getLayout().switchToNext();
    }

    inline
    bool
    SelectorWidget::gainFocusEvent(const core::engine::FocusEvent& e) {
      // We need a special processing here in order to provide the `switchToNext`
      // child behavior which may be active for selector widgets. This behavior
      // allows to switch to the next registered child whenever the active one is
      // clicked upon.
      // We figured it's nice to put this behavior here because we're inside the
      // focus handling process anyway so it's easy to hijack some of the existing
      // behavior.
      // Basically what we want to do is to check whether the focus event was done
      // by a click of the mouse: if this is the case and the event comes directly
      // from one of the children of this widget we can activate the switch to next
      // behavior if internal conditions are met. Such conditions include checking
      // that at least two children are registered and things like that.
      // TODO: We should probably handle some kind of mechanism allowing to determine
      // whether the focus event is a primary event (meaning that the source is directly
      // the item that has received focus) or a secondary event (meaning that we are
      // only receiving the event because we're in the hierarchy of the item that
      // received focus). This would help for example only switching when the child
      // itself has been focused and not another item deeper in the hierarchy (which
      // might be weird).

      // First thing to verify is that the `switchOnClick` behavior is activated.
      // It can be deactivated for various reasons, either because it is really
      // not switched on for this widget or because there's not enough children,
      // or because this widget gained focus (and not its children), etc.
      // This first pass tries to determine if we actually needs to switch anything.
      bool dontSwitch =
        !switchOnClick() ||
        getChildrenCount() < 2 ||
        isEmitter(e) ||
        e.getReason() != core::engine::FocusEvent::Reason::MouseFocus ||
        e.isSpontaneous() ||
        !hasChild(e.getEmitter()->getName())
      ;

      // Check whether we need to switch anything.
      if (dontSwitch) {
        return core::SdlWidget::gainFocusEvent(e);
      }

      // Retrieve the child which has just been activated. We now for sure that we
      // can actually retrieve the emitter of the event as a child of `this` widget
      // based on the condition to use the base handler.
      core::SdlWidget* child = getChildAs<core::SdlWidget>(e.getEmitter()->getName());

      // Post a focus out event for this widget.
      log("Posting focus out event for " + child->getName() + " in order to switch active child");
      core::engine::FocusEventShPtr lfe = core::engine::FocusEvent::createFocusOutEvent(e.getReason(), false, child);
      postEvent(lfe, false, true);

      // Trigger the call to switch to the next child.
      switchToNext();

      // Note that we actually voluntarily choose to stop the normal process here and
      // not to call the base method defined over in `SdlWidget`: this actually stops
      // the propagation of the focus event and will not result in previously focused
      // widgets losing it: in the case of a user clicking on a sibling widget to this
      // `SelectorWidget` and then on a child of `this` widget, the sibling will not
      // lose its focus. This is okay as we actually don't consider that the child
      // gained focus: we just used it to perform the switch.
      // So at this point we just want to return whether the event was handled: it is
      // the case as we knew how to deal with it.
      return true;
    }

    inline
    bool
    SelectorWidget::switchOnClick() const noexcept {
      return m_switchOnLeftClick;
    }

    inline
    SelectorLayout&
    SelectorWidget::getLayout() {
      // Try to retrieve the layout as a `SelectorLayout`.
      SelectorLayout* layout = getLayoutAs<SelectorLayout>();

      // If the conversion failed, this is a problem.
      if (layout == nullptr) {
        error(
          std::string("Cannot retrieve layout for selector widget"),
          std::string("Invalid layout type")
        );
      }

      // Return this layout.
      return *layout;
    }

  }
}

#endif    /* SELECTOR_WIDGET_HXX */
