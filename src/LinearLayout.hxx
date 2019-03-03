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
    sdl::utils::Sizef
    LinearLayout::computeIncompressibleSize(std::unordered_set<unsigned>& fixedWidgetsAlongDirection,
                                            const std::vector<WidgetInfo>& widgets) const
    {
      float flowingSize = 0.0f;
      float perpendicularSize = 0.0f;

      for (unsigned index = 0u ; index < m_items.size() ; ++index) {
        float increment = 0.0f;
        float size = 0.0f;

        if (getDirection() == Direction::Horizontal) {
          // This layout stacks widgets using an horizontal flow:
          // we should add the incompressible size of this widget
          // if it has any in the horizontal direction and retrieve
          // its vertical size if any.
          if (widgets[index].policy.getVerticalPolicy() == sdl::core::SizePolicy::Fixed) {
            size = widgets[index].hint.h();
          }
          increment = widgets[index].hint.w();

          // Mark this widget as fixed in the layout's direction.
          if (widgets[index].hint.isValid() && widgets[index].policy.getHorizontalPolicy() == sdl::core::SizePolicy::Fixed) {
            fixedWidgetsAlongDirection.insert(index);
          }
        }
        else if (getDirection() == Direction::Vertical) {
          // This layout stacks widgets using a vertical flow:
          // we should add the incompressible size of this widget
          // if it has any in the vertical direction and retrieve
          // its horizontal size if any.
          if (widgets[index].policy.getHorizontalPolicy() == sdl::core::SizePolicy::Fixed) {
            size = widgets[index].hint.w();
          }
          increment = widgets[index].hint.h();

          // Mark this widget as fixed in the layout's direction.
          if (widgets[index].hint.isValid() && widgets[index].policy.getVerticalPolicy() == sdl::core::SizePolicy::Fixed) {
            fixedWidgetsAlongDirection.insert(index);
          }
        }
        else {
          throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
        }

        // Increse the `incompressibleSize` with the provided `size` (which may be
        // 0 if the widget does not have a valid size hint) and performs a comparison
        // of the size of the widget in the other direction (i.e. not in the direction
        // of the flow) against the current maximum and update it if needed.
        flowingSize += increment;
        if (perpendicularSize < size) {
          perpendicularSize = size;
        }
      }

      // Create a valid size based on this layout's direction.
      if (getDirection() == Direction::Horizontal) {
        return sdl::utils::Sizef(flowingSize, perpendicularSize);
      }
      else if (getDirection() == Direction::Vertical) {
        return sdl::utils::Sizef(perpendicularSize, flowingSize);
      }
      else {
        throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
      }
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
