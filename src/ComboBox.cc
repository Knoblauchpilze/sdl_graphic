
# include "ComboBox.hh"
# include "LinearLayout.hh"
# include "LabelWidget.hh"
# include "PictureWidget.hh"

namespace sdl {
  namespace graphic {

    ComboBox::ComboBox(const std::string& name,
                       const InsertPolicy& policy,
                       core::SdlWidget* parent,
                       const utils::Sizef& area,
                       const int maxVisibleItems):
      core::SdlWidget(name, area, parent, core::engine::Color::NamedColor::White),

      m_insertPolicy(policy),
      m_maxVisibleItems(maxVisibleItems),

      m_activeItem(-1),
      m_items()
    {
      // Build the layout for this component.
      build();
    }

    ComboBox::~ComboBox() {}

    void
    ComboBox::insertItem(const int index,
                         const std::string& text,
                         const std::string& icon)
    {
      // Create a new item from the input data.
      ComboBoxItem item = ComboBoxItem{
        text,
        icon
      };

      // Check the position of insertion: we want to correctly handle
      // cases where the index is either negative or larger than the
      // current size of the internal `m_items` vector.
      if (index < 0) {
        m_items.insert(m_items.cbegin(), item);
      }
      else if (index >= getItemsCount()) {
        m_items.push_back(item);
      }
      else {
        m_items.insert(m_items.cbegin() + index, item);
      }

      // We also need to update the active item if any: indeed if the
      // active item was after the item which has just been inserted
      // we need to increase the value of the `m_activeItem` by one
      // to make it for the newly inserted item.
      if (hasActiveItem() && m_activeItem >= index) {
        ++m_activeItem;
      }

      // Check whether we need to modify the display for this combobox. This
      // can be the case if:
      // 1. there is no an item in the combo box.
      // 2. there was only one item in the combo box and it was selected.
      if (getItemsCount() == 1 || (getItemsCount() == 2 && hasActiveItem())) {
        makeContentDirty();
      }
    }

    void
    ComboBox::removeItem(const int index) {
      // Remove the corresponding item if it exists.
      if (index < 0 || index >= getItemsCount()) {
        error(
          std::string("Cannot remove item ") + std::to_string(index) + " from combobox",
          std::string("No such item")
        );
      }

      // Perform the deletion.
      m_items.erase(m_items.cbegin() + index);

      // Check whether this was the active item.
      if (m_activeItem == index) {
        // We need to make the next item active if it is still possible.
        // The only way it can fail is if the active item was the last one
        // on the list.
        if (m_activeItem >= getItemsCount()) {
          m_activeItem = getItemsCount() - 1;
        }
      }

      // We need to update the content.
      makeContentDirty();
    }

    bool
    ComboBox::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // We need to switch the active widget or assign it if none is
      // selected yet. Nothing happens if theere is already a widget
      // active and no more than one is registered in this box.
      // In addition to that we have to verify that the location of
      // the click is inside of this widget.

      // Determine whether the position of the click is inside the widget.
      if (isInsideWidget(e.getMousePosition()) && getItemsCount() > 1) {
        // Update the active widget.
        setActiveItem((getActiveItem() + 1) % getItemsCount());
      }

      // Use base handler to determine whether the event was recognized.
      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    void
    ComboBox::build() {
      // Assign a linear layout which will allow positionning items and icons.
      LinearLayoutShPtr layout = std::make_shared<LinearLayout>(
        "combobox_layout",
        this,
        LinearLayout::Direction::Horizontal,
        0.0f,
        1.0f
      );

      // Create two children: a picture widget and a label widget which will be used
      // to represent the items of this combobox.
      PictureWidget* icon = new PictureWidget(
        "combobox_icon",
        std::string(),
        PictureWidget::Mode::Fit,
        this
      );

      LabelWidget* text = new LabelWidget(
        "combobox_text",
        std::string(),
        std::string("data/fonts/times.ttf"),
        15,
        LabelWidget::HorizontalAlignment::Left,
        LabelWidget::VerticalAlignment::Center,
        this,
        core::engine::Color::NamedColor::Silver
      );

      // Add these items to the layout.
      layout->addItem(icon);
      layout->addItem(text);

      // And assign the layout to this widget.
      setLayout(layout);
    }

    std::pair<int, bool>
    ComboBox::getIndexFromInsertPolicy(const std::string& text) const {
      // Compute the alphabetical position of the input text based on the
      // existing items already registered in the combobox.
      // We could retrieve all the keys, add the input `text` then perform
      // a sort and obtain the id. But we can also just scan all the
      // existing keys and compare each one to the input `text` and work
      // out the position of the text from there.
      int alphabeticalRank = getItemsCount();
      for (unsigned id = 0u ; id < m_items.size() ; ++id) {
        if (m_items[id].text.compare(text) < 0) {
          // The new text should be inserted at least before this item.
          alphabeticalRank = id - 1;
        }
      }

      // We only need to take care of the case where the input `text` should
      // be inserted before the first registered item. We will end up with an
      // `alphabeticalRank` of `-1` which we should transform into `0`. This
      // is not a problem because the `AlphabeticalOrder` does not erase any
      // existing item.
      alphabeticalRank = std::max(alphabeticalRank, 0);


      // Disitnguish according to the insertion policy.
      switch (m_insertPolicy) {
        case InsertPolicy::InsertAtTop:
          return std::make_pair(0, false);
        case InsertPolicy::InsertAtCurrent:
          return std::make_pair(getActiveItem(), true);
        case InsertPolicy::InsertAtBottom:
          return std::make_pair(getItemsCount(), false);
        case InsertPolicy::InsertAfterCurrent:
          return std::make_pair(getActiveItem() + 1, false);
        case InsertPolicy::InsertBeforeCurrent:
          return std::make_pair(getActiveItem(), false);
        case InsertPolicy::InsertAlphabetically:
          return std::make_pair(alphabeticalRank, false);
        case InsertPolicy::NoInsert:
        default:
          break;
      }

      // If we reach this point it means that we were not able to determine
      // a valid insertion index from the policy. Raise an error.
      error(
        std::string("Could not determine insertion index from policy ") + getNameFromPolicy(m_insertPolicy),
        std::string("Invalid policy")
      );

      // Silent compiler even though `error` will throw.
      return std::make_pair(0, false);
    }

    void
    ComboBox::setActiveItem(const int& index) {
      // Remove the corresponding item if it exists.
      if (index < 0 || index >= getItemsCount()) {
        error(
          std::string("Cannot set active item ") + std::to_string(index) + " in combobox",
          std::string("No such item")
        );
      }

      // If the index corresponds to the currently active item we don't have to
      // do anything.
      if (index == m_activeItem) {
        return;
      }

      // Assign the active item.
      m_activeItem = index;

      // We need to activate the corresponding text and icon from the internal
      // table.
      getChildAs<PictureWidget>("combobox_icon")->setImagePath(m_items[m_activeItem].icon);
      getChildAs<LabelWidget>("combobox_text")->setText(m_items[m_activeItem].text);
    }

  }
}