
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
    LinearLayout::updatePrivate(const sdl::core::Boxf& window) {
      // The LinearLayout allows to arrange widgets in a single direction
      // and to provide equal space to each widget. Widgets can override the
      // default behavior using a stretch, which tells the layout that the
      // widget should span a multiple of the initial available space.
      //
      // The space which is not spanned by the linear layout (i.e. vertical
      // space for an horizontal layout and horizontal space for a vertical
      // layout is set to be filled up by the widgets unless other indications
      // are specified in the widget's size policy.
      //
      // The input `window` specifies the available space for all the widgets
      // and represents the space to split.

      // Split the available space according to the number of elements to space.
      float cw, ch;
      switch(m_direction) {
        case Direction::Horizontal:
          handleHorizontalLayout(window, cw, ch);
          break;
        case Direction::Vertical:
        default:
          handleVerticalLayout(window, cw, ch);
          break;
      }

      // The `cw` and `ch` variables now hold the available width and height
      // for each logical _cell_ in this layout. We now can apply these values
      // to each widget based on its stretch.

      // Hold the current coordinates.
      float xStart = m_margin;
      float yStart = m_margin;

      // Update each widget handled by this layout.
      for (int indexItem = 0 ; indexItem < m_items.size() ; ++indexItem) {
        // Compute the span of this widget based on its stretch.
        const float w = m_items[indexItem]->getSizePolicy().getHorizontalStretch() * cw;
        const float h = m_items[indexItem]->getSizePolicy().getVerticalStretch() * ch;

        // Update the position and dimensions for this widget.
        m_items[indexItem]->setRenderingArea(
          sdl::core::Boxf(
            xStart + w / 2.0f,
            yStart + h / 2.0f,
            w,
            h
          )
        );

        // Update the starting coordinates.
        if (m_direction == Direction::Horizontal) {
          xStart += (w + m_componentMargin);
        }
        if (m_direction == Direction::Vertical) {
          yStart += (h + m_componentMargin);
        }
      }
    }

    void
    LinearLayout::handleHorizontalLayout(const sdl::core::Boxf& totalArea,
                                         float& cw,
                                         float& ch) const
    {
      // In the case of horizontal layout, we want to split up the space along
      // the horizontal axis (i.e. the available width) among all the widgets
      // and provide the maximum available space along the vertical axis (i.e.
      // the height) for each widget.

      // The width available for each widget corresponds to the total width minus
      // the global margin and the margin between components.
      const float availableWidth =
          totalArea.w()
        - 2.0f * m_margin
        - (m_items.size() - 1) * m_componentMargin
      ;

      // Now we need to split this available width among all the logical _cells_
      // defined by the widgets.
      // Indeed each widget can provide a `stretch` which allows to specify that
      // the widget should occupy a larger portion of the available space.
      // As an example if we have two widget `A` and `B` with `A.stretch = 1`
      // and `B.stretch = 2`, the layout will allocate space so that `B` is twice
      // the size of `A`.

      // Compute the number of logical _cells_ from the components.
      float cells = 0.0f;
      for (int indexItem = 0 ; indexItem < m_items.size() ; ++indexItem) {
        cells += m_items[indexItem]->getSizePolicy().getHorizontalStretch();
      }

      // The available width can now be split evenly between all the cells.
      cw = availableWidth / cells;

      // The height available for each widget corresponds to the total height
      // minus the margin.
      ch = totalArea.h() - 2.0f * m_margin;
    }

    inline
    void
    LinearLayout::handleVerticalLayout(const sdl::core::Boxf& totalArea,
                                       float& cw,
                                       float& ch) const
    {
      // In the case of vertical layout, we want to split up the space along
      // the vertical axis (i.e. the available height) among all the widgets
      // and provide the maximum available space along the horizontal axis
      // (i.e. the width) for each widget.

      // The width available for each widget corresponds to the total width
      // minus the margin.
      cw = totalArea.w() - 2.0f * m_margin;

      // The height available for each widget corresponds to the total height minus
      // the global margin and the margin between components.
      const float availableHeight =
          totalArea.h()
        - 2.0f * m_margin
        - (m_items.size() - 1) * m_componentMargin
      ;

      // Now we need to split this available height among all the logical _cells_
      // defined by the widgets.
      // Indeed each widget can provide a `stretch` which allows to specify that
      // the widget should occupy a larger portion of the available space.
      // As an example if we have two widget `A` and `B` with `A.stretch = 1`
      // and `B.stretch = 2`, the layout will allocate space so that `B` is twice
      // the size of `A`.

      // Compute the number of logical _cells_ from the components.
      float cells = 0.0f;
      for (int indexItem = 0 ; indexItem < m_items.size() ; ++indexItem) {
        cells += m_items[indexItem]->getSizePolicy().getVerticalStretch();
      }

      // The available width can now be split evenly between all the cells.
      ch = availableHeight / cells;
    }

  }
}
