
# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    LabelWidget::LabelWidget(const std::string& name,
                             const std::string& text,
                             const utils::Uuid& font,
                             const HorizontalAlignment& hAlignment,
                             const VerticalAlignment& vAlignment,
                             SdlWidget* parent,
                             const bool transparent,
                             const core::engine::Palette& palette,
                             const utils::Sizef& area):
      sdl::core::SdlWidget(name,
                           area,
                           parent,
                           transparent,
                           palette),
      m_text(text),
      m_font(font),
      m_hAlignment(hAlignment),
      m_vAlignment(vAlignment),
      m_textDirty(true),
      m_label()
    {}

    LabelWidget::~LabelWidget() {
      if (m_label.valid()) {
        getEngine().destroyTexture(m_label);
      }
      if (m_font.valid()) {
        getEngine().destroyColoredFont(m_font);
      }
    }

    void
    LabelWidget::drawContentPrivate(const utils::Uuid& uuid) const noexcept {
      // Load the text.
      if (m_textDirty) {
        loadText();
        m_textDirty = false;
      }

      // Compute the blit position of the text so that it is centered.
      if (m_label.valid()) {
        // Perform the copy operation according to the alignment.
        utils::Sizei sizeText = getEngine().queryTexture(m_label);
        utils::Sizei sizeEnv = getEngine().queryTexture(uuid);

        utils::Boxf dstRect;

        // Dimension of the dst area are known.
        dstRect.w() = sizeText.w();
        dstRect.h() = sizeText.h();

        switch (m_hAlignment) {
          case HorizontalAlignment::Left:
            dstRect.x() = dstRect.w() / 2.0f;
            break;
          case HorizontalAlignment::Right:
            dstRect.x() = sizeEnv.w() - sizeText.w() / 2.0f;
            break;
          case HorizontalAlignment::Center:
          default:
            dstRect.x() = sizeEnv.w() / 2.0f;
            break;
        }

        switch (m_vAlignment) {
          case VerticalAlignment::Top:
            dstRect.y() = dstRect.h() / 2.0f;
            break;
          case VerticalAlignment::Bottom:
            dstRect.y() = sizeEnv.h() - sizeText.h() / 2.0f;
            break;
          case VerticalAlignment::Center:
          default:
            dstRect.y() = sizeEnv.h() / 2.0f;
            break;
        }

        getEngine().drawTexture(
          m_label,
          &uuid,
          &dstRect
        );
      }
    }

  }
}
