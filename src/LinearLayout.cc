
# include "LinearLayout.hh"
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    LinearLayout::LinearLayout(const Direction& direction,
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
      // specified axis. THe default behavior is to provide an equal allocation
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
      std::vector<sdl::core::SizePolicy> widgetsPolicies(m_items.size());
      std::vector<sdl::utils::Sizef> widgetsMins(m_items.size());
      std::vector<sdl::utils::Sizef> widgetsHints(m_items.size());
      std::vector<sdl::utils::Sizef> widgetsMaxs(m_items.size());
      std::vector<sdl::utils::Boxf> widgetsBoxes(m_items.size());
      for (unsigned index = 0u ; index < m_items.size() ; ++index) {
        widgetsPolicies[index] = m_items[index]->getSizePolicy();
        widgetsMins[index] = m_items[index]->getMinSize();
        widgetsHints[index] = m_items[index]->getSizeHint();
        widgetsMaxs[index] = m_items[index]->getMaxSize();
        widgetsBoxes[index] = m_items[index]->getRenderingArea();
      }

      // Now we need to compute and assign size information to each widget based
      // on its policy and required min/hint/max dimensions. We also need to
      // provide a first valid size hint for widgets which still don't have any.
      // In a first approach, we will try to allocate fairly the available space
      // among all the widgets.
      // This approach is likely to fail because some widgets will have specific
      // constraints, in which case we will modify the default behavior to account
      // for these modifications.

      // We need to first account for the fixed size widgets which have a defined
      // size hint: these widgets will be the first to handle we have no margin in
      // handling their definitive size. Indeed it must stay the size provided by
      // the size hint.
      // In order to ease the computations, we can just ignore the space occupied
      // by these widgets and subtract it from the available size.

      // Build the set of widgets which are fixed and aiwht a valid size hint. This
      // will build the first basis to exclude from further computations the widgets
      // which have been successfully handled.
      std::unordered_set<unsigned> handledWidgets;
      float incompressibleSize = computeIncompressibleSize(
        handledWidgets,
        widgetsPolicies,
        widgetsHints
      );

      // Check whether we have some margin to perform an adjustment: if even at this
      // point the incompressible size is larger than the available size we're screwed.
      float relevantSize = (getDirection() == Direction::Horizontal ? internalSize.w() : internalSize.h());
      if (incompressibleSize > relevantSize) {
        throw sdl::core::SdlException(
          std::string("Cannot handle linear layout, ") +
          "available size is " + std::to_string(relevantSize) +
          " but widgets occupy at least " + std::to_string(incompressibleSize)
        );
      }

      // Compute the working size available for this layout by subtracting the chunk
      // of incompressible size occupied by fixed sized widgets with valid size hints.
      sdl::utils::Sizef workingSize = computeWorkingSize(internalSize, incompressibleSize);
      
      // Compute the default box to assign to each not handled yet widget.
      const sdl::utils::Sizef defaultBox = computeDefaultWidgetBox(workingSize, m_items.size() - handledWidgets.size());

      std::cout << "[LAY] Available size: " << window.w() << "x" << window.h() << std::endl;
      std::cout << "[LAY] Internal size: " << internalSize.w() << "x" << internalSize.h() << std::endl;
      std::cout << "[LAY] Working size: " << workingSize.w() << "x" << workingSize.h() << std::endl;
      std::cout << "[LAY] Default box is " << defaultBox.w() << "x" << defaultBox.h() << std::endl;

      std::vector<sdl::utils::Boxf> outputBoxes(m_items.size());

      // We now have a working set of dimensions which we can begin to apply to widgets
      // in order to build the layout.
      // In case a widget cannot be assigned the `defaultBox`, we update the two values
      // declared right now in order to keep track of additional space or missing space
      // for example in case the minimum/maximum size of a widget prevent it from being
      // set to the `defaultBox`.
      float extraWidth = 0.0f;
      float extraHeight = 0.0f;

      for (unsigned index = 0u ; index < widgetsBoxes.size() ; ++index) {
        // We now enter the core of the widgets' dimensions update.
        // The aim of this loop is to apply the `defaultBox` to as many
        // widgets as possible.
        // The exceptions to this rule occur when:
        // 1) The widget has a fixed policy and a valid size hint.
        // 2) The widget has a minimum size larger than the required
        //    size.
        // 3) The widget has a maximum size smaller than the required
        //    size.
        // In any other case, we can apply the `defaultBox` to the
        // widget without problems.

        // Check whether the size hint for this widget is valid
        if (widgetsHints[index].isValid()) {
          // The size hint is valid, this gives a first indication regarding
          // dimensions for this widget.
        }
        else {
          // No valid hint for this widget, we should be able to apply
          // the `defaultBox` if possible. This scenario only matters if
          // the minimum or maximum size conflicts with the desired
          // `defaultBox` though, otherwise everything's fine.
          
          sdl::utils::Sizef area = computeSizeFromPolicy(
            defaultBox,
            widgetsPolicies[index],
            widgetsMins[index],
            widgetsHints[index],
            widgetsMaxs[index]
          );
          outputBoxes[index].w() = area.w();
          outputBoxes[index].h() = area.h();

          // Update the extra width/height if the box is not identical to the
          // provided one.
          extraWidth += (defaultBox.w() - area.w());
          extraHeight += (defaultBox.h() - area.h());
        }

        std::cout << "[LAY] Widget \"" << m_items[index]->getName() << "\": "
                  << outputBoxes[index].x() << ", " << outputBoxes[index].y()
                  << ", dims: "
                  << outputBoxes[index].w() << ", " << outputBoxes[index].h()
                  << ", extra dims: "
                  << extraWidth << ", " << extraHeight
                  << std::endl;
      }

      // All widgets have suited dimensions, we can now handle the position of each
      // widget. We basically just move each widget side by side based on their
      // dimensions and adding margins.
      float x = m_margin;
      float y = m_margin;

      for (unsigned index = 0u ; index < m_items.size() ; ++index) {
        // Position the widget based on the position of the previous ones.
        // TODO : Handle centering of widget with smaller dimensions than the
        // required dimensions.
        outputBoxes[index].x() = x + outputBoxes[index].w() / 2.0f;
        outputBoxes[index].y() = y + outputBoxes[index].h() / 2.0f;

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
      for (unsigned index = 0u; index < outputBoxes.size() ; ++index) {
        m_items[index]->setRenderingArea(outputBoxes[index]);
      }
    }

  }
}
