
# include "GridLayout.hh"
# include <unordered_set>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    GridLayout::GridLayout(const unsigned& columns,
                           const unsigned& rows,
                           const float& margin,
                           sdl::core::SdlWidget* widget):
      sdl::core::Layout(widget),
      m_columns(columns),
      m_rows(rows),

      m_columnsInfo(),
      m_rowsInfo(),

      m_margin(margin),
      m_itemsLocation()
    {
      // Build default information for columns/rows.
      resetGridInfo();
    }

    GridLayout::~GridLayout() {}

    void
    GridLayout::updatePrivate(const sdl::utils::Boxf& window) {
      // The `GridLayout` allows to arrange widgets using across a virtual
      // grid composed of `m_columns` columns and `m_rows` rows. The default
      // behavior is to provide an equal allocation of the available space
      // to all the widgets, but also to take into account the provided
      // information about widgets' preferred size and bounds.
      //
      // The process includes adjusting rows and columns by validating that
      // the final dimensions do correspond to the criteria applied to all
      // the widgets registered for a single column/row.

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
      // Basically we will try to assign a `ðefaultBox` to all widgets, corresponding to
      // the ideal value to use to allocate fairly the space between all widgets. Widgets
      // will handle internally this assignment by checking it against internal
      // constraints (min and max size for example) and use the best size considering the
      // input request.
      // Once we're done, we can compute the space left (or missing) and start the
      // process again with the remaining size.
      // We stop the process when the space has been entirely allocated to widgets or
      // when there's no widget left to expand/shrink without violating the provided
      // size constraints.
      
      // Compute the amount of space we will try to allocate to each widget still
      // available for adjustment.
      // The `defaultBox` is computed by dividing equally the remaining `workingSize`
      // among all the available widgets.
      const sdl::utils::Sizef defaultBox = computeDefaultWidgetBox(internalSize, m_columns, m_rows);

      std::cout << "[LAY] Default box is " << defaultBox.w() << "x" << defaultBox.h() << std::endl;

      for (unsigned index = 0u ; index < widgetsInfo.size() ; ++index) {
        // Retrieve the `ItemInfo` struct for this widget.
        const std::unordered_map<int, ItemInfo>::const_iterator itemInfo = m_itemsLocation.find(index);
        if (itemInfo == m_itemsLocation.cend()) {
          throw sdl::core::SdlException(
            std::string("Could not retrieve information for widget \"") +
            m_items[index]->getName() + "\" while updating grid layout"
          );
        }

        // Try to assign the `defaultBox` to this widget: we use a dedicated handler
        // to handle the case where the provided space is too large/small/not suited
        // to the widget for some reasons, in which case the handler will provide a
        // size which can be applied to the widget.
        // The process is not completely straightforward though as we need to account
        // for widgets spanning multiple columns/rows. The `defaultBox` should thus
        // be scaled to account for this.

        // Scale the `defaultBox`.
        const sdl::utils::Sizef widgetBox(defaultBox.w() * itemInfo->second.w, defaultBox.h() * itemInfo->second.h);

        // Apply the policy for this widget.
        sdl::utils::Sizef area = computeSizeFromPolicy(widgetBox, outputBoxes[index], widgetsInfo[index]);
        outputBoxes[index].w() = area.w();
        outputBoxes[index].h() = area.h();

        std::cout << "[LAY] Widget \"" << m_items[index]->getName() << "\": "
                  << outputBoxes[index].x() << ", " << outputBoxes[index].y()
                  << ", dims: "
                  << outputBoxes[index].w() << ", " << outputBoxes[index].h()
                  << std::endl;
      }

      // TODO: Handle some kind of columns/rows dimensions so that we can compute the position of each widget.
      // To do so we can at each end of the loop line `75` performs a computation which fetches for each column
      // or row the dimensions and store it into a dedicated vector.

      // Assign the rendering area to widgets.
      assignRenderingAreas(outputBoxes);
    }

  }
}
