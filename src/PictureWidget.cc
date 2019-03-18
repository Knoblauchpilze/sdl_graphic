
# include "PictureWidget.hh"

namespace sdl {
  namespace graphic {

    PictureWidget::PictureWidget(const std::string& name,
                                       const std::string& picture,
                                       const Mode& mode,
                                       SdlWidget* parent,
                                       const bool transparent,
                                       const core::engine::Palette& palette,
                                       const utils::Sizef& area):
      sdl::core::SdlWidget(name,
                           area,
                           parent,
                           transparent,
                           palette),
      m_file(picture),
      m_mode(mode),
      m_picture(nullptr),
      m_pictureDirty(true)
    {}

    PictureWidget::~PictureWidget() {
      if (m_picture != nullptr) {
        core::engine::EngineLocator::getEngine().destroyTexture(*m_picture);
      }
    }

    void
    PictureWidget::drawContentPrivate(const core::engine::Texture::UUID& uuid) const noexcept {
      // Load the picture.
      if (m_pictureDirty) {
        loadPicture();
        m_pictureDirty = false;
      }

      // Compute the blit position of the picture so that it is centered.
      if (m_picture != nullptr) {
        // Perform the copy operation according to the display mode.
        if (m_mode == Mode::Crop) {
          utils::Sizei sizePic = core::engine::EngineLocator::getEngine().queryTexture(*m_picture);
          utils::Sizei sizeEnv = core::engine::EngineLocator::getEngine().queryTexture(uuid);

          utils::Boxf dstRect(
            (sizeEnv.w() - sizePic.w()) / 2.0f,
            (sizeEnv.h() - sizePic.h()) / 2.0f,
            sizePic.w(),
            sizePic.h()
          );

          core::engine::EngineLocator::getEngine().drawTexture(
            *m_picture,
            &uuid,
            &dstRect
          );
        }

        if (m_mode == Mode::Fit) {
          core::engine::EngineLocator::getEngine().drawTexture(
            *m_picture,
            &uuid,
            nullptr
          );
        }
      }
    }

  }
}
