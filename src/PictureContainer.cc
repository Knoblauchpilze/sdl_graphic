
# include "PictureContainer.hh"

namespace sdl {
  namespace graphic {

    PictureContainer::PictureContainer(const std::string& name,
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

    PictureContainer::~PictureContainer() {
      if (m_picture != nullptr) {
        SDL_DestroyTexture(m_picture);
      }
    }

    SDL_Texture*
    PictureContainer::createContentPrivate(SDL_Renderer* renderer) const {
      // Load the picture.
      loadPicture(renderer);

      // Create the surface based on the dimensions of this container.
      SDL_Texture* croppedArea = sdl::core::SdlWidget::createContentPrivate(renderer);
      if (croppedArea == nullptr) {
        throw sdl::core::SdlException(std::string("Unable to create perform rendering for picture container \"") + getName() + "\" using file " + m_file);
      }

      // Compute the blit position of the picture so that it is centered.
      if (m_picture != nullptr) {

        SDL_Texture* initialRenderingArea = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, croppedArea);

        // Perform the copy operation according to the display mode.
        if (m_mode == Mode::Crop) {
          int wt, ht;
          SDL_QueryTexture(croppedArea, nullptr, nullptr, &wt, &ht);

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
      }

      // This is the final image for this container.
      return croppedArea;
    }

  }
}