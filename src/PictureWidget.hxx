#ifndef    PICTUREWIDGET_HXX
# define   PICTUREWIDGET_HXX

# include "PictureWidget.hh"

# include "GraphicException.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    PictureWidget::setImagePath(const std::string& path) {
      std::lock_guard<std::mutex> guard(getLocker());
      m_file = path;
      m_pictureDirty = true;
    }

    inline
    void
    PictureWidget::setMode(const Mode& mode) {
      std::lock_guard<std::mutex> guard(getLocker());
      m_mode = mode;
    }

    inline
    void
    PictureWidget::clear() {
      setImagePath(std::string(""));
    }

    inline
    void
    PictureWidget::loadPicture(SDL_Renderer* renderer) const {
      // Clear existing image if any.
      if (m_picture != nullptr) {
        SDL_DestroyTexture(m_picture);
        m_picture = nullptr;
      }

      // Load the image.
      if (!m_file.empty()) {
        SDL_Surface* imageAsSurface = SDL_LoadBMP(m_file.c_str());
        if (imageAsSurface == nullptr) {
          throw GraphicException(
            std::string("Unable to create picture widget using file \"") + m_file + "\"",
            getName()
          );
        }

        m_picture = SDL_CreateTextureFromSurface(renderer, imageAsSurface);
        SDL_FreeSurface(imageAsSurface);
        if (m_picture == nullptr) {
          throw GraphicException(
            std::string("Unable to create picture widget using file \"") + m_file + "\"",
            getName()
          );
        }
      }
    }

  }
}

#endif    /* PICTUREWIDGET_HXX */
