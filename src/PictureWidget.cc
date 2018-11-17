
# include "PictureWidget.hh"

namespace sdl {
  namespace graphic {

    PictureWidget::PictureWidget(const std::string& name,
                                       const std::string& picture,
                                       const Mode& mode,
                                       SdlWidget* parent,
                                       const SDL_Color& backgroundColor):
      sdl::core::SdlWidget(name,
                           sdl::core::Boxf(),
                           parent,
                           backgroundColor),
      m_file(picture),
      m_mode(mode),
      m_picture(nullptr)
    {}

    PictureWidget::~PictureWidget() {
      if (m_picture != nullptr) {
        SDL_DestroyTexture(m_picture);
      }
    }

    void
    PictureWidget::drawContentPrivate(SDL_Renderer* renderer, SDL_Texture* texture) const noexcept {
      // Load the picture.
      loadPicture(renderer);

      // Compute the blit position of the picture so that it is centered.
      if (m_picture != nullptr) {

        SDL_Texture* initialRenderingArea = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, texture);

        // SDL_BlendMode initialMode;
        // SDL_GetRenderDrawBlendMode(renderer, &initialMode);
        // SDL_SetRenderDrawBlendMode(renderer, m_blendMode);
        SDL_SetTextureAlphaMod(m_picture, m_background.a);

        // Perform the copy operation according to the display mode.
        if (m_mode == Mode::Crop) {
          int wt, ht;
          SDL_QueryTexture(texture, nullptr, nullptr, &wt, &ht);

          int wp, hp;
          SDL_QueryTexture(m_picture, nullptr, nullptr, &wp, &hp);


          SDL_Rect dstRect = {
            static_cast<Sint16>(wt / 2.0f - wp / 2.0f),
            static_cast<Sint16>(ht / 2.0f - hp / 2.0f),
            static_cast<Uint16>(wp),
            static_cast<Uint16>(hp)
          };

          SDL_RenderCopy(renderer, m_picture, nullptr, &dstRect);
        }

        if (m_mode == Mode::Fit) {
          SDL_RenderCopy(renderer, m_picture, nullptr, nullptr);
        }

        SDL_SetRenderTarget(renderer, initialRenderingArea);
        // SDL_SetRenderDrawBlendMode(renderer, initialMode);
      }
    }

  }
}
