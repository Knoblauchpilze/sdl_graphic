#ifndef    PICTUREWIDGET_HXX
# define   PICTUREWIDGET_HXX

# include "PictureWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    PictureWidget::setImagePath(const std::string& path) {
      m_file = path;
      m_picChanged = true;
      // TODO: At some point we might want to create an intermediate event preventing from
      // recreating the whole content even if the only the image has been modified.
      // It could maybe go with reimplementing the `repaintEvent` method to distinguish between
      // cases.
      makeContentDirty();
    }

    inline
    void
    PictureWidget::setMode(const Mode& mode) {
      m_mode = mode;
      makeContentDirty();
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

  }
}

#endif    /* PICTUREWIDGET_HXX */
