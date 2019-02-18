#ifndef    LABELWIDGET_HXX
# define   LABELWIDGET_HXX

# include <sdl_core/SdlException.hh>
# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    LabelWidget::setText(const std::string& text) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_text = text;
      m_textDirty = true;
      makeDirty();
    }

    inline
    void
    LabelWidget::setFont(core::ColoredFontShPtr font) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_font = font;
      m_textDirty = true;
      makeDirty();
    }

    inline
    void
    LabelWidget::setHorizontalAlignment(const HorizontalAlignment& alignment) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_hAlignment = alignment;
      m_textDirty = true;
      makeDirty();
    }

    inline
    void
    LabelWidget::setVerticalAlignment(const VerticalAlignment& alignment) noexcept {
      std::lock_guard<std::mutex> guard(getLocker());
      m_vAlignment = alignment;
      m_textDirty = true;
      makeDirty();
    }

    inline
    void
    LabelWidget::loadText(SDL_Renderer* renderer) const {
      // Clear existing label if any.
      if (m_label != nullptr) {
        SDL_DestroyTexture(m_label);
        m_label = nullptr;
      }

      // Load the text
      if (!m_text.empty()) {
        if (m_font == nullptr) {
          throw core::FontException(std::string("Cannot create text \"") + m_text + "\" for \"" + getName() + "\", invalid null font");
        }

        m_label = m_font->render(renderer, m_text);
        if (m_label == nullptr) {
          throw sdl::core::SdlException(std::string("Unable to create label widget \"") + getName() + "\" using text \"" + m_text + "\"");
        }
      }
    }

  }
}

#endif    /* LABELWIDGET_HXX */
