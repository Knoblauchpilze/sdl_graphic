
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
      std::vector<sdl::utils::Sizef> widgetsHints(m_items.size());
      std::vector<sdl::utils::Boxf> widgetsBoxes(m_items.size());
      for (unsigned index = 0u ; index < m_items.size() ; ++index) {
        widgetsPolicies[index] = m_items[index]->getSizePolicy();
        widgetsHints[index] = m_items[index]->getSizeHint();
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
      
      // Compute the default box to assign to each widget.
      const sdl::utils::Sizef defaultBox = computeDefaultWidgetBox(internalSize);

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
      sdl::utils::Sizef workingSize;
      if (getDirection() == Direction::Horizontal) {
        workingSize = sdl::utils::Sizef(internalSize.w() - incompressibleSize, internalSize.h());
      }
      else if (getDirection() == Direction::Vertical) {
        workingSize = sdl::utils::Sizef(internalSize.w(), internalSize.h() - incompressibleSize);
      }
      else {
        throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
      }

      std::cout << "[LAY] Available size: " << window.w() << "x" << window.h() << std::endl;
      std::cout << "[LAY] Internal size: " << internalSize.w() << "x" << internalSize.h() << std::endl;
      std::cout << "[LAY] Working size: " << workingSize.w() << "x" << workingSize.h() << std::endl;
    }

  }
}
