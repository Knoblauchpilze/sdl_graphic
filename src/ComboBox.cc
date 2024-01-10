
# include "ComboBox.hh"
# include "GridLayout.hh"
# include "LabelWidget.hh"
# include "PictureWidget.hh"
# include <core_utils/CoreException.hh>

namespace sdl {
  namespace graphic {

    const int ComboBox::sk_defaultZOrder(1);

    ComboBox::ComboBox(const std::string& name,
                       const InsertPolicy& policy,
                       core::SdlWidget* parent,
                       const utils::Sizef& area,
                       int maxVisibleItems):
      core::SdlWidget(name, area, parent, core::engine::Color::NamedColor::White),

      m_insertPolicy(policy),
      m_maxVisibleItems(maxVisibleItems),

      m_state(State::Closed),
      m_closedBox(),

      m_activeItem(-1),
      m_items()
    {
      // Assign the z order for this widget: it should be drawn in front of other
      // regular widgets.
      setZOrder(getDefaultZOrder());

      // Build the layout for this component.
      build();
    }

    ComboBox::~ComboBox() {}

    void
    ComboBox::insertItem(int index,
                         const std::string& text,
                         const std::string& icon)
    {
      // Create a new item from the input data.
      ComboBoxItem item = ComboBoxItem{text, icon};

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
      // can only be the case if the combobox is dropped, in which case the
      // new item will be displayed. Otherwise nothing changes in the combobox
      // so we can skip the update part.
      if (isDropped()) {
        requestRepaint();
      }
    }

    void
    ComboBox::removeItem(int index) {
      // Remove the corresponding item if it exists.
      if (index < 0 || index >= getItemsCount()) {
        error(
          std::string("Cannot remove item ") + std::to_string(index) + " from combobox",
          std::string("No such item")
        );
      }

      // Perform the deletion.
      m_items.erase(m_items.cbegin() + index);

      bool removedActive = false;

      // Check whether this was the active item.
      if (m_activeItem == index) {
        // We need to make the next item active if it is still possible.
        // The only way it can fail is if the active item was the last one
        // on the list.
        if (m_activeItem >= getItemsCount()) {
          m_activeItem = getItemsCount() - 1;
        }

        removedActive = true;
      }

      // We need to update the content if needed. This can happen either is the
      // deleted item was the active one or if the combobox is dropped: in this
      // case as all items are displayed the removal of the item will be directly
      // visible.
      if (isDropped() || removedActive) {
        requestRepaint();
      }
    }

    bool
    ComboBox::focusInEvent(const core::engine::FocusEvent& e) {
      // We want to set the state of this combo box to dropped
      // if the focus event has a click as source. If this is
      // not the case we will use the base handler to perform
      // the job.
      if (e.getReason() != core::engine::FocusEvent::Reason::MouseFocus) {
        return core::SdlWidget::focusInEvent(e);
      }

      // The reason of the focus gain is a mouse click. We now
      // by design of the events system that the focus is valid
      // and we just have to react to it by dropping the list
      // of options if the combobox is still closed.
      if (isClosed()) {
        // Create a drop event.
        setState(State::Dropped);
      }

      // Use the base handler to provide the return value.
      return core::SdlWidget::focusInEvent(e);
    }

    bool
    ComboBox::focusOutEvent(const core::engine::FocusEvent& e) {
      // The widget just lost focus: this means that we should close
      // the list of options. Indeed there's two possible scenarii:
      // either the focus have been triggered because one of the
      // option of the combobox has been activated or the user clicked
      // elsewhere.
      // In both cases we need to close the combobox if it is opened.

      // Of course this assumes the fact that the focus reason should
      // be mouse click.
      if (e.getReason() != core::engine::FocusEvent::Reason::MouseFocus) {
        return core::SdlWidget::focusOutEvent(e);
      }

      // Close the combobox if needed.
      if (isDropped()) {
        setState(State::Closed);
      }

      // Use the base handler to provide the return value.
      return core::SdlWidget::focusOutEvent(e);
    }

