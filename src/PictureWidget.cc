
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
      m_picChanged(true),

      m_propsLocker()
    {
      // Check whether we can assign a valid size hint to this item if possible.
      // The size hint could either be the result of the input area (in which
      // case it denotes a will of the user to use this precise value and we
      // should honor that or it could be left empty in which case we can try to
      // make better.
      // Typically in the case of a picture mode set to `Fit` we want to assign
      // a size hint equivalent to the initial size of the picture. This allows
      // to at least request that the picture can be displayed in full.
      if (!area.valid()) {
        // TODO: Determine and assign the picture size.
      }
    }

    PictureWidget::~PictureWidget() {
      clearPicture();
    }

    void
    PictureWidget::drawContentPrivate(const utils::Uuid& uuid,
                                      const utils::Boxf& area)
    {
      // Acquire the lock on the attributes of this widget.
      Guard guard(m_propsLocker);

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
      //
      // In any of the available mode an important part of the process is to
      // determine which part of the image should be blit if the area spans the
      // entire area available in the `uuid` texture.
      // This area will be then intersected with the actual `area` to be drawn
      // in order to only repaint the needed bits.
      // So a first preliminary step is to determine these values.

      // In a first approach let's consider that the entire picture can be drawn.
      utils::Sizef sizePic = getEngine().queryTexture(m_picture);
      utils::Boxf srcRect = utils::Boxf::fromSize(sizePic, true);

      // Handle the area where the picture should be drawn. The input `area` is
      // expected to be expressed in local coordinates so we can directly compute
      // the engine usable equivalent. There is only one subtlety: we need to
      // intersect the input `area` with the rendering area of this picture: if
      // we don't do that we risk to blit the texture onto an area larger than
      // the current size of the widget leading to invalid scaling.
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);
      utils::Boxf dstRect = utils::Boxf::fromSize(sizeEnv, true).intersect(area);

      // Check whether the `dstRect` is valid: if this is not the case it means
      // that we're indeed asked to repaint an area that is not inside this
      // widget so we can return early as there's nothing to do.
      if (!dstRect.valid()) {
        return;
      }

      // Handle `Fit` mode.
      if (m_mode == Mode::Fit) {
        // In this mode the picture is automatically resized in order to fit the
        // available space. It is thus much harder to exactly determine which
        // part of the image will be drawn in the input `area`.
        // To do so, we know that the whole `srcRect` gets mapped to a `sizeEnv`
        // area upon drawing. From this we can deduce the initial area which gets
        // mapped to `area` in the final drawing.
        // Once we have this information we can use ti to draw the initial area
        // and map it to the desired `area` part of the final image.

        // Determine the initial location of the input `area` in the initial image.
        const float wScale = sizeEnv.w() / srcRect.w();
        const float hScale = sizeEnv.h() / srcRect.h();

        const utils::Vector2f center = utils::Vector2f(dstRect.x() / wScale, dstRect.y() / hScale);

        utils::Boxf areaAsSrc(center, dstRect.w() / wScale, dstRect.h() / hScale);

        // The output `area` corresponds to the `areaAsSrc` portion of the initial
        // image. We now need to intersect this with the image area and see if
        // this makes sense. The only restriction is if the input `area` is larger
        // than the available `sizeEnv`: this is the only case where the `srcRect`
        // will perform any intersection on the `areaAsSrc`. Otherwise as we're
        // performing a fit operation we can only draw subparts of the picture.
        utils::Boxf srcAreaToDraw = srcRect.intersect(areaAsSrc);

        // Convert this area to engine format.
        utils::Boxf srcAreaToDrawEngine = convertToEngineFormat(srcAreaToDraw, srcRect);

        // The final blit area can be directly converted into engine usable format.
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRect, LayoutItem::getRenderingArea());

        // Repaint the picture.
        getEngine().drawTexture(m_picture, &srcAreaToDrawEngine, &uuid, &dstRectEngine);
      }

      // Handle `Crop` mode.
      if (m_mode == Mode::Crop) {
        // In crop mode, the picture is displayed with no scaling and just takes
        // the maximum amount of space between what's available and its internal
        // size.
        // In the case where the image is smaller than the available area it will
        // be centered and if it is larger only its center will be displayed so
        // that we get a partial representation of it.
        // Depending on these consideration and the position of the `area` to
        // update, it might happen that we actually don't draw anything. If for
        // example the picture is really small and the `area` refers to the top
        // left corner of this widget, the picture does not span this area and
        // thus we won't do anything.
        //
        // Just like the `Fit` mode, we will determine the part of the input image
        // coordinate frame which corresponds to the input `area` and then perform
        // the intersection between both to determine which part of the image needs
        // to be drawn.
        // It is simpler that in the `Fit` mode because there's a 1 to 1 matching
        // between an area in the widget's space and in the picture space.
        // Considering this the input `area` corresponds exactly to the same area
        // in picture space.

        utils::Boxf srcAreaToDraw = srcRect.intersect(dstRect);

        // If this area is not valid, it means that no part of the image are drawn
        // on the input `area` to update. The `clearContentPrivate` should have
        // handled this and we have nothing more to do in here.
        if (!srcAreaToDraw.valid()) {
          return;
        }

        // The intersection between the area to repaint and the picture is not empty:
        // convert it to engine semantic and perform the repaint.
        utils::Boxf srcAreaToDrawEngine = convertToEngineFormat(srcAreaToDraw, srcRect);

        // Now let's compute the final blit area. Most of the time it will corresponds
        // to the input `area` converted so that it can be used by the engine. One notable
        // exception to this is when the input `area` is larger than the picture or spans
        // areas which do not belong to the picture. In this case we need to restrict the
        // destination area to its portion where it intersects the area of the picture.
        utils::Boxf dstRectMatched = dstRect.intersect(srcAreaToDraw);

        // The final blit area can be directly converted into engine usable format.
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRectMatched, LayoutItem::getRenderingArea());

        // Draw the texture with specified rects.
        getEngine().drawTexture(m_picture, &srcAreaToDrawEngine, &uuid, &dstRectEngine);
      }
    }

  }
}
