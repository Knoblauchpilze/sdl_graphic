
# include "PictureWidget.hh"

namespace sdl {
  namespace graphic {

    PictureWidget::PictureWidget(const std::string& name,
                                       const std::string& picture,
                                       const Mode& mode,
                                       SdlWidget* parent,
                                       const core::engine::Color& color,
                                       const utils::Sizef& area):
      core::SdlWidget(name, area, parent, color),
      m_file(picture),
      m_mode(mode),
      m_picture(),
      m_picChanged(true)
    {}

    PictureWidget::~PictureWidget() {
      clearPicture();
    }

    void
    PictureWidget::drawContentPrivate(const utils::Uuid& uuid,
                                      const utils::Boxf& area) const
    {
      // Load the picture: this should happen only if the picture has changed
      // since last draw operation.
      if (pictureChanged()) {
        loadPicture();
        m_picChanged = false;
      }

      // If we don't have any picture to display, return early, nothing more
      // to do.
      if (!m_picture.valid()) {
        return;
      }

      // Handle the drawing of the picture based on the fitting mode. A fit
      // operation is easier because we just blit the picture so that it takes
      // up all the space available.
      // The `Crop` mode specifies that only the part of the picture which can
      // be displayed in the provided space should be displayed.
      // We also need to handle the `area` of the picture which should be drawn
      // so that we only handle the intersection of the area provided in input
      // and the area spanned by the picture.

      // Handle `Fit` mode.
      if (m_mode == Mode::Fit) {
        // The picture should be drawn at the position specified by the area.
        // We need to convert the input area to a format which can be used by
        // the engine.
        utils::Boxf converted = convertToEngineFormat(area);
        log("Drawing picture while updating region " + area.toString() + " (converted: " + converted.toString());

        // TODO: We should care about updating only part of the texture: indeed imagine that the
        // root widget is a picture widget: if a child get updated the folliwing call will draw
        // the entire texture behind the widget which is not what we want. We want to copy only
        // the part of the picture which would effectively be blitted at this position.
        getEngine().drawTexture(m_picture, &uuid, &converted);
      }

      // Handle `Crop` mode.
      if (m_mode == Mode::Crop) {
        utils::Sizei sizePic = getEngine().queryTexture(m_picture);
        utils::Sizei sizeEnv = getEngine().queryTexture(uuid);

        utils::Boxf dstRect(
          (sizeEnv.w() - sizePic.w()) / 2.0f,
          (sizeEnv.h() - sizePic.h()) / 2.0f,
          sizePic.w(),
          sizePic.h()
        );

        // TODO: Handle `area`.
        getEngine().drawTexture(m_picture, &uuid, &dstRect);
      }

      // Compute the blit position of the picture so that it is centered.
      if (m_picture.valid()) {
        // Perform the copy operation according to the display mode.
        if (m_mode == Mode::Crop) {
        }

        if (m_mode == Mode::Fit) {
        }
      }
    }

  }
}
