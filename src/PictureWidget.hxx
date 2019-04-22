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
      setPictureChanged();
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
      clearPicture();

      // Load the image.
      if (!m_file.empty()) {
        m_picture = getEngine().createTextureFromFile(m_file, core::engine::Palette::ColorRole::Base);
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
