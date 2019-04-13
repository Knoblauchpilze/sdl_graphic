#ifndef    LABELWIDGET_HXX
# define   LABELWIDGET_HXX

# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    LabelWidget::setText(const std::string& text) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_text = text;
      m_textDirty = true;
      makeContentDirty();
    }

    inline
    void
    LabelWidget::setHorizontalAlignment(const HorizontalAlignment& alignment) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_hAlignment = alignment;
      m_textDirty = true;
      makeContentDirty();
    }

    inline
    void
    LabelWidget::setVerticalAlignment(const VerticalAlignment& alignment) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_vAlignment = alignment;
      m_textDirty = true;
      makeContentDirty();
    }

    inline
    void
    LabelWidget::loadText() const {
      // Clear existing label if any.
      if (m_label.valid()) {
        getEngine().destroyTexture(m_label);
        m_label.invalidate();
      }

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

        m_label = getEngine().createTextureFromText(m_text, m_font);
      }
    }

  }
}

#endif    /* LABELWIDGET_HXX */
