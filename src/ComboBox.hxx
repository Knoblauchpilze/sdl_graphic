#ifndef    COMBO_BOX_HXX
# define   COMBO_BOX_HXX

# include "ComboBox.hh"

namespace sdl {
  namespace graphic {

    inline
    std::string
    ComboBox::getNameFromPolicy(const InsertPolicy& policy) noexcept {
      switch (policy) {
        case InsertPolicy::NoInsert:
          return std::string("\"no_insert\"");
        case InsertPolicy::InsertAtTop:
          return std::string("\"at_top\"");
        case InsertPolicy::InsertAtCurrent:
          return std::string("\"at_current\"");
        case InsertPolicy::InsertAtBottom:
          return std::string("\"at_bottom\"");
        case InsertPolicy::InsertAfterCurrent:
          return std::string("\"after_current\"");
        case InsertPolicy::InsertBeforeCurrent:
          return std::string("\"before_current\"");
        case InsertPolicy::InsertAlphabetically:
          return std::string("\"alphabetically\"");
        default:
          return std::string("\"unknown_policy\"");
      }
    }

    inline
    int
    ComboBox::getItemsCount() const noexcept {
      return m_items.size();
    }

    inline
    bool
    ComboBox::hasActiveItem() const noexcept {
      return m_activeItem != -1;
    }

    int
    ComboBox::getActiveItem() const noexcept {
      return m_activeItem;
    }

    inline
    void
    ComboBox::insertItem(const std::string& text,
                         const std::string& icon)
    {
      // Determine the index to which this item should be inserted from the
      // internal insertion policy.
      const std::pair<int, bool> index = getIndexFromInsertPolicy(text);

      // Check whether we should care about the removal of the existing item.
      if (index.first >= 0 && index.first < getItemsCount() && index.second) {
        // We need to reomve an existing item.
        removeItem(index.first);
      }

      // Perform the insertion using the dedicated handler.
      insertItem(index.first, text, icon);
    }

    inline
    bool
    ComboBox::isDropped() const noexcept {
      return m_state == State::Dropped;
    }

    inline
    bool
    ComboBox::isClosed() const noexcept {
      return m_state == State::Closed;
    }

    inline
    int
    ComboBox::getVisibleItemsCount() const noexcept {
      return std::max(1, std::min(getItemsCount(), m_maxVisibleItems));
    }

    inline
    std::string
    ComboBox::getIconNameFromID(const int& id) const noexcept {
      return std::string("icon_widget_") + std::to_string(id);
    }

    inline
    std::string
    ComboBox::getTextNameFromID(const int& id) const noexcept {
      return std::string("text_widget_") + std::to_string(id);
    }

  }
}

#endif    /* COMBO_BOX_HXX */
