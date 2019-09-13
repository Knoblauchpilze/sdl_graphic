
# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    LabelWidget::LabelWidget(const std::string& name,
                             const std::string& text,
                             const std::string& font,
                             const unsigned& size,
                             const HorizontalAlignment& hAlignment,
                             const VerticalAlignment& vAlignment,
                             SdlWidget* parent,
                             const core::engine::Color& color,
                             const utils::Sizef& area):
      core::SdlWidget(name, area, parent, color),
      m_text(text),

      m_fontName(font),
      m_fontSize(size),
      m_font(),

      m_hAlignment(hAlignment),
      m_vAlignment(vAlignment),

      m_textRole(core::engine::Palette::ColorRole::WindowText),

      m_textChanged(true),

      m_label(),

      m_propsLocker()
    {}

    LabelWidget::~LabelWidget() {
      // Clear text.
      clearText();

      // Clear font.
      if (m_font.valid()) {
        getEngine().destroyColoredFont(m_font);
      }
    }

    void
    LabelWidget::drawContentPrivate(const utils::Uuid& uuid,
                                    const utils::Boxf& area)
    {
      // Lock the content using the locker provided by the parent class.
      Guard guard(m_propsLocker);

      // Load the text: this should happen only if the text has changed since
      // last draw operation. This can either mean that the text itself has
      // been modified or that one of the rendering properties to use to draw
      // the text has been updated.
      if (textChanged()) {
        // Update the text's role.
        updateTextRole(uuid);

        // Load the text.
        loadText();

        // The text has been updated.
        m_textChanged = false;
      }

      // If we don't have any text to display, return early, nothing more to
      // do for the drawing operation.
      if (!m_label.valid()) {
        return;
      }

      // There's a double goal to this function: it must determine where the
      // text should be displayed but also which part of it should be drawn.
      // These tasks are not exactly independent. More precisely to determine
      // which part of the text should be displayed one must first determine
      // where the text will be displayed.
      // Indeed we can find out where the text will be displayed only using
      // its dimension and the internal alignment.
      // Once this is done it gives us a box in local coordinate frame which
      // represents the position at which the whole text will be displayed.
      // This might not be exactly what we want in the end but it is a start.
      //
      // Once we have that we can found out whether this area includes part
      // of the `area` to update. From there, two possible scenari:
      // a. the text does not overlap with the input `area`.
      // b. the text does overlap with the input `area`.
      //
      // If the text does not overlap we can return early: nothing can be
      // done from here and all the job should be done by the other internal
      // 'clearContentPrivate' method.
      //
      // If the text does overlap we need to figure out which part is visible
      // in the input `area`. This can be computed as both the area where the
      // text should be blit and the input `area` are expressed in the local
      // coordinate frame.

      // Determine the position where the text should be blit, not considering
      // the input `area` nor the available space.
      utils::Sizef sizeText = getEngine().queryTexture(m_label);
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);

      utils::Vector2f center;

      switch (m_hAlignment) {
        case HorizontalAlignment::Left:
          center.x() = -sizeEnv.w() / 2.0f + sizeText.w() / 2.0f;
          break;
        case HorizontalAlignment::Right:
          center.x() = sizeEnv.w() / 2.0f - sizeText.w() / 2.0f;
          break;
        case HorizontalAlignment::Center:
        default:
          center.x() = 0.0f;
          break;
      }

      switch (m_vAlignment) {
        case VerticalAlignment::Top:
          center.y() = sizeEnv.w() / 2.0f - sizeText.h() / 2.0f;
          break;
        case VerticalAlignment::Bottom:
          center.y() = -sizeEnv.h() / 2.0f + sizeText.h() / 2.0f;
          break;
        case VerticalAlignment::Center:
        default:
          center.y() = 0.0f;
          break;
      }

      utils::Boxf dstRect(center, sizeText);

      // Compute the intersection between the input `area` and this `dstRect`
      // area. If both overlaps it means that part of the text is visible.
      utils::Boxf dstRectToUpdate = dstRect.intersect(area);

      if (!dstRectToUpdate.valid()) {
        return;
      }

      // Some part of the text is visible. We now need to determine which part.
      // In order to do so we need to convert the `dstRectToUpdate` into the
      // coordinate frame of the text to display. This can be done by expressing
      // this area relatively to the `dstRect` itself as this area has the same
      // scale as the text itself.
      utils::Boxf srcRect = convertToLocal(dstRectToUpdate, dstRect);

      // Convert both area to areas usable by the engine.
      utils::Boxf env = utils::Boxf::fromSize(sizeEnv, true);

      utils::Boxf srcRectEngine = convertToEngineFormat(srcRect, utils::Boxf::fromSize(sizeText, true));
      utils::Boxf dstRectEngine = convertToEngineFormat(dstRectToUpdate, env);

      // Repaint the needed part of the text.
      getEngine().drawTexture(m_label, &srcRectEngine, &uuid, &dstRectEngine);
    }

    void
    LabelWidget::stateUpdatedFromFocus(const core::FocusState& state,
                                       const bool gainedFocus)
    {
      // First apply the base class handler so that the base texture's role is set
      // to a value consistent with the current state.
      core::SdlWidget::stateUpdatedFromFocus(state, gainedFocus);

      // Follow up by updating the text's role. As we don't have an idea of the
      // actual role of the texture we will just mark the text as dirty and wait
      // for the next `drawContentPrivate` operation to update the text's role.
      // This can only occur if this widget is the source of the focus change.
      Guard guard(m_propsLocker);
      setTextChanged();
    }

  }
}
