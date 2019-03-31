
# include "LinearLayout.hh"

# include <unordered_set>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    LinearLayout::LinearLayout(const sdl::core::Layout::Direction& direction,
                               const float& margin,
                               const float& interMargin,
                               sdl::core::SdlWidget* widget):
      sdl::core::Layout(widget, margin),
      m_direction(direction),
      m_componentMargin(interMargin)
    {
      // Nothing to do.
    }

    LinearLayout::~LinearLayout() {}

    void
    LinearLayout::updatePrivate(const utils::Boxf& window) {
      // The `LinearLayout` allows to arrange widgets using a flow along a
      // specified axis. The default behavior is to provide an equal allocation
      // of the available space to all widgets, but also to take into account
      // the provided stretch factors in order to obtain a nice growing/shrinking
      // behavior if needed.
      //
      // Widgets are stretched to use all the space in which the layout is not
      // flowing (i.e. vertical space for horizontal layout and horizontal space
      // for vertical layout) unless other indications are specified in the
      // widget's size policy.

      // First, we need to compute the available size for this layout. We need
      // to take into account margins.
      const utils::Sizef internalSize = computeAvailableSize(window);

      // Copy the current size of widgets so that we can work with it without
      // requesting constantly information or setting information multiple times.
      std::vector<WidgetInfo> widgetsInfo = computeWidgetsInfo();

      std::cout << "[LAY] Available size: " << window.w() << "x" << window.h() << std::endl;
      std::cout << "[LAY] Internal size: " << internalSize.w() << "x" << internalSize.h() << std::endl;

      std::vector<utils::Boxf> outputBoxes(getItemsCount());

      // We now have a working set of dimensions which we can begin to apply to widgets
      // in order to build the layout.
      // In case a widget cannot be assigned the `defaultBox`, we update the two values
      // declared right now in order to keep track of additional space or missing space
      // for example in case the minimum/maximum size of a widget prevent it from being
      // set to the `defaultBox`.
      // After updating all widgets, we need to loop again to apply the space we have
      // not used up before. This process continues until we run out of space to allocate
      // (usually meaning that we could expand some widgets to take the space not used
      // by others) or if no more container can be expanded or shrunk without bypassing
      // the sizes provided to bound the widgets.
      std::unordered_set<unsigned> widgetsToAdjust;

      // In a first approach all the widgets can be adjusted.
      for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
        widgetsToAdjust.insert(index);
      }

      // Also assume that we didn't use up all the available space.
      utils::Sizef spaceToUse = internalSize;
      bool allSpaceUsed = false;

      // Loop until no more widgets can be used to adjust the space needed or all the
      // available space has been used up.
      // TODO: Handle cases where the widgets are too large to fit into the widget ?
      while (!widgetsToAdjust.empty() && !allSpaceUsed) {

        // Compute the amount of space we will try to allocate to each widget still
        // available for adjustment.
        // The `defaultBox` is computed by dividing equally the remaining `spaceToUse`
        // among all the available widgets.
        const utils::Sizef defaultBox = computeDefaultWidgetBox(spaceToUse, widgetsToAdjust.size());

        std::cout << "[LAY] Default box is " << defaultBox.w() << "x" << defaultBox.h() << std::endl;

        // Loop on all the widgets that can still be used to adjust the space used by
        // this layout and perform the size adjustements.
        for (std::unordered_set<unsigned>::const_iterator widget = widgetsToAdjust.cbegin() ;
             widget != widgetsToAdjust.cend() ;
             ++widget)
        {
          // Try to assign the `defaultBox` to this widget: we use a dedicated handler
          // to handle the case where the provided space is too large/small/not suited
          // to the widget for some reasons, in which case the handler will provide a
          // size which can be applied to the widget.
          utils::Sizef area = computeSizeFromPolicy(outputBoxes[*widget], defaultBox, widgetsInfo[*widget]);
          outputBoxes[*widget].w() = area.w();
          outputBoxes[*widget].h() = area.h();

          std::cout << "[LAY] Widget \"" << m_items[*widget]->getName() << "\": "
                    << outputBoxes[*widget].x() << ", " << outputBoxes[*widget].y()
                    << ", dims: "
                    << outputBoxes[*widget].w() << ", " << outputBoxes[*widget].h()
                    << std::endl;
        }

        // We have tried to apply the `defaultBox` to all the widgets. This might have fail
        // in some cases (for example due to a `Fixed` size policy for a widget) and thus
        // we might end up with a total size for all the widget different from the one desired
        // and expected when the `defaultBox` has been computed.
        // In order to fix things, we must compute the deviation from the expected size and
        // try to allocate the remaining space to other widgets (or remove the missing space
        // from widgets which can give up some).

        // Compute the total size of the bounding boxes.
        utils::Sizef achievedSize = computeSizeOfWidgets(outputBoxes);

        // Check whether all the space have been used.
        if (achievedSize.fuzzyEqual(internalSize, 0.5f)) {
          // We used up all the available space, no more adjustments to perform.
          allSpaceUsed = true;
          continue;
        }

        // All space has not been used. Update the relevant `spaceToUse` in order to perform
        // the next iteration.
        spaceToUse = computeSpaceAdjustmentNeeded(achievedSize, internalSize);

        // Determine the policy to apply based on the achieved size.
        const sdl::core::SizePolicy action = shrinkOrGrow(internalSize, achievedSize, 0.5f);

        std::cout << "[LAY] Desired: " << internalSize.w() << ", " << internalSize.h()
                  << " achieved: " << achievedSize.w() << ", " << achievedSize.h()
                  << " space: " << spaceToUse.w() << ", " << spaceToUse.h()
                  << std::endl;

        // We now know what should be done to make the `achievedSize` closer to `desiredSize`.
        // Based on the `policy` provided by the base class method, we can now determine which
        // widget should be used to perform the needed adjustments.
        std::unordered_set<unsigned> widgetsToUse;
        for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
          // Check whether this widget can be used to grow/shrink.
          std::pair<bool, bool> usable = canBeUsedTo(m_items[index]->getName(), widgetsInfo[index], outputBoxes[index], action);
          if ((usable.first && getDirection() == Direction::Horizontal) ||
              (usable.second && getDirection() == Direction::Vertical))
          {
            std::cout << "[LAY] " << m_items[index]->getName() << " can be used to "
                      << std::to_string(static_cast<int>(action.getHorizontalPolicy()))
                      << " and "
                      << std::to_string(static_cast<int>(action.getVerticalPolicy()))
                      << std::endl;
            widgetsToUse.insert(index);
          }
        }

        // There's one more thing to determine: the `Expanding` flag on any widget's policy should
        // mark it as priority over other widgets. For example if two widgets can grow, one having
        // the flag `Grow` and the other the `Expand` flag, we should make priority for the one
        // with `Expanding` flag.
        // Widgets with `Grow` flag will only grow when all `Expanding` widgets have been maxed out.
        // Of course this does not apply in case widgets should be shrunk: all widgets are treated
        // equally in this case and there's not preferred widgets to shrink.
        if ((getDirection() == Direction::Horizontal && action.canExtendHorizontally()) ||
            (getDirection() == Direction::Vertical && action.canExtendVertically()))
        {
          // Select only `Expanding` widget if any.
          std::unordered_set<unsigned> widgetsToExpand;

          for (std::unordered_set<unsigned>::const_iterator widget = widgetsToUse.cbegin() ;
               widget != widgetsToUse.cend() ;
               ++widget)
          {
            // Check whether this widget can expand.
            if (getDirection() == Direction::Horizontal && widgetsInfo[*widget].policy.canExpandHorizontally()) {
              std::cout << "[LAY] " << m_items[*widget]->getName() << " can be expanded horizontally" << std::endl;
              widgetsToExpand.insert(*widget);
            }
            if (getDirection() == Direction::Vertical && widgetsInfo[*widget].policy.canExpandVertically()) {
              std::cout << "[LAY] " << m_items[*widget]->getName() << " can be expanded vertically" << std::endl;
              widgetsToExpand.insert(*widget);
            }
          }

          std::cout << "[LAY] Saved " << widgetsToExpand.size() << " which can expand compared to "
                    << widgetsToUse.size() << " which can extend"
                    << std::endl;
          // Check whether we could select at least one widget to expand: if this is not the
          // case we can proceed to extend the widget with only a `Grow` flag.
          if (!widgetsToExpand.empty()) {
            widgetsToUse.swap(widgetsToExpand);
          }
        }


        // Use the computed list of widgets to perform the next action in order
        // to reach the desired space.
        widgetsToAdjust.swap(widgetsToUse);
      }

      if (!allSpaceUsed) {
        std::cout << "[LAY] Exited because no more widgets can occupy space" << std::endl;
      }

      // All widgets have suited dimensions, we can now handle the position of each
      // widget. We basically just move each widget side by side based on their
      // dimensions and adding margins.
      float x = getMargin().w();
      float y = getMargin().h();

      for (unsigned index = 0u ; index < getItemsCount() ; ++index) {
        // Position the widget based on the position of the previous ones.
        // In addition to this mechanism, we should handle some kind of
        // centering to allow widgets with sizes smaller than the provided
        // layout's dimensions to still be nicely displayed in the center
        // of the layout.
        // To handle this case we check whether the dimensions of the size
        // of the widget is smaller than the dimension stored in `internalSize`
        // in which case we can center it.
        // The centering only takes place in the perpendicular direction of
        // the flow of the layout (e.g. vertical direction for horizontal
        // layout and horizontal direction for vertical layout).
        float xWidget = x + outputBoxes[index].w() / 2.0f;
        float yWidget = y + outputBoxes[index].h() / 2.0f;

        if (getDirection() == Direction::Horizontal && outputBoxes[index].h() < internalSize.h()) {
          yWidget += ((internalSize.h() - outputBoxes[index].h()) / 2.0f);
        }
        if (getDirection() == Direction::Vertical && outputBoxes[index].w() < internalSize.w()) {
          xWidget += ((internalSize.w() - outputBoxes[index].w()) / 2.0f);
        }

        outputBoxes[index].x() = xWidget;
        outputBoxes[index].y() = yWidget;

        // Update the position for the next widget based on the layout's
        // direction.
        if (getDirection() == Direction::Horizontal) {
          x += (outputBoxes[index].w() + m_componentMargin);
        }
        else if (getDirection() == Direction::Vertical) {
          y += (outputBoxes[index].h() + m_componentMargin);
        }
        else {
          error(std::string("Unknown direciton when updating linear layout"));
        }
      }

      for (unsigned index = 0u ; index < outputBoxes.size() ; ++index) {
        std::cout << "[WIG] Widget \"" << m_items[index]->getName() << "\" has: "
                  << outputBoxes[index].x() << ", " << outputBoxes[index].y()
                  << ", dims: "
                  << outputBoxes[index].w() << ", " << outputBoxes[index].h()
                  << std::endl;
      }

      // Assign the rendering area to widgets.
      assignRenderingAreas(outputBoxes);
    }



    utils::Sizef
    LinearLayout::computeSizeOfWidgets(const std::vector<utils::Boxf>& boxes) const
    {
      float flowingSize = 0.0f;
      float perpendicularSize = 0.0f;

      for (unsigned index = 0u ; index < boxes.size() ; ++index) {
        float increment = 0.0f;
        float size = 0.0f;

        if (getDirection() == Direction::Horizontal) {
          // The `increment` is given by the width of the box while the
          // `size` is given by its height.
          size = boxes[index].h();
          increment = boxes[index].w();
        }
        else if (getDirection() == Direction::Vertical) {
          // The `increment` is given by the height of the box while the
          // `size` is given by its width.
          size = boxes[index].w();
          increment = boxes[index].h();
        }
        else {
          error(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
        }

        // Increase the `flowingSize` with the provided `increment` and
        // perform a comparison of the size of the widget in the other
        // direction (i.e. not in the direction of the flow) against the
        // current maximum and update it if needed.
        flowingSize += increment;
        if (perpendicularSize < size) {
          perpendicularSize = size;
        }
      }

      // Create a valid size based on this layout's direction.
      if (getDirection() == Direction::Horizontal) {
        return utils::Sizef(flowingSize, perpendicularSize);
      }
      else if (getDirection() == Direction::Vertical) {
        return utils::Sizef(perpendicularSize, flowingSize);
      }
      else {
        error(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
      }

      // Return dummy value because the `error` statement already handles throwing.
      return utils::Sizef();
    }

  }
}
