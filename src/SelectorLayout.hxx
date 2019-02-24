#ifndef    SELECTORLAYOUT_HXX
# define   SELECTORLAYOUT_HXX

# include "SelectorLayout.hh"
# include <sdl_core/SdlException.hh>

namespace sdl {
  namespace graphic {

    inline
    const float&
    SelectorLayout::getMargin() const noexcept {
      return m_margin;
    }

    inline
    void
    SelectorLayout::setActiveItem(const std::string& name) {
      int indexItem = 0;
      bool found = false;
      while (indexItem < m_items.size() && !found) {
        if (m_items[indexItem] != nullptr && m_items[indexItem]->getName() == name) {
          found = true;
        }
        else {
          ++indexItem;
        }
      }

      if (!found) {
        throw sdl::core::SdlException(std::string("Cannot activate item ") + name + " in selector layout, item not found");
      }

      setActiveItem(indexItem);
    }

    inline
    void
    SelectorLayout::setActiveItem(const int& index) {
      if (index >= m_items.size()) {
        throw sdl::core::SdlException(
          std::string("Cannot activate child ") + std::to_string(index) +
          " in selector layout only containing " + std::to_string(m_items.size()) + " child(ren)"
        );
      }

      if (index == m_activeItem) {
        return;
      }

      m_activeItem = index;
      invalidate();
    }

    inline
    std::string
    SelectorLayout::getActiveItem() const {
      if (m_activeItem < 0) {
        throw sdl::core::SdlException(std::string("Cannot retrieve name of active child for selector layout, no such element"));
      }
      if (m_items[m_activeItem] == nullptr) {
        throw sdl::core::SdlException(std::string("Cannot retrieve name of active child for selector layout, invalid null element"));
      }

      return m_items[m_activeItem]->getName();
    }

    inline
    int
    SelectorLayout::getActiveItemId() const {
      if (m_activeItem < 0) {
        throw sdl::core::SdlException(std::string("Cannot retrieve name of active child for selector layout, no such element"));
      }
      return m_activeItem;
    }

  }
}

#endif    /* SELECTORLAYOUT_HXX */
