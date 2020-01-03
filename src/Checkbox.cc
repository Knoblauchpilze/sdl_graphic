
# include "Checkbox.hh"
# include "LinearLayout.hh"
# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    Checkbox::Checkbox(const std::string& name,
                       const std::string& text,
                       const std::string& font,
                       bool checked,
                       unsigned size,
                       core::SdlWidget* parent,
                       const utils::Sizef& area,
                       const core::engine::Color& color):
      core::SdlWidget(name, area, parent, color),

      m_propsLocker(),

      m_boxChanged(true),
      m_toggled(checked),

      m_emptyBox(),
      m_selectionItem()
    {
      build(TextData{text, font, size});
    }

    void
    Checkbox::drawContentPrivate(const utils::Uuid& uuid,
                                 const utils::Boxf& area)
    {
      // Acquire the lock on the attributes of this widget.
      Guard guard(m_propsLocker);

      // Load the selection box if needed.
      if (boxChanged()) {
        loadBox();

        m_boxChanged = false;
      }

      // Repaint the selection box to its specified place. We need to fetch
      // the correct selection box based on the current status of the checkbox.
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);
      utils::Sizef sbSize = getEngine().queryTexture(m_emptyBox);
      utils::Sizef siSize = getEngine().queryTexture(m_selectionItem);

      // Compute the position of the selection box based on the result from the
      // layout and handle the case where it does not intersect the input area.
      utils::Boxf whereToSB = m_boxItem->getRenderingArea();
      utils::Boxf whereToSI(whereToSB.x(), whereToSB.y(), siSize);

      utils::Boxf intersectSBWhere = whereToSB.intersect(area);
      utils::Boxf intersectSIWhere = whereToSI.intersect(area);
      utils::Boxf fromSB = convertToLocal(intersectSBWhere, whereToSB);
      utils::Boxf fromSI = convertToLocal(intersectSIWhere, whereToSI);
      utils::Boxf sSBEngine = convertToEngineFormat(fromSB, sbSize);
      utils::Boxf sSIEngine = convertToEngineFormat(fromSI, siSize);
      utils::Boxf dSBEngine = convertToEngineFormat(intersectSBWhere, sizeEnv);
      utils::Boxf dSIEngine = convertToEngineFormat(intersectSIWhere, sizeEnv);

      if (sSBEngine.valid() && dSBEngine.valid()) {
        getEngine().drawTexture(m_emptyBox, &sSBEngine, &uuid, &dSBEngine);
      }
      if (m_toggled && sSIEngine.valid() && dSIEngine.valid()) {
        getEngine().drawTexture(m_selectionItem, &sSIEngine, &uuid, &dSIEngine);
      }
    }

    void
    Checkbox::build(const TextData& text) {
      // Assign a linear layout which will allow positionning the selection box
      // and the display text.
      sdl::graphic::LinearLayoutShPtr layout = std::make_shared<sdl::graphic::LinearLayout>(
        "checkbox_layout",
        this,
        LinearLayout::Direction::Horizontal,
        getBorderDims(),
        2.0f
      );

      // And assign the layout to this widget.
      setLayout(layout);

      // Create the virtual item holding the selection box and the label
      // for this checkbox.
      m_boxItem = std::make_shared<VirtualLayoutItem>(std::string("vitem_for_box"));
      if (m_boxItem == nullptr) {
        error(
          std::string("Could not create checkbox \"") + getName() + "\"",
          std::string("Could not allocate selection box")
        );
      }

      // Both width and size are managed, and we have a max size.
      m_boxItem->setManageWidth(true);
      m_boxItem->setManageHeight(true);
      m_boxItem->setMaxSize(getSelectionBoxMaxDims());

      LabelWidget* label = nullptr;
      if (!text.title.empty()) {
        label = new LabelWidget(
          std::string("checkbox_label"),
          text.title,
          text.font,
          text.size,
          LabelWidget::HorizontalAlignment::Left,
          LabelWidget::VerticalAlignment::Center,
          this
        );

        if (label == nullptr) {
          error(
            std::string("Could not create checkbox \"") + getName() + "\"",
            std::string("Could not allocate text \"") + text.title + "\""
          );
        }

        label->setFocusPolicy(core::FocusPolicy());
        label->setPalette(getPalette());
      }

      // Disable log for various component.
      layout->allowLog(false);
      if (label != nullptr) {
        label->allowLog(false);
      }

      // Add each item to the layout if needed.
      layout->addItem(m_boxItem.get());
      if (label != nullptr) {
        layout->addItem(label);
      }
    }

  }
}
