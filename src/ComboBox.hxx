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
    ComboBox::getDefaultZOrder() noexcept {
      return sk_defaultZOrder;
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
    ComboBox::getIconNameFromID(int id) const noexcept {
      return std::string("icon_widget_") + std::to_string(id);
    }

    inline
    std::string
    ComboBox::getTextNameFromID(int id) const noexcept {
      return std::string("text_widget_") + std::to_string(id);
    }

    inline
    int
    ComboBox::getIDFromWidgetName(const std::string& name) const {
      // First determine whether we have a name corresponding to the generated
      // convention. The name should match something like `type_widget_ID` and
      // thus contain exactly two '_' characters.
      std::size_t count = std::count(name.cbegin(), name.cend(), '_');

      if (count != 2) {
        error(
          std::string("Cannot retrieve id from name \"") + name + "\"",
          std::string("Name does not match internal naming convention")
        );
      }

      // Keep only the last part of the name (i.e. the part of the string after
      // the second occurrence of the '_' character): this should correspond to
      // the identifier.
      const std::size_t pos = name.find_last_of('_');
      std::string idAsStr = name.substr(pos + 1);

      // Try to convert this string into a valid identifier.
      int val = 0;
      try {
        val = std::stoi(idAsStr);
      }
      catch (const std::invalid_argument& e) {
        error(
          std::string("Could not determine id from name \"") + name + "\"",
          std::string("Identifier \"") + idAsStr + "\" could not be converted to int (err: " + e.what() + ")"
        );
      }

      // Check whether this item is valid (i.e. is within the acceptable id
      // range).
      if (val < 0 || val >= getItemsCount()) {
        error(
          std::string("Could not determine id from name \"") + name + "\"",
          std::string("Identifier ") + std::to_string(val) + " is not in acceptable range [0; " + std::to_string(getItemsCount()) + "]"
        );
      }

      // The value is valid as no exception was raised.
      return val;
    }

  }
}

#endif    /* COMBO_BOX_HXX */
