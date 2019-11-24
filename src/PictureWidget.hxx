#ifndef    PICTUREWIDGET_HXX
# define   PICTUREWIDGET_HXX

# include "PictureWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    PictureWidget::setImagePath(const std::string& path) {
      Guard guard(m_propsLocker);

      // Create a new image from the input path if needed.
      if (path.empty()) {
        m_img.reset();
      }
      else {
        m_img = std::make_shared<core::engine::Image>(path);
      }

      setPictureChanged();
    }

    inline
    void
    PictureWidget::setMode(const Mode& mode) {
      Guard guard(m_propsLocker);
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
      clearPicture();

      // Load the image.
      if (m_img != nullptr) {
        m_picture = getEngine().createTextureFromFile(m_img, core::engine::Palette::ColorRole::Base);
      }
    }

    inline
    void
    PictureWidget::clearPicture() const {
      if (m_picture.valid()) {
        getEngine().destroyTexture(m_picture);
        m_picture.invalidate();
      }
    }

    inline
    bool
    PictureWidget::pictureChanged() const noexcept {
      return m_picChanged;
    }

    inline
    void
    PictureWidget::setPictureChanged() const noexcept {
      m_picChanged = true;
    }

  }
}

#endif    /* PICTUREWIDGET_HXX */
