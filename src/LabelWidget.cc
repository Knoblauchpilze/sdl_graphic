
# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    LabelWidget::LabelWidget(const std::string& name,
                             const std::string& text,
                             ColoredFontShPtr font,
                             const HorizontalAlignment& hAlignment,
                             const VerticalAlignment& vAlignment,
                             SdlWidget* parent,
                             const SDL_Color& backgroundColor):
      sdl::core::SdlWidget(name,
                           sdl::core::Boxf(),
                           parent,
                           backgroundColor),
      m_text(text),
      m_font(font),
      m_hAlignment(hAlignment),
      m_vAlignment(vAlignment),
      m_textDirty(true),
      m_label(nullptr)
    {}

    LabelWidget::~LabelWidget() {
      if (m_label != nullptr) {
        SDL_DestroyTexture(m_label);
      }
    }

    void
    LabelWidget::drawContentPrivate(SDL_Renderer* renderer, SDL_Texture* texture) const noexcept {
      // Load the text if needed.
      if (m_textDirty) {
        createText(renderer);
        m_textDirty = false;
      }

      // Compute the blit position of the picture so that it is centered.
      if (m_label != nullptr) {

        SDL_Texture* initialRenderingArea = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, texture);

        // Perform the copy operation according to the alignment.
        SDL_Rect dstRect;
        int wl, hl;
        SDL_QueryTexture(m_label, nullptr, nullptr, &wl, &hl);
        int wt, ht;
        SDL_QueryTexture(texture, nullptr, nullptr, &wt, &ht);

        Uint16 x, y;

        switch (m_hAlignment) {
          case HorizontalAlignment::Left:
            x = 0;
            break;
          case HorizontalAlignment::Right:
            x = wt - wl;
            break;
          case HorizontalAlignment::Center:
          default:
            x = static_cast<Uint16>(wt / 2.0f - wl / 2.0f);
            break;
        }

        switch (m_vAlignment) {
          case VerticalAlignment::Top:
            y = 0;
            break;
          case VerticalAlignment::Bottom:
            y = ht - hl;
            break;
          case VerticalAlignment::Center:
          default:
            y = static_cast<Uint16>(ht / 2.0f - hl / 2.0f);
            break;
        }

        dstRect = {
          static_cast<Uint16>(x),
          static_cast<Uint16>(y),
          static_cast<Uint16>(wl),
          static_cast<Uint16>(hl)
        };

        SDL_RenderCopy(renderer, m_label, nullptr, &dstRect);

        SDL_SetRenderTarget(renderer, initialRenderingArea);
      }
    }

  }
}
