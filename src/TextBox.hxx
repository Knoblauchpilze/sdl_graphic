#ifndef    TEXT_BOX_HXX
# define   TEXT_BOX_HXX

# include "TextBox.hh"

namespace sdl {
  namespace graphic {

    inline
    bool
    TextBox::updateStateFromFocus(const core::engine::FocusEvent::Reason& reason,
                                  const bool gainedFocus)
    {
      // First apply the base class handler to determine we should update the
      // text's role at all.
      const bool updated = core::SdlWidget::updateStateFromFocus(reason, gainedFocus);

      // If the base class has handled the focus reason, we can display the cursor
      // at the end of the text or hide it if the focus was lost.
      if (updated) {
        updateCursorState(gainedFocus);
      }

      // Return the value provided by the base handler.
      return updated;
    }

    inline
    void
    TextBox::updateCursorState(const bool visible) {
      // TODO: Implement this probably through some addition texture displayed at the
      // end of the text's texture.
      log(std::string("Should make cursor ") + (visible ? "visible" : "hidden"), utils::Level::Warning);
    }

  }
}

#endif    /* TEXT_BOX_HXX */
