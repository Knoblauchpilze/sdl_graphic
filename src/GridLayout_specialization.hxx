#ifndef    GRIDLAYOUT_SPECIALIZATION_HXX
# define   GRIDLAYOUT_SPECIALIZATION_HXX

# include "GridLayout.hh"
# include <functional>

namespace std {

  template<>
  struct hash<sdl::graphic::GridLayout::WidgetData> {
    inline
    std::size_t operator()(const sdl::graphic::GridLayout::WidgetData& resource) const noexcept {
      std::size_t val = hash<unsigned>()(resource.id);
      return val;
    }
  };

}

namespace sdl {
  namespace graphic {

    bool
    operator==(const sdl::graphic::GridLayout::WidgetData& lhs, const sdl::graphic::GridLayout::WidgetData& rhs) noexcept
    {
      return lhs.id == rhs.id;
    }

  }
}

#endif    /* GRIDLAYOUT_SPECIALIZATION_HXX */
