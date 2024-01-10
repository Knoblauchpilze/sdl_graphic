#ifndef    LABELWIDGET_HXX
# define   LABELWIDGET_HXX

# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    LabelWidget::setText(const std::string& text) noexcept {
      const std::lock_guard guard(m_propsLocker);
      m_text = text;
      setTextChanged();
    }

    inline
    void
    LabelWidget::setHorizontalAlignment(const HorizontalAlignment& alignment) noexcept {
      const std::lock_guard guard(m_propsLocker);
      m_hAlignment = alignment;
    }

    inline
    void
    LabelWidget::setVerticalAlignment(const VerticalAlignment& alignment) noexcept {
      const std::lock_guard guard(m_propsLocker);
      m_vAlignment = alignment;
    }

    inline
    void
    LabelWidget::loadText() {
      // Clear existing label if any.
      clearText();

      // Load the text.
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

        m_label = getEngine().createTextureFromText(m_text, m_font, m_textRole);
      }
    }

    inline
    void
    LabelWidget::clearText() {
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

    inline
    void
    LabelWidget::setTextChanged() noexcept {
      // The text is now dirty.
      m_textChanged = true;

      // Request a repaint.
      requestRepaint();
    }

    inline
    void
    LabelWidget::updateTextRole(const utils::Uuid& base) {
      // Check whether the input `base` texture is valid.
      if (!base.valid()) {
        warn("Cannot update text role using invalid base texture identifier");
        return;
      }

      // Retrieve the current role for the base texture.
      const core::engine::Palette::ColorRole baseRole = getEngine().getTextureRole(base);

      // Determine the text's role from the base role.
      switch (baseRole) {
        case core::engine::Palette::ColorRole::Highlight:
        case core::engine::Palette::ColorRole::Dark:
          m_textRole = core::engine::Palette::ColorRole::HighlightedText;
          break;
        default:
        case core::engine::Palette::ColorRole::Background:
          m_textRole = core::engine::Palette::ColorRole::WindowText;
          break;
      }
    }

  }
}

#endif    /* LABELWIDGET_HXX */
