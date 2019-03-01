#ifndef    LINEARLAYOUT_HXX
# define   LINEARLAYOUT_HXX

# include <sdl_core/SdlException.hh>
# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    const LinearLayout::Direction&
    LinearLayout::getDirection() const noexcept {
      return m_direction;
    }

    inline
    const float&
    LinearLayout::getMargin() const noexcept {
      return m_margin;
    }

    inline
    const float&
    LinearLayout::getComponentMargin() const noexcept {
      return m_componentMargin;
    }

    inline
    sdl::utils::Sizef
    LinearLayout::computeAvailableSize(const sdl::utils::Boxf& totalArea) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return sdl::utils::Sizef(
          totalArea.w() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin,
          totalArea.h() - 2.0f * m_margin
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return sdl::utils::Sizef(
          totalArea.w() - 2.0f * m_margin,
          totalArea.h() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin
        );
      }
      throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
    }

    inline
    sdl::utils::Sizef
    LinearLayout::computeDefaultWidgetBox(const sdl::utils::Sizef& area) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return sdl::utils::Sizef(
          area.w() / m_items.size(),
          area.h()
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return sdl::utils::Sizef(
          area.w(),
          area.h() / m_items.size()
        );
      }
      throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
    }

    inline
    float
    LinearLayout::computeIncompressibleSize(std::unordered_set<unsigned>& hintedWidgets,
                                            const std::vector<sdl::core::SizePolicy>& widgetsPolicies,
                                            const std::vector<sdl::utils::Sizef>& widgetsHints) const
    {
      float incompressibleSize = 0.0f;

      for (unsigned index = 0u ; index < m_items.size() ; ++index) {
        float size = 0.0f;
        sdl::core::SizePolicy::Policy policy;

        if (getDirection() == Direction::Horizontal) {
          size = widgetsHints[index].w();
          policy = widgetsPolicies[index].getHorizontalPolicy();
        }
        else if (getDirection() == Direction::Vertical) {
          size = widgetsHints[index].h();
          policy = widgetsPolicies[index].getVerticalPolicy();
        }
        else {
          throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
        }

        if (policy == sdl::core::SizePolicy::Fixed && widgetsHints[index].isValid()) {
          // This widget already has a valid size hint and its policy is set to
          // fixed: we have no margin whatsoever in resizing it so we'd better
          // ignore the space it occupies for the rest of the computations.
          incompressibleSize += size;
          hintedWidgets.insert(index);
        }
      }

      return incompressibleSize;
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
