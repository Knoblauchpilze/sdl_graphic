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
    LinearLayout::computeDefaultWidgetBox(const sdl::utils::Sizef& area,
                                          const unsigned& widgetsCount) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return sdl::utils::Sizef(
          area.w() / widgetsCount,
          area.h()
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return sdl::utils::Sizef(
          area.w(),
          area.h() / widgetsCount
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

    inline
    sdl::utils::Sizef
    LinearLayout::computeWorkingSize(const sdl::utils::Sizef& size,
                                     const float& unavailable) const
    {
      if (getDirection() == Direction::Horizontal) {
        return sdl::utils::Sizef(size.w() - unavailable, size.h());
      }
      else if (getDirection() == Direction::Vertical) {
        return sdl::utils::Sizef(size.w(), size.h() - unavailable);
      }
      else {
        throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
      }
    }

    inline
    sdl::utils::Sizef
    LinearLayout::computeSizeFromPolicy(const sdl::utils::Sizef& desiredSize,
                                        const sdl::core::SizePolicy& policy,
                                        const sdl::utils::Sizef& minSize,
                                        const sdl::utils::Sizef& hint,
                                        const sdl::utils::Sizef& maxSize) const
    {
      // Create the return size and assume the desired size is valid.
      sdl::utils::Sizef outputBox(desiredSize);

      bool widthDone = false;
      bool heightDone = false;

      // Check the policy for fixed size. If the policy is fixed, we should assign
      // the `hint` size whatever the input `desiredSize`. Except of course if the
      // `hint` is not a valid size, in which case we can use the `desiredSize`.
      if (policy.getHorizontalPolicy() == sdl::core::SizePolicy::Fixed) {
        // Two distinct cases:
        // 1) The `hint` is valid, in which case we have to use it.
        // 2) The `hint` is not valid in which case we have to use the `desiredSize`.
        if (hint.isValid()) {
          outputBox.setWidth(hint.w());
        }

        widthDone = true;
      }
      if (policy.getVerticalPolicy() == sdl::core::SizePolicy::Fixed) {
        // Two distinct cases:
        // 1) The `hint` is valid, in which case we have to use it.
        // 2) The `hint` is not valid in which case we have to use the `desiredSize`.
        if (hint.isValid()) {
          outputBox.setHeight(hint.h());
        }

        heightDone = true;
      }

      // Check whether we should continue further.
      if (widthDone && heightDone) {
        return outputBox;
      }

      // At least one of the dimension is not set to fixed, so we have to check for
      // min and max sizes.
      if (outputBox.w() < minSize.w()) {
        outputBox.setWidth(minSize.w());
      }
      if (outputBox.h() < minSize.h()) {
        outputBox.setHeight(minSize.h());
      }

      if (outputBox.w() > maxSize.w()) {
        outputBox.setWidth(maxSize.w());
      }
      if (outputBox.h() > maxSize.h()) {
        outputBox.setHeight(maxSize.h());
      }

      // The last thing to check concerns the size policy. FOr example if the `desiredSize`
      // is larger than the provided hint, even though the `desiredSize` is smaller than the
      // `maxSize`, if the policy is not set to `Grow`, we should still use the `hint` size.
      // Same goes for the case where the `desiredSize` lies in the interval [`minSize`; `hint`]
      // and the policy is not set to `Shrink`: the `hint` should be used.
      // If course all this is only relevant if the hint is valid, otherwise we can use the
      // `desiredSize`.
      if (!hint.isValid()) {
        // Nothing more to do, the `desiredSize` can be used once clamped using the `minSize`
        // and `maxSize`.
        return outputBox;
      }

      // Check shrinking policy.
      if (outputBox.w() < hint.w() && !(policy.getHorizontalPolicy() | sdl::core::SizePolicy::Policy::Shrink)) {
        outputBox.setWidth(hint.w());
      }
      if (outputBox.h() < hint.h() && !(policy.getVerticalPolicy() | sdl::core::SizePolicy::Policy::Shrink)) {
        outputBox.setHeight(hint.h());
      }

      if (outputBox.w() > hint.w() && !(policy.getHorizontalPolicy() | sdl::core::SizePolicy::Policy::Expand)) {
        outputBox.setWidth(hint.w());
      }
      if (outputBox.h() > hint.h() && !(policy.getVerticalPolicy() | sdl::core::SizePolicy::Policy::Expand)) {
        outputBox.setHeight(hint.h());
      }

      // We can return the computed box.
      return outputBox;
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