    bool
    ComboBox::gainFocusEvent(const core::engine::FocusEvent& e) {
      // This method is called whenever a child widget has gained the
      // focus. The corresponding event is given in argument and we
      // can retrieve the focus reason from there.
      // From a combobox standpoint, only the case of a `MouseFocus`
      // is really worth it because it indicates that one child widget
      // has been clicked.
      //
      // A click can either trigger the combobox to display the options
      // available or close the list. In order to choose between both
      // options we refer to the internal state and the emitter.
      //
      // So first case: the combobox is closed. We do want to open it
      // if:
      // 1. the combobox is closed.
      // 2. the focus reason is a click.
      // 3. the source of the event is either the main combobox text
      //    or icon.
      // If all these conditions are verified we should set the state
      // of the combobox to `Dropped` so that we display all the
      // available options.
      //
      // Now to the second case: the combobox is opened. We want to
      // close it if:
      // 1. the combobox is dropped.
      // 2. the focus reason is a click.
      // 3. the source of the event is a child different from the main
      //    icon or text.
      // If all these conditions are verified we should set the state
      // of the combobox to `Closed` so that we only display the
      // selected option.

      // We can see that the focus reason needs to be clicked for
      // anything to happen in here so let's handle that first.
      if (e.getReason() == core::engine::FocusEvent::Reason::MouseFocus) {
        // Retrieve the main icon and text widgets: both scenarii use
        // them.
        PictureWidget* icon = getChildAs<PictureWidget>(std::string("combobox_icon"));
        LabelWidget* text = getChildAs<LabelWidget>(std::string("combobox_text"));

        // Check whether the combobox is closed: we will try to open the
        // combobox.
        if (isClosed()) {
          // If the emitter is either the icon or the text we can set the
          // state to `Dropped`.
          if (e.isEmittedBy(icon) || e.isEmittedBy(text)) {
            // Set the state of this combobox to dropped.
            setState(State::Dropped);

            // Also, deactivate the corresponding child so that we don't
            // keep it selected.
            core::engine::FocusEventShPtr foe = core::engine::FocusEvent::createFocusOutEvent(e.getReason(), false, e.getEmitter());
            postEvent(foe, false, true);
          }
        }

        // Check whether the combobox is opened: we will try to close the
        // combobox.
        if (isDropped()) {
          // If the emitter is a child different from the main icon or text
          // but still one of the widget, we have a valid click so let's
          // close the box.
          core::SdlWidget* child = getChildOrNull<core::SdlWidget>(e.getEmitter()->getName());

          if (child != icon && child != text && child != nullptr) {
            // Set the state of this combobox to closed.
            setState(State::Closed);
          }
        }
      }

      // Use the base handler to provide the return value.
      return core::SdlWidget::gainFocusEvent(e);
    }

    bool
    ComboBox::resizeEvent(core::engine::ResizeEvent& e) {
      // Here we need to handle the resize of the widget. We should also try
      // to update the `m_closedBox` attribute so that we can handle correctly
      // the drop/close events.
      // Basically we know that the `setState` method will trigger a resize
      // event. In this case we do not want to react to it because it would
      // override the *real* box size. Consider this:
      //
      // 1. The user clicks on the combobox.
      // 2. It triggers a resize event, using the internal `m_closedBox` to
      //    compute the dropped size.
      // 3. While handling the resize event we update the `m_closedBox`.
      // 4. When the user exits the dropped combobox we cannot use the internal
      //    value of the `m_closedBox` because it has been updated when
      //    the first resize event was handled.
      //
      // We can prevent this situation by checking that the event was not
      // sent by this object. This has the double advantage of allowing to
      // still update the size of the widget when it is dropped and not to
      // override the `m_closedBox` with values generated internally.

      // Check whether this event has been generated by this object.
      if (isEmitter(e)) {
        // Handle resize but do not update the internal `m_closedBox` value
        // as the event has probably been generated from the `setState`
        // function.
        return SdlWidget::resizeEvent(e);
      }

      // We know that the event has not been generated by this object. So
      // it should have originated elsewhere and the size information is
      // relevant and should be transmitted to both the internal `m_closedBox`
      // and to the area of the widget.
      // In any case the size should correspond to the size of the closed
      // combobox so we can safely assign it to the internal value.
      m_closedBox = e.getNewSize();

      // Now we should handle the actual resize event. There's nothing
      // different here from the base `SdlWidget` method but we need to
      // determine which size to give:
      // 1. If the combobox is closed, we can continue right away with the
      //    size provided by the event.
      // 2. If the combobox is dropped, we need to compute the size of the
      //    combobox in dropped mode using the dedicated method.

      // Assume the size corresponds to the input one (i.e. the combobox
      // is closed). If the combobox is dropped we need to update it.
      if (m_state == State::Dropped) {
        e.setNewSize(getDroppedSize());
      }

      // Use the base method to provide the return value.
      return SdlWidget::resizeEvent(e);
    }

