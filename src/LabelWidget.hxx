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
    LabelWidget::setFont(core::engine::ColoredFontShPtr font) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_font = font;
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
      if (m_label != nullptr) {
        getEngine()->destroyTexture(*m_label);
        m_label.reset();
      }

      // Load the text
      if (!m_text.empty()) {
        if (m_font == nullptr) {
          error(
            std::string("Cannot create text \"") + m_text + "\"",
            std::string("Invalid null font")
          );
        }

        const core::engine::Texture::UUID tex = getEngine()->createTextureFromText(m_text, m_font);
        m_label = std::make_shared<core::engine::Texture::UUID>(tex);
      }
    }

  }
}

#endif    /* LABELWIDGET_HXX */
