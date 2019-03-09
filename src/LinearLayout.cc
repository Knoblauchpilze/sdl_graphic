
# include "LinearLayout.hh"
# include <unordered_set>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    LinearLayout::LinearLayout(const sdl::core::Layout::Direction& direction,
                               const float& margin,
                               const float& interMargin,
                               sdl::core::SdlWidget* widget):
      sdl::core::Layout(widget),
      m_direction(direction),
      m_margin(margin),
      m_componentMargin(interMargin)
    {
      // Nothing to do.
    }

    LinearLayout::~LinearLayout() {}

    void
    LinearLayout::updatePrivate(const sdl::utils::Boxf& window) {
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
      const sdl::utils::Sizef internalSize = computeAvailableSize(window);

      // Copy the current size of widgets so that we can work with it without
      // requesting constantly information or setting information multiple times.
      std::vector<WidgetInfo> widgetsInfo = computeWidgetsInfo();

      std::cout << "[LAY] Available size: " << window.w() << "x" << window.h() << std::endl;
      std::cout << "[LAY] Internal size: " << internalSize.w() << "x" << internalSize.h() << std::endl;

      std::vector<sdl::utils::Boxf> outputBoxes(m_items.size());

      // We now have a working set of dimensions which we can begin to apply to widgets
      // in order to build the layout.
      // In case a widget cannot be assigned the `defaultBox`, we update the two values
      // declared right now in order to keep track of additional space or missing space
      // for example in case the minimum/maximum size of a widget prevent it from being
      // set to the `defaultBox`.
      // After updating all widgets, we need to loop again to apply the space we have
      // not used up before. This process continues until we run out of space to allocate
      // (usually meaning that we could expand some widgets to take the space not used
      // by others) or if no more container can be expanded or shrinked without bypassing
      // the sizes provided to bound the widgets.
      std::unordered_set<unsigned> widgetsToAdjust;

      // In a first approach all the widgets can be adjusted.
      for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
        widgetsToAdjust.insert(index);
      }

      // Also assume that we didn't use up all the available space.
      sdl::utils::Sizef spaceToUse = internalSize;
      bool allSpaceUsed = false;

      // Loop until no more widgets can be used to adjust the space needed or all the
      // available space has been used up.
      // TODO: Handle cases where the widgets are too large to fit into the widget ?
      while (!widgetsToAdjust.empty() && !allSpaceUsed) {

        // Compute the amount of space we will try to allocate to each widget still
        // available for adjustment.
        // The `defaultBox` is computed by dividing equally the remaining `workingSize`
        // among all the available widgets.
        const sdl::utils::Sizef defaultBox = computeDefaultWidgetBox(spaceToUse, widgetsToAdjust.size());

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
          sdl::utils::Sizef area = computeSizeFromPolicy(defaultBox, outputBoxes[*widget], widgetsInfo[*widget]);
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
        sdl::utils::Sizef achievedSize = computeSizeOfWidgets(getDirection(), outputBoxes);

        // Check whether all the space have been used.
        if (achievedSize.fuzzyEqual(internalSize, 1.0f)) {
          // We used up all the available space, no more adjustments to perform.
          allSpaceUsed = true;
          continue;
        }

        // All space has not been used. Update the relevant `spaceToUse` in order to perform
        // the next iteration.
        spaceToUse = computeSpaceAdjustmentNeeded(achievedSize, internalSize);

        // Determine the policy to apply based on the achieved size.
        const sdl::core::SizePolicy action = shrinkOrGrow(internalSize, achievedSize);

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

        // Use the computed list of widgets to perform the next action in order
        // to reach the desired space.
        widgetsToAdjust.swap(widgetsToUse);

        char c;
        std::cout << "[ENTER] Key: ";
        std::cin >> c;
      }

      if (!allSpaceUsed) {
        std::cout << "[LAY] Exited because no more widgets can occupy space" << std::endl;
      }

      // All widgets have suited dimensions, we can now handle the position of each
      // widget. We basically just move each widget side by side based on their
      // dimensions and adding margins.
      float x = m_margin;
      float y = m_margin;

      for (unsigned index = 0u ; index < m_items.size() ; ++index) {
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
          throw sdl::core::SdlException(std::string("Unknown direciton when updating linear layout"));
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

  }
}
