#ifndef    LINEARLAYOUT_HXX
# define   LINEARLAYOUT_HXX

# include <sdl_core/SdlException.hh>
# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    const sdl::core::Layout::Direction&
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
    sdl::utils::Sizef
    LinearLayout::computeWorkingSize(const sdl::utils::Sizef& size,
                                     const sdl::utils::Sizef& unavailable) const
    {
      if (getDirection() == Direction::Horizontal) {
        return sdl::utils::Sizef(size.w() - unavailable.w(), size.h());
      }
      else if (getDirection() == Direction::Vertical) {
        return sdl::utils::Sizef(size.w(), size.h() - unavailable.h());
      }
      else {
        throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
      }
    }

    inline
    sdl::utils::Sizef
    LinearLayout::computeSizeFromPolicy(const sdl::utils::Sizef& desiredSize,
                                        const WidgetInfo& info) const
    {
      // Create the return size and assume the desired size is valid.
      sdl::utils::Sizef outputBox(desiredSize);

      bool widthDone = false;
      bool heightDone = false;

      // Check the policy for fixed size. If the policy is fixed, we should assign
      // the `hint` size whatever the input `desiredSize`. Except of course if the
      // `hint` is not a valid size, in which case we can use the `desiredSize`.
      if (info.policy.getHorizontalPolicy() == sdl::core::SizePolicy::Fixed) {
        // Two distinct cases:
        // 1) The `hint` is valid, in which case we have to use it.
        // 2) The `hint` is not valid in which case we have to use the `desiredSize`.
        if (info.hint.isValid()) {
          outputBox.setWidth(info.hint.w());
        }

        widthDone = true;
      }
      if (info.policy.getVerticalPolicy() == sdl::core::SizePolicy::Fixed) {
        // Two distinct cases:
        // 1) The `hint` is valid, in which case we have to use it.
        // 2) The `hint` is not valid in which case we have to use the `desiredSize`.
        if (info.hint.isValid()) {
          outputBox.setHeight(info.hint.h());
        }

        heightDone = true;
      }

      // Check whether we should continue further.
      if (widthDone && heightDone) {
        return outputBox;
      }

      // At least one of the dimension is not set to fixed, so we have to check for
      // min and max sizes.
      if (outputBox.w() < info.min.w()) {
        outputBox.setWidth(info.min.w());
      }
      if (outputBox.h() < info.min.h()) {
        outputBox.setHeight(info.min.h());
      }

      if (outputBox.w() > info.max.w()) {
        outputBox.setWidth(info.max.w());
      }
      if (outputBox.h() > info.max.h()) {
        outputBox.setHeight(info.max.h());
      }

      // The last thing to check concerns the size policy. FOr example if the `desiredSize`
      // is larger than the provided hint, even though the `desiredSize` is smaller than the
      // `maxSize`, if the policy is not set to `Grow`, we should still use the `hint` size.
      // Same goes for the case where the `desiredSize` lies in the interval [`minSize`; `hint`]
      // and the policy is not set to `Shrink`: the `hint` should be used.
      // If course all this is only relevant if the hint is valid, otherwise we can use the
      // `desiredSize`.
      if (!info.hint.isValid()) {
        // Nothing more to do, the `desiredSize` can be used once clamped using the `minSize`
        // and `maxSize`.
        return outputBox;
      }

      // Check shrinking policy.
      if (outputBox.w() < info.hint.w() && !(info.policy.getHorizontalPolicy() & sdl::core::SizePolicy::Policy::Shrink)) {
        outputBox.setWidth(info.hint.w());
      }
      if (outputBox.h() < info.hint.h() && !(info.policy.getVerticalPolicy() & sdl::core::SizePolicy::Policy::Shrink)) {
        outputBox.setHeight(info.hint.h());
      }

      if (outputBox.w() > info.hint.w() && !(info.policy.getHorizontalPolicy() & sdl::core::SizePolicy::Policy::Expand)) {
        outputBox.setWidth(info.hint.w());
      }
      if (outputBox.h() > info.hint.h() && !(info.policy.getVerticalPolicy() & sdl::core::SizePolicy::Policy::Expand)) {
        outputBox.setHeight(info.hint.h());
      }

      // We can return the computed box.
      return outputBox;
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
