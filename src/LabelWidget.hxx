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
      setTextChanged();
    }

    inline
    void
    LabelWidget::setHorizontalAlignment(const HorizontalAlignment& alignment) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_hAlignment = alignment;
    }

    inline
    void
    LabelWidget::setVerticalAlignment(const VerticalAlignment& alignment) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_vAlignment = alignment;
    }

    inline
    bool
    LabelWidget::enterEvent(const core::engine::EnterEvent& e) {
      // Update the role of the text texture.
      log("Updating role of text to highlighted " + getPalette().getColorForRole(core::engine::Palette::ColorRole::HighlightedText).toString());
      m_textRole = core::engine::Palette::ColorRole::HighlightedText;

      // Mark the text as dirty.
      setTextChanged();

      // Apply the base handler and use it to determine the return value.
      return core::SdlWidget::enterEvent(e);
    }

    inline
    bool
    LabelWidget::leaveEvent(const core::engine::Event& e) {
      // Update the role of the text texture.
      m_textRole = core::engine::Palette::ColorRole::WindowText;

      // Mark the text as dirty.
      setTextChanged();

      // Apply the base handler and use it to determine the return value.
      return core::SdlWidget::leaveEvent(e);
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

        m_label = getEngine().createTextureFromText(m_text, m_font, m_textRole);
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

    inline
    void
    LabelWidget::setTextChanged() const noexcept {
      m_textChanged = true;
    }

  }
}

#endif    /* LABELWIDGET_HXX */
