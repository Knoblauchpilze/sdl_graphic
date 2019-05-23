#ifndef    GRIDLAYOUT_SPECIALIZATION_HXX
# define   GRIDLAYOUT_SPECIALIZATION_HXX

# include "GridLayout.hh"
# include <functional>

namespace std {

  template<>
  struct hash<sdl::graphic::GridLayout::WidgetDataWrapper> {
    inline
    std::size_t operator()(const sdl::graphic::GridLayout::WidgetDataWrapper& resource) const noexcept {
      std::size_t val = hash<unsigned>()(resource.id);
      return val;
    }
  };

}

namespace sdl {
  namespace graphic {

    inline
    bool
    operator==(const sdl::graphic::GridLayout::WidgetDataWrapper& lhs,
               const sdl::graphic::GridLayout::WidgetDataWrapper& rhs) noexcept
    {
      return lhs.id == rhs.id;
    }

  }
}

#endif    /* GRIDLAYOUT_SPECIALIZATION_HXX */
