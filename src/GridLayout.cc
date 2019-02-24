
# include "GridLayout.hh"
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

      m_columnsMinimumWidth(columns, 0.0f),
      m_rowsMinimumHeight(rows, 0.0f),

      m_columnsStretches(columns, 1.0f),
      m_rowsStretches(rows, 1.0f),

      m_margin(margin),
      m_itemsLocation()
    {
      // Nothing to do.
    }

    GridLayout::~GridLayout() {}

    void
    GridLayout::updatePrivate(const sdl::core::Boxf& window) {
      // Compute the dimensions available for each logical _cell_
      // defines for columns and rows.
      float cw;
      computeColumnsWidth(window.w(), cw);

      float rh;
      computeRowsHeight(window.h(), rh);

      // The `cw` and `rh` variables now hold the available width and height
      // for each logical _cell_ in this layout. We now can apply these values
      // to each column/row based on its stretch.

      // Compute the origins for each column/row.
      std::vector<float> co = computeColumnsOrigin(cw);
      std::vector<float> ro = computeRowsOrigin(rh);

      // We now need to process each column/row based on the computed dimensions.
      for (std::unordered_map<int, ItemInfo>::const_iterator item = m_itemsLocation.cbegin() ;
           item != m_itemsLocation.cend() ;
           ++item)
      {
        // Retrieve the properties of the widget.
        const int widgetId = item->first;
        const ItemInfo info = item->second;

        // This widget spans across the columns `info.x` through `info.x + info.w`.
        // It also spans across the rows `info.y` through `info.y + info.h`.
        // We can compute the total width and height for this widget based on the
        // dimensions a single logical _cell_ and the stretch for all the spanning
        // column.
        float w;
        float h;
        computeWidgetSpan(info, cw, rh, w, h);

        // Update the position and dimensions for this widget.
        m_items[widgetId]->setRenderingArea(
          sdl::core::Boxf(
            co[info.x] + w / 2.0f,
            ro[info.y] + h / 2.0f,
            w,
            h
          )
        );
      }
    }

  }
}
