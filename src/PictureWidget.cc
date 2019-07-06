
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
        log("Drawing picture while updating region " + area.toString() + " (converted: " + converted.toString() + ")");

        // TODO: We should care about updating only part of the texture: indeed
        // imagine that the root widget is a picture widget: if a child get updated
        // the folliwing call will draw the entire texture behind the widget which
        // is not what we want. We want to copy only the part of the picture which
        // would effectively be blitted at this position.
        getEngine().drawTexture(m_picture, nullptr, &uuid, &converted);
      }

      // Handle `Crop` mode.
      if (m_mode == Mode::Crop) {
        // In crop mode, the final area corresponds to a position where the image
        // will be centered, and the dimensions are consistent with the internal
        // image dimensions. Due to how we use the position of textures, the final
        // rect is always set to `[0; 0]` because the picture is centered in the
        // parent widget.
        // The convert method will set it to the middle of the widget as expected
        // by the engine.
        //
        // In order to crop the picture, we need to select the area of the picture
        // which corresponds to its center. This center will be displayed on all
        // the available space.
        // We can note that either the picture is smaller than the available space
        // in which case the image will be centered, or the picture is larger than
        // the available space, in which case the center of the image will be
        // displayed.
        // Two steps are required to determine which part of the image should be
        // drawn in the input `area`:
        // 1. The part of the image which should be displayed needs to be computed.
        // 2. The actual blit part needs to be computed both from the part of the
        //    image displayed and the area to update.

        // First, proceed to the determination of the part of the image which is
        // drawn: in a first approach, the whole image can be displayed.
        utils::Sizei sizePic = getEngine().queryTexture(m_picture);
        utils::Boxf srcRect = utils::Boxf::fromSize(sizePic, true);

        // Now we actually need to restrain the area to the available space.
        utils::Sizei sizeEnv = getEngine().queryTexture(uuid);
        utils::Boxf dstRect = utils::Boxf::fromSize(sizeEnv, true);

        srcRect = srcRect.intersect(dstRect);
        utils::Boxf srcRectEngine = utils::Boxf(sizePic.w() / 2.0f, sizePic.h() / 2.0f, srcRect.w(), srcRect.h());

        // Now determine the blit coordinates by converting the `dstRect`.
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRect);

        // Draw the texture with specified rects.
        getEngine().drawTexture(m_picture, &srcRectEngine, &uuid, &dstRectEngine);
      }
    }

  }
}
