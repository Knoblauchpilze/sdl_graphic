#ifndef    PICTURECONTAINER_HXX
# define   PICTURECONTAINER_HXX

# include <sdl_core/SdlException.hh>
# include "PictureContainer.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    PictureContainer::setImagePath(const std::string& path) {
      std::lock_guard<std::mutex> guard(getLocker());
      m_file = path;
      makeDirty();
    }

    inline
    void
    PictureContainer::setMode(const Mode& mode) {
      std::lock_guard<std::mutex> guard(getLocker());
      m_mode = mode;
      makeDirty();
    }

    inline
    void
    PictureContainer::clear() {
      setImagePath(std::string(""));
    }

    inline
    void
    PictureContainer::loadPicture(SDL_Renderer* renderer) const {
      // Clear existing image if any.
      if (m_picture != nullptr) {
        SDL_DestroyTexture(m_picture);
        m_picture = nullptr;
      }

      // Load the image
      if (!m_file.empty()) {
        SDL_Surface* imageAsSurface = SDL_LoadBMP(m_file.c_str());
        if (imageAsSurface == nullptr) {
          throw sdl::core::SdlException(std::string("Unable to create picture container \"") + getName() + "\" using file " + m_file);
        }

        m_picture = SDL_CreateTextureFromSurface(renderer, imageAsSurface);
        SDL_FreeSurface(imageAsSurface);

        if (m_picture == nullptr) {
          throw sdl::core::SdlException(std::string("Unable to create picture container \"") + getName() + "\" using file " + m_file);
        }
      }
    }

  }
}

#endif // PICTURECONTAINER_HXX
