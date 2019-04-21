#ifndef    LABELWIDGET_HXX
# define   LABELWIDGET_HXX

# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    LabelWidget::setText(const std::string& text) noexcept {
      m_text = text;
      m_textChanged = true;
      // TODO: Should probably not trigger a complete recreation of the widget but rather only
      // a clear and `loadText` operation.
      makeContentDirty();
    }

    inline
    void
    LabelWidget::setHorizontalAlignment(const HorizontalAlignment& alignment) noexcept {
      m_hAlignment = alignment;
      makeContentDirty();
    }

    inline
    void
    LabelWidget::setVerticalAlignment(const VerticalAlignment& alignment) noexcept {
      m_vAlignment = alignment;
      makeContentDirty();
    }

    inline
    void
    LabelWidget::loadText() const {
      // Clear existing label if any.
      clearText();

      // Load the text
      if (!m_text.empty()) {
        if (!m_font.valid()) {
          // Load the font.
          m_font = getEngine().createColoredFont(m_fontName, getPalette(), m_fontSize);

          if (!m_font.valid()) {
            error(
              std::string("Cannot create text \"") + m_text + "\"",
              std::string("Invalid null font")
            );
          }
        }

        // TODO: Should probably be related to the current state of the widget instead of hard coding the role.
        m_label = getEngine().createTextureFromText(m_text, m_font, core::engine::Palette::ColorRole::WindowText);
      }
    }

    inline
    void
    LabelWidget::clearText() const {
      if (m_label.valid()) {
        getEngine().destroyTexture(m_label);
        m_label.invalidate();
      }
    }

    inline
    bool
    LabelWidget::textChanged() const noexcept {
      return m_textChanged;
    }

  }
}

#endif    /* LABELWIDGET_HXX */
