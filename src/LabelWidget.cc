
# include <sdl_core/RendererState.hh>
# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    LabelWidget::LabelWidget(const std::string& name,
                             const std::string& text,
                             core::ColoredFontShPtr font,
                             const HorizontalAlignment& hAlignment,
                             const VerticalAlignment& vAlignment,
                             SdlWidget* parent,
                             const bool transparent,
                             const core::Palette& palette,
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
      m_label(nullptr)
    {}

    LabelWidget::~LabelWidget() {
      if (m_label != nullptr) {
        SDL_DestroyTexture(m_label);
      }
    }

    void
    LabelWidget::onKeyReleasedEvent(const SDL_KeyboardEvent& keyEvent) {
      std::lock_guard<std::mutex> guard(m_drawingLocker);
      for (WidgetMap::const_iterator widget = m_children.cbegin() ;
           widget != m_children.cend() ;
           ++widget)
      {
        widget->second->onKeyReleasedEvent(keyEvent);
      }

      if (keyEvent.keysym.sym == SDLK_KP_7) {
        SDL_Color color = m_font->getColor()();
        if (color.a > 245) {
          color.a = 255;
        }
        else {
          color.a = std::min(255, color.a + 10);
        }
        m_font->setColor(core::Color(color));
        m_textDirty = true;
        std::cout << "[LAB] " << getName() << " alpha: " << std::to_string(color.a) << std::endl;
      }
      if (keyEvent.keysym.sym == SDLK_KP_4) {
        SDL_Color color = m_font->getColor()();
        if (color.a < 10) {
          color.a = 0;
        }
        else {
          color.a = std::max(0, color.a - 10);
        }
        m_font->setColor(core::Color(color));
        m_textDirty = true;
        std::cout << "[LAB] " << getName() << " alpha: " << std::to_string(color.a) << std::endl;
      }
    }

    void
    LabelWidget::drawContentPrivate(SDL_Renderer* renderer, SDL_Texture* texture) const noexcept {
      // Load the text.
      if (m_textDirty) {
        loadText(renderer);
        m_textDirty = false;
      }

      // Compute the blit position of the text so that it is centered.
      if (m_label != nullptr) {
        // Save the current state of the renderer.
        sdl::core::RendererState state(renderer);
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
      }
    }

  }
}
