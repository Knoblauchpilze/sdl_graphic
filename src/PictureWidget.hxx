#ifndef    PICTUREWIDGET_HXX
# define   PICTUREWIDGET_HXX

# include "PictureWidget.hh"

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
    PictureWidget::loadPicture() const {
      // Clear existing image if any.
      if (m_picture != nullptr) {
        getEngine()->destroyTexture(*m_picture);
        m_picture.reset();
      }

      // Load the image.
      if (!m_file.empty()) {
        const core::engine::Texture::UUID tex = getEngine()->createTextureFromFile(m_file);
        m_picture = std::make_shared<core::engine::Texture::UUID>(tex);
      }
    }

  }
}

#endif    /* PICTUREWIDGET_HXX */