    bool
    ComboBox::filterMouseEvents(const core::engine::EngineObject* watched,
                                const core::engine::MouseEventShPtr e) const noexcept
    {
      // Perform the filtering using the base class: if it already filters the
      // event we won't do anything more.
      const bool filtered = core::SdlWidget::filterMouseEvents(watched, e);
      if (filtered) {
        return filtered;
      }

      // The event is not filtered. Let's check whether the `watched` object
      // is either the main icon or text and the associated state of `this`
      // combobox.
      if (isClosed()) {
        // The combobox is closed, we don't filter anything more than the
        // base method in this case.
        return filtered;
      }

      // Retrieve both the main icon and text.
      PictureWidget* icon = getChildAs<PictureWidget>(std::string("combobox_icon"));
      LabelWidget* text = getChildAs<LabelWidget>(std::string("combobox_text"));

      // If the `watched` object is either of those, filter the event: we know
      // that the event is already a mouse event so it's acceptable to filter
      // out all these events.
      if (watched == icon || watched == text) {
        return true;
      }

      // The object for which the events should be filtered does not match
      // neither the main icon nor the text: return the base class value.
      return filtered;
    }

    void
    ComboBox::build() {
      // Assign a linear layout which will allow positionning items and icons.
      GridLayoutShPtr layout = std::make_shared<GridLayout>(
        "combobox_layout",
        this,
        2u,
        1u + static_cast<unsigned>(m_maxVisibleItems)
      );

      // And assign the layout to this widget.
      setLayout(layout);

      // Create two children: a picture widget and a label widget which will be used
      // to represent the items of this combobox.
      PictureWidget* icon = new PictureWidget(
        std::string("combobox_icon"),
        std::string(),
        PictureWidget::Mode::Fit,
        this,
        core::engine::Color::NamedColor::Silver
      );

      LabelWidget* text = new LabelWidget(
        std::string("combobox_text"),
        std::string(),
        std::string("data/fonts/times.ttf"),
        15,
        LabelWidget::HorizontalAlignment::Left,
        LabelWidget::VerticalAlignment::Center,
        this,
        core::engine::Color::NamedColor::Silver
      );

      // Add these items to the layout.
      layout->addItem(icon, 0, 0, 1, 1);
      layout->addItem(text, 1, 0, 1, 1);
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
    ComboBox::setActiveItem(int index) {
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
      getChildAs<PictureWidget>(std::string("combobox_icon"))->setImagePath(m_items[m_activeItem].icon);
      getChildAs<LabelWidget>(std::string("combobox_text"))->setText(m_items[m_activeItem].text);
    }

    void
    ComboBox::setState(const State& state) {
      // First check that the state is actually different from the current state
      if (m_state == state) {
        // Nothing to do.
        return;
      }

      // We need to assign a new size to the combobox based on the desired state.
      // The current closed size is always saved in the `m_closedBox` and in order
      // to obtain the opened size we multiply it by the number of item visible
      // in the combobox.

      // Assume the size is for the closed size.
      utils::Boxf newSize = m_closedBox;
      if (state == State::Dropped) {
        newSize = getDroppedSize();
      }

      debug("Old size is " + m_closedBox.toString() + ", new is " + newSize.toString());

      // Update the internal state.
      m_state = state;

      // Now proceed to posting a resize event with the new size.
      postEvent(std::make_shared<core::engine::ResizeEvent>(
        newSize,
        LayoutItem::getRenderingArea()
      ));

      // Retrieve the layout so that we can insert new widgets if needed.
      GridLayout* layout = getLayoutAs<GridLayout>();

      // We also need to either create the needed icon and text widgets
      // or make them visible/hidden if they already exist. This only
      // applies if the state to apply is `Dropped`. In the case of a close
      // event we only need to make all the widgets non visible.
      const int count = getVisibleItemsCount();
      const bool visible = (m_state == State::Dropped);

      for (int id = 0 ; id < count ; ++id) {
        // Try to set the icon and the visible status for the picture widget.
        PictureWidget* icon = getChildOrNull<PictureWidget>(getIconNameFromID(id));

        if (icon == nullptr) {
          // Create the item.
          icon = new PictureWidget(
            getIconNameFromID(id),
            std::string(),
            PictureWidget::Mode::Fit,
            this,
            core::engine::Color::NamedColor::Silver
          );

          // Add it to the layout.
          layout->addItem(icon, 0u, 1u + id, 1u, 1u);

          // Register the click on the icon widget so that we can update the
          // selected element in this combobox.
          icon->onClick.connect_member<ComboBox>(this, &ComboBox::onElementClicked);
        }

        // Now we either created the icon widget if needed or found it if it
        // already existed. We can assign properties to the picture widget.
        icon->setImagePath(m_items[id].icon);
        icon->setVisible(visible);

        // Try to set the text and the visible status for the label widget.
        LabelWidget* text = getChildOrNull<LabelWidget>(getTextNameFromID(id));

        if (text == nullptr) {
          // Create the item.
          text = new LabelWidget(
            getTextNameFromID(id),
            std::string(),
            std::string("data/fonts/times.ttf"),
            15,
            LabelWidget::HorizontalAlignment::Left,
            LabelWidget::VerticalAlignment::Center,
            this,
            core::engine::Color::NamedColor::Silver
          );

          // Add it to the layout.
          layout->addItem(text, 1u, 1u + id, 1u, 1u);

          // Register the click on the icon widget so that we can update the
          // selected element in this combobox.
          text->onClick.connect_member<ComboBox>(this, &ComboBox::onElementClicked);
        }

        // Now we either created the label widget if needed or found it if it
        // already existed. We can assign properties to the label widget.
        text->setText(m_items[id].text);
        text->setVisible(visible);
      }
    }

    utils::Boxf
    ComboBox::getDroppedSize() const noexcept {
      // We basically scale the closed size by the number of items to
      // display. We use either the minimum number of items available
      // or the count of items.
      // We also need to account for the base item which is always
      // displayed in the combobox.
      // We at least want to keep one element even though no elements
      // are registered in the combobox.
      const int scaling = 1 + getVisibleItemsCount();

      // Compute the delta.
      const float delta = (scaling - 1) * m_closedBox.h();

      return utils::Boxf(
        m_closedBox.x(),
        m_closedBox.y() - delta / 2.0f,
        m_closedBox.w(),
        m_closedBox.h() * scaling
      );
    }

    void
    ComboBox::onElementClicked(const std::string& name) {
      // Retrieve the index of the element based on the name of the widget
      // which has been clicked.
      const int id = getIDFromWidgetName(name);

      debug("Clicked on element " + name + ", id: " + std::to_string(id));

      // Set this item as active: note that if the user clicks on a widget
      // which is already active nothing will happen.
      setActiveItem(id);

      // We do not need to close the combobox here (i.e. set its state to
      // `Closed`): indeed as the child widget has been clicked it will
      // get the focus and thus we will receive a gain focus event. The
      // close option will be handled there.
    }

  }
}