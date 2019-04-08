
# include "GridLayout.hh"

# include <unordered_set>
# include <sdl_core/SdlWidget.hh>

# include <iomanip>

namespace sdl {
  namespace graphic {

    GridLayout::GridLayout(const unsigned& columns,
                           const unsigned& rows,
                           const float& margin,
                           sdl::core::SdlWidget* widget):
      sdl::core::Layout(widget, margin),
      m_columns(columns),
      m_rows(rows),

      m_columnsInfo(),
      m_rowsInfo(),

      m_locations()
    {
      // Build default information for columns/rows.
      resetGridInfo();
    }

    GridLayout::~GridLayout() {}

    void
    GridLayout::updatePrivate(const utils::Boxf& window) {
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
      const utils::Sizef internalSize = computeAvailableSize(window);

      // Compute default columns and rows dimensions.
      std::vector<CellInfo> cells = computeCellsInfo();

      // Copy the current size of widgets so that we can work with it without
      // requesting constantly information or setting information multiple times.
      std::vector<WidgetInfo> widgetsInfo = computeWidgetsInfo();

      // Once this is done, we can start applying specific behavior to this layout.
      // The first thing we want to do is handling the minimum column width and
      // minimum row height attributes. These are specified on a per column/row
      // basis and should override the minimum hint provided by the widgets if it
      // is allowed by the constraints.
      // Typically let's figure some widgets with the following properties:
      //
      // Widget 1:
      //  Min  : [100, 100]
      //  Hint : [150, 150]
      //  Max  : [200, 200]
      //
      // Widget 2:
      //  Min  : [300, 100]
      //  Hint : [undefined]
      //  Max  : [undefined]
      //
      // Widget 3:
      //  Min  : [110, 110]
      //  Hint : [120, 120]
      //  Max  : [150, 150]
      //
      // Widget 4:
      //  Min  : [50, 50]
      //  Hint : [60, 60]
      //  Max  : [90, 90]
      //
      // All three widgets are supposed to land in a column with a minimum column
      // width of 130.
      // Let's also assume that the total size of the widget allows to fit the widgets
      // without problem. We will not details the optimization process and consider that
      // the first assigned size is the final one.
      //
      // The first widget will see its minimum size raised (please not that it will only
      // affect the computations inside this layout and not the actual minimum size of
      // this widget) to [130, 130] and be assigned its hint anyway (of [150, 150]).
      //
      // The second widget already has a minimum size larger than the minimum column
      // width so nothing will happen and it will be assigned a size based on the layout
      // preferred size (as no hint is provided).
      //
      // The third widget has a minimum size smaller than the minimum column size so the
      // layout will try to raise the minimum size to [130, 130]. From there, we see that
      // it conflicts with the provided hint so there's too main case:
      // 1) The widget's policy does allow to grow in which case the minimum size and the
      //    hint will be set to [130, 130].
      // 2) The widget's policy does not allow to grow (which would be weird considered
      //    that the widget has a maximum size) in which case the minimum size will be
      //    raised to [120, 120].
      //    The rest of the widgets though will still try to use [130, 130].
      //
      // The fourth widget has a maximum size smaller than the provided minimum column
      // width. The logic will be the same as for the third widget, except that the
      // maximum size will constraint the application of the minimum column width and
      // thus the maximum reachable size will be [90, 90] (if the policy allows to grow).
      //
      // We handle these considerations before starting the optimization process as it
      // will directly impact it.
      adjustWidgetToConstraints(widgetsInfo);


      std::cout << "[LAY] Available size: " << window.w() << "x" << window.h() << std::endl;
      std::cout << "[LAY] Internal size: " << internalSize.w() << "x" << internalSize.h() << std::endl;

      for (unsigned row = 0u ; row < m_rows ; ++row) {
        std::cout << "[LAY] Row " << row << ": ";
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          float w = 0.0f;
          float h = 0.0f;

          for (LocationsMap::const_iterator widget = m_locations.cbegin() ;
                widget != m_locations.cend() ;
                ++widget)
          {
            if (widget->second.y == row &&
                widget->second.x == column)
            {
              w = cells[widget->first].box.w();
              h = cells[widget->first].box.h();
              break;
            }
          }

          std::cout << std::setw(7) << w << "x" << std::setw(7) << h;
          if (column < m_columns - 1) {
            std::cout << " ";
          }
        }
        std::cout << std::endl;
      }

      // We now have a working set of dimensions which we can begin to apply to widgets
      // in order to build the layout.
      // The specification of the dimensions is divided into two main phases: first the
      // determination of a base dimension for each column and row based on single-cell
      // widget, and then an adjustment phase where multi-cells widgets are placed.
      // In the first phase, the process is divided into two parts: the adjustment of
      // rows and columns is indeed completely independent and can be processed with no
      // particular ordering.
      // This allows for simpler algorithms.
      // In each case, the process is similar: we start by assuming an ideal size of the
      // widget for each column/row and try to work with this. In case some widget cannot
      // take full advantage of this size (because their constraints forbid it) we
      // allocate the remaining space between remaining widgets until the achieved size
      // is close enough from the target size.
      // If the widgets' constraints do not allow for a perfect repartition, we stop the
      // process and produce the best possible solution given the constraints.
      // Once all single-cell widgets have been laid out, we can try to further adjust
      // the resulting distribution to multi-cell widgets. At this point, the best case
      // scenario is that the widgets will take advantage of the computed repartition
      // and that no other adjustment will occur. This is rarely the case though and
      // we might have to redo an adjustment for single-cell widgets afterwards.

      // Proceed to adjust the columns' width.
      std::vector<float> columnsDims = adjustColumnsWidth(internalSize, widgetsInfo, cells);

      // Adjust rows' height.
      std::vector<float> rowsDims = adjustRowHeight(internalSize, widgetsInfo, cells);

      std::cout << "[LAY] Dims before multi-cell:" << std::endl;
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        std::cout << "[LAY] Row " << row << ": ";
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          std::cout << std::setw(4) << columnsDims[column] << "x" << std::setw(4) << rowsDims[row];
          if (column < m_columns - 1) {
            std::cout << " ";
          }
        }
        std::cout << std::endl;
      }

      // All widgets have suited dimensions, we can now handle the position of each
      // widget. We basically just move each widget based on the dimensions of the
      // rows and columns to reach the position of a specified widget.
      std::vector<utils::Boxf> outputBoxes(getItemsCount());

      for (unsigned index = 0u ; index < getItemsCount() ; ++index) {
        // Position the widget based on the dimensions of the rows and columns
        // until the position of the widget.
        // We maintained a vector to keep track of the dimensions of each row
        // and column during the adjustment process so that we can use it now
        // to assign a position to the boxes.
        // In addition to this mechanism, we should handle some kind of
        // centering to allow widgets with sizes smaller than the provided
        // layout's dimensions to still be nicely displayed in the center
        // of the layout.
        // To handle this case we check whether the dimensions of the size
        // of the widget is smaller than the dimension stored in `internalSize`
        // in which case we can center it.
        // The centering takes place according to both the dimensions of the
        // rows and columns spanned by the widget.

        // Retrieve the item's location.
        const LocationsMap::const_iterator loc = m_locations.find(index);
        if (loc == m_locations.end()) {
          error(
            std::string("Could not retrieve information for widget \"") +
            m_items[index]->getName() + "\" while updating grid layout"
          );
        }

        float xWidget = getMargin().w();
        float yWidget = getMargin().h();

        // Compute the offset to apply to reach the desired column based on the
        // item's location.
        for (unsigned column = 0u ; column < loc->second.x ; ++column) {
          xWidget += columnsDims[column];
        }
        for (unsigned row = 0u ; row < loc->second.y ; ++row) {
          yWidget += rowsDims[row];
        }

        // Center the position (because `Boxf` are centered).
        xWidget += (cells[index].box.w() / 2.0f);
        yWidget += (cells[index].box.h() / 2.0f);

        // Handle the centering of the widget in case it is smaller than the
        // desired width or height.
        // To do so, compute the size the widget _should_ have based on its
        // columns/rows span.
        float expectedWidth = 0.0f;
        float expectedHeight = 0.0f;

        for (unsigned column = loc->second.x ; column < loc->second.x + loc->second.w ; ++column) {
          expectedWidth += columnsDims[column];
        }
        for (unsigned row = loc->second.y ; row < loc->second.y + loc->second.h ; ++row) {
          expectedHeight += rowsDims[row];
        }

        if (cells[index].box.w() < expectedWidth) {
          xWidget += ((expectedWidth - cells[index].box.w()) / 2.0f);
        }
        if (cells[index].box.h() < expectedHeight) {
          yWidget += ((expectedHeight - cells[index].box.h()) / 2.0f);
        }

        outputBoxes[index] = utils::Boxf(
          xWidget, yWidget,
          cells[index].box.w(), cells[index].box.h()
        );
      }

      // Assign the rendering area to widgets.
      assignRenderingAreas(outputBoxes);
    }

    std::vector<GridLayout::CellInfo>
    GridLayout::computeCellsInfo() const noexcept {
      // Allocate vector where all cells are by default empty (no stretch,
      // no dimensions and no associated widget).
      // Note that we create as many cells as widgets, and not a single cell
      // per element of the global `m_columns * m_rows` cells defined by the
      // layout.
      // This does not allow exhaustive traversal of all the cells of the
      // layout but it allows for efficient mapping between a widget ID and
      // the corresponding cell information.
      std::vector<CellInfo> cells(
        getItemsCount(),
        CellInfo{
          0,
          0,
          utils::Boxf(),
          false,
          -1
        }
      );

      // Complete the information with widgets' data: if a cell contains
      // a widget, fill in the corresponding stretches and associate it
      // with the widget's identifier. Note that a cell is said to `contain`
      // a widget as long as it is in the area used by the widget. This
      // means that mult-cell widgets will be assigned several entries in
      // the vector produced by this function.

      // Traverse each widget's location information and update the relevant
      // information.
      for (LocationsMap::const_iterator item = m_locations.cbegin() ;
           item != m_locations.cend() ;
           ++item)
      {
        cells[item->first].hStretch = m_columnsInfo[item->second.x].stretch;
        cells[item->first].vStretch = m_rowsInfo[item->second.y].stretch;
        cells[item->first].box = utils::Boxf();
        cells[item->first].multiCell = (item->second.w > 1) || (item->second.h > 1);
        cells[item->first].widget = item->first;
      }

      // Return the built-in vector.
      return cells;
    }

    void
    GridLayout::adjustWidgetToConstraints(std::vector<WidgetInfo>& widgets) const noexcept {
      // The aim of this function is to provide for the minimum column width and minimum
      // row height adjustments needed to override the properties of the widgets.
      // This is not as simple as it is because we cannot just override the minimum size
      // of all widgets with the provided size based on their columns and rows: indeed
      // this might conflict with the size hint or maximum size provided.
      // We can only grow it as much as possible without overriding other attributes.

      // Traverse each widget and update the relevant constraints.
      for (unsigned widget = 0u ; widget < widgets.size() ; ++widget) {
        // Retrieve the location information for this widget.
        const LocationsMap::const_iterator info = m_locations.find(widget);
        if (info == m_locations.cend()) {
          error(
            std::string("Could not adjust widget ") + std::to_string(widget) + " to minimum constraints",
            std::string("Inexisting widget")
          );
        }

        const ItemInfo& loc = info->second;

        // Compute the minimum dimensions of this widget based on its location.
        utils::Sizef desiredMin;

        for (unsigned row = loc.y ; row < loc.y + loc.h ; ++row) {
          for (unsigned column = loc.x ; column < loc.x + loc.w ; ++column) {
            desiredMin.w() += m_columnsInfo[column].min;
          }
          desiredMin.h() += m_rowsInfo[row].min;
        }

        std::cout << "[LAY] Widget \"" << m_items[widget]->getName() << "\" "
                  << "at " << loc.x << "x" << loc.y
                  << " has min " << desiredMin.toString()
                  << " while internal are: "
                  << "min: " << widgets[widget].min.w() << "x" << widgets[widget].min.h() << " "
                  << "hint: " << widgets[widget].hint.w() << "x" << widgets[widget].hint.h() << " (valid: " << widgets[widget].hint.isValid() << ")"
                  << "max: " << widgets[widget].max.w() << "x" << widgets[widget].max.h()
                  << std::endl;

        // We computed the minimum size for this widget from the internal constraints. We have to
        // check whether this conflicts with some property of the widget. The best case scenario
        // is that this size is already smaller than the provided minimum size in which case there's
        // nothing to be done.
        // Otherwise, we want to check whether we can increase the minimum size of the widget
        // without breaking other constraints (like size hint or maximum size).
        // Whether we should consider the size hint (if provided) or the maximum size is based on the
        // policy for this widget: however we cannot really assume anything except to use the size
        // hint. Modifying this behavior would require to search for other widgets and check whether
        // some other widgets can expand. This does not even include considering whether the expanding
        // process would even occur in the first place because we have no idea of the total size of
        // the widgets. So let's keep this simple right now and just focus on whether the size hint
        // is provided or not.
        // As usual we will process separately width and height to keep it simple.
        utils::Sizef& min = widgets[widget].min;
        const utils::Sizef& hint = widgets[widget].hint;
        const utils::Sizef& max = widgets[widget].max;
        const core::SizePolicy& policy = widgets[widget].policy;

        // Assume we can assign the desired minimum width for this widget.
        float adjustedMinWidth = desiredMin.w();

        // Check whether it conflicts with the provided minimum size if any.
        // Basically we have the three following scenario:
        // 1) The min size is valid and:
        // 1.a) The min size is smaller than the desired size: keep the min size.
        // 1.b) The min size is equal to the desired size: keep the min size.
        // 1.c) The min size is larger than the desired size: keep the min size.
        // 2) The min size is not valid (no requirements) so we can keep the desired
        //    size.
        // We see that the only relevant check is to see whether a minimum size is
        // provided.
        if (min.isValid()) {
          adjustedMinWidth = min.w();
        }

        // Check whether the size hint conflicts with the desired minimum width.
        // These are the possible scenarii:
        // 1) The size hint is valid and:
        // 1.a) The size hint is smaller than the desired size: check with the policy to
        //      determine whether we can grow.
        // 1.b) The size hint is equal to the desired size: keep the desired size.
        // 1.c) The size hint is larger than the desired size: keep the size hint.
        // 2) The size hint is not valid (no requirements) so we can keep the desired
        //    size.
        if (hint.isValid()) {
          // Chewk whether the size hint is larger than the desired width.
          if (adjustedMinWidth < hint.w()) {
            // Override with the size hint.
            adjustedMinWidth = hint.w();
          }

          // The size hint is smaller than the desired size: check whether the policy allows
          // to grow.
          if (hint.w() < adjustedMinWidth && !policy.canExtendHorizontally()) {
            // Override with the size hint as the policy does not allow to grow.
            adjustedMinWidth = hint.w();
          }
        }

        // Check whether the maximum size conflicts with the desired minimum width.
        // At this point we already checked that the minimum size allowed for such a
        // width (otherwise it would have been clamped already). Same goes for the size
        // hint (which is right above) so we know that from these two constraints
        // standpoint the size is valid. The only remaining problem might be a size
        // too large compared to the maximum size.
        // Below are listed the possible scenarii:
        // 1) The max size is valid and:
        // 1.a) The max size is smaller than the desired size: keep the max size.
        // 1.b) The max size is equal to the desired size: keep the desired size.
        // 1.c) The max size is larger than the desired size: keep the desired size.
        // 2) The max size is not valid (no requirements) so we can keep the desired
        //    size.
        if (max.isValid() && max.w() < adjustedMinWidth) {
          // Override with the maximum size.
          adjustedMinWidth = max.w();
        }

        // Now proceed to adjustment for the height of the widget: similar reasoning can be
        // applied to width so we will not duplicate information here.
        float adjustedMinHeight = desiredMin.h();

        if (min.isValid()) {
          adjustedMinHeight = min.h();
        }

        if (hint.isValid()) {
          // Chewk whether the size hint is larger than the desired height.
          if (adjustedMinHeight < hint.h()) {
            // Override with the size hint.
            adjustedMinHeight = hint.h();
          }

          // The size hint is smaller than the desired size: check whether the policy
          // allows to grow.
          if (hint.h() < adjustedMinHeight && !policy.canExtendVertically()) {
            // Override with the size hint as the policy does not allow to grow.
            adjustedMinHeight = hint.h();
          }
        }

        // Check whether the maximum size conflicts with the desired minimum width.
        if (max.isValid() && max.h() < adjustedMinHeight) {
          adjustedMinHeight = max.h();
        }

        // Assign the new min size for this widget.
        min.w() = adjustedMinWidth;
        min.h() = adjustedMinHeight;
      }

    }

    utils::Sizef
    GridLayout::computeAchievedSize(const std::vector<WidgetData>& elements,
                                    const std::vector<CellInfo>& cells) const noexcept
    {
      // Assume empty dimensions.
      utils::Sizef achieved;

      // Traverse the input set of elements.
      for (unsigned element = 0u ; element < elements.size() ; ++element) {
        // Handle only single-cell widgets: multi-cell widgets will be handled
        // in a second phase when all the dimensions have been computed for
        // single-cell widgets. This allows for a better distribution of these
        // widgets over spanned columns/rows.
        if (elements[element].shared) {
          continue;
        }

        // We have a single-cell widget, handle its dimensions.
        const utils::Boxf box = cells[elements[element].widget].box;

        if (box.w() > achieved.w()) {
          achieved.w() = box.w();
        }
        if (box.h() > achieved.h()) {
          achieved.h() = box.h();
        }
      }

      // Return the achieved size.
      return achieved;
    }

    std::vector<float>
    GridLayout::adjustColumnsWidth(const utils::Sizef& window,
                                   const std::vector<WidgetInfo>& widgets,
                                   std::vector<CellInfo>& cells) const
    {
      // This method needs to combine several constraints in order to converge to final
      // columns' dimensions:
      // 1) The user might have specified `minimum column width` through the dedicated
      //    handler.
      // 2) Widgets might have policies describing minimum or maximum sizes.
      // 3) Several widgets in a single column might have conflicting expectations.
      //
      // The aim is thus to combine all these constraints in order to produce the final
      // columns' dimensions vector.
      // Note that only widget spanning a single column are considered in this function
      // so that we get simpler algorithms.
      // Multi-cells widgets will be considered in a second time and will only further
      // adapt the dimensions produced by this function.

      // Start by creating the return value: in addition to the individual width for
      // each widget this function return a global columns' dimensions vector where
      // the maximum width for each column is registered. This allows to easily
      // iterate over columns without needing to extract the largest widget in each
      // one.
      std::vector<float> columns(widgets.size(), 0.0f);

      // Now, we need to retrieve for each column the list of widgets related to it:
      // this allows for quick access when iterating to determine columns' dimensions.
      // We also want to provide easy access to multi-cell widget by inserting each
      // one of them in all the columns where it appear.
      // For that to happen we want to first initialize this map with empty vectors
      // and then dynamically populate it.

      std::unordered_map<unsigned, std::vector<WidgetData>> widgetsForColumns;
      for (unsigned column = 0u ; column < m_columns ; ++column) {
        widgetsForColumns[column] = std::vector<WidgetData>();
      }

      for (LocationsMap::const_iterator item = m_locations.cbegin() ;
            item != m_locations.cend() ;
            ++item)
      {
        // Insert the widget in each column spanned by it.
        const ItemInfo& info = item->second;

        for (unsigned column = 0u ; column < info.w ; ++column) {
          widgetsForColumns[info.x + column].push_back(
            WidgetData{
              item->first,
              info.w > 1,
              column == 0u,
              info.w,
              (info.y) * m_columns + info.x + column
            }
          );
        }
      }

      // There's a first part of the optimization process which should be handled
      // right away: the user is allowed to specify a minimum column width for any
      // column of the layout.
      // We can handle the columns where there's at least a widget later on (it's
      // actually part of the optimization process) but here we should handle the
      // columns which do not contain any widgets. As no widget is there to take
      // space such columns will be ignored by the optimization process.
      // In order to handle these nonetheless, we figured that the best way is to
      // simply subtract the required width from the available total `window`: this
      // makes sense as it would have been the width applied anyway because no
      // widget is there to modify it so it would have been accepted right away.

      float widthForEmptyColumns = 0.0f;

      // Traverse the `widgetsForColumns` map and search for columns with no widgets.
      for (std::unordered_map<unsigned, std::vector<WidgetData>>::const_iterator column = widgetsForColumns.cbegin() ;
           column != widgetsForColumns.cend() ;
           ++column)
      {
        // Search empty columns.
        if (column->second.empty()) {
          // Assign the minimum width to this column and retain the used space.
          columns[column->first] = m_columnsInfo[column->first].min;
          widthForEmptyColumns += columns[column->first];
        }
      }

      log(
        std::string("Total size: ") + std::to_string(window.w()) + ", used for empty columns: " + std::to_string(widthForEmptyColumns),
        utils::Level::Info
      );

      // Let's start the optimization process.
      // We start with an available space budget described by the value of `window.w()`.
      // We will first try to allocate fairly this space among all columns. Each column
      // will perform checks to determine whether the proposed dimension is valid based
      // on the criteria of each widget registered for this column.
      // If this is the case the dimension will be adopted. If not, the dimension will
      // be adapted so that it fits all the constraingts for this column.
      // Once all the columns have been processed, we start all over again: the remaining
      // or missing space is computed, and we then try to allocate the difference among
      // all the columns.
      // This process continues until either all the space has been successfully allocated
      // or there's no more columns to use to adjust the size.
      // In order to make things easier, we use a little trick here. Indeed even though
      // we're reasoning with columns here, the individual unit inside a column remains
      // a widget. Thus, when an adjustment should be performed for a column, it is
      // ultimately a widget which should be adjusted. Also, during the process columns
      // are interchangeable which means that any operation can and will be applied
      // indistinctly on all the columns available for adjustments.
      // For all these reasons, it makes more sense to keep a list of widgets to adjust
      // instead of a list of columns: we avoid an indirection to fetch the widget linked
      // to an identifier in a column.
      // We still keep the global `widgetsForColumn` which allows to cherry-pick the
      // single-cell widget for a given column, but the whole process is done in terms
      // of widgets.
      // A small drawbacks is that we need to know how many columns can still be used to
      // perform adjustment: indeed this is what defines how we will split fairly the
      // remanining space.

      std::unordered_set<WidgetData> widgetsToAdjust;
      unsigned columnsRemaining = widgetsForColumns.size();
      std::unordered_set<unsigned> emptyColumns;

      // In a first approach all the columns can be adjusted (except empty ones).
      for (unsigned column = 0u ; column < m_columns ; ++column) {
        if (widgetsForColumns[column].empty()) {
          emptyColumns.insert(column);
          --columnsRemaining;
        }
        else {
          widgetsToAdjust.insert(widgetsForColumns[column].cbegin(), widgetsForColumns[column].cend());
        }
      }

      // Also assume that we didn't use up all the available space. The remaining space is
      // the difference between the provided space from `window` minus the space used for
      // columns with no widgets (see two paragraphs above).
      float spaceToUse = window.w() - widthForEmptyColumns;
      bool allSpaceUsed = false;

      float achievedWidth = widthForEmptyColumns;

      // Loop until no more widgets can be used to adjust the space needed or all the
      // available space has been used up. As discussed, this is roughly equivalent to
      // checking whether a column is available for adjustment: we only avoid another
      // indirection to fetch the widget when performing the adjustments.
      // TODO: Handle cases where the widgets are too large to fit into the columns ?
      while (!widgetsToAdjust.empty() && !allSpaceUsed) {
      
        // Compute the amount of space we will try to allocate to each column still
        // available for adjustment.
        // The `defaultWidth` is computed by dividing equally the remaining `spaceToUse`
        // among all the available columns.
        // The available columns for adjustment is retrieved using the `columnsRemaining`
        // variable and not directly the `widgetsToAdjust.size()` as a single column can
        // have several widgets needing adjustments.
        const float defaultWidth = allocateFairly(spaceToUse, columnsRemaining);

        log(std::string("Default width is ") + std::to_string(defaultWidth), utils::Level::Info);

        // Allocate this space on each widget: as all columns are equivalent, this means
        // that we can directly work on individual widgets.
        for (std::unordered_set<WidgetData>::const_iterator data = widgetsToAdjust.cbegin() ;
             data != widgetsToAdjust.cend() ;
             ++data)
        {
          // Check whether this widget is a shared one: in this case we only need to process
          // it once, for the master instance.
          if (data->shared && !data->master) {
            // Do not process this instance, the `master` will be processed or has already been
            // processed (most likely).
            continue;
          }

          const unsigned widget = data->widget;

          // Retrieve the `ItemInfo` struct for this widget.
          const LocationsMap::const_iterator itemInfo = m_locations.find(widget);
          if (itemInfo == m_locations.cend()) {
            error(
              std::string("Could not retrieve information for widget \"") +
              m_items[widget]->getName() + "\" while updating grid layout"
            );
          }
          const ItemInfo& loc = itemInfo->second;

          // Try to assign the `defaultWidth` to this widget: we use a dedicated handler
          // to handle the case where the provided space is too large/small/not suited
          // to the widget for some reasons, in which case the handler will provide a
          // size which can be applied to the widget.
          // The process is not completely straightforward though as we need to account
          // for widgets spanning multiple cells. The `defaultWidth` should thus
          // be scaled to account for this.

          // Scale the `defaultWidth`.
          const float widgetWidth = defaultWidth * loc.w;

          // Apply the policy for this widget.
          float width = computeWidthFromPolicy(cells[widget].box, widgetWidth, widgets[widget]);
          cells[widget].box.w() = width;

          std::cout << "[LAY] Widget \"" << m_items[widget]->getName() << "\": "
                    << cells[widget].box.x() << ", " << cells[widget].box.y()
                    << ", dims: "
                    << cells[widget].box.w() << ", " << cells[widget].box.h()
                    << std::endl;
        }

        for (unsigned row = 0u ; row < m_rows ; ++row) {
          std::cout << "[LAY] Row " << row << ": ";
          for (unsigned column = 0u ; column < m_columns ; ++column) {
            float w = 0.0f;
            float h = 0.0f;

            for (LocationsMap::const_iterator widget = m_locations.cbegin() ;
                 widget != m_locations.cend() ;
                 ++widget)
            {
              if (widget->second.y == row &&
                  widget->second.x == column)
              {
                w = cells[widget->first].box.w();
                h = cells[widget->first].box.h();
                break;
              }
            }

            std::cout << std::setw(7) << w << "x" << std::setw(7) << h;
            if (column < m_columns - 1) {
              std::cout << " ";
            }
          }
          std::cout << std::endl;
        }

        // We have tried to apply the `defaultWidth` to all the remaining widgets available
        // for adjustments. This might have fail in some cases (for example due to a `Fixed`
        // size policy for a widget in a given column) and thus we might end up with a total
        // size for all the widget different from the one desired and expected when the
        // `defaultWidth` has been computed.
        // In order to fix things, we must compute the deviation from the expected size and
        // try to allocate the remaining space to other widgets (or remove the missing space
        // from widgets which can give up some).
        // For single-cell widget, it is rather easy: the achieved size only contributes to
        // a single column.
        // For multi-cell widgets it is a bit different though: we need to distribute the
        // width across several columns: but how ? The approach we chose is to first account
        // for the width available through single-cell widget and then add the remaining
        // width equally between all the spanned columns for each multi-cell widget.
        // We perform this operation after each optimization operation in order to guarantee
        // that the final value of the `columns` vector will be usable as a valid return value.

        // So first, compute achieved size for single-cell widgets.
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          // Only handle non empty columns.
          if (emptyColumns.find(column) == emptyColumns.cend()) {
            columns[column] = computeAchievedSize(widgetsForColumns[column], cells).w();
          }
        }

        // Consolidate the achieved size with multi-cell widgets.
        for (std::unordered_set<WidgetData>::const_iterator data = widgetsToAdjust.cbegin() ;
             data != widgetsToAdjust.cend() ;
             ++data)
        {
          // Check whether the widget is a multi-cell widget. Additionally we only want to
          // process the master's data.
          if (!data->shared || !data->master) {
            // Move on to the next widget.
            continue;
          }

          // Distribute the height of this widget over the available columns.
          distributeMultiBoxWidth(cells[data->widget], columns);
        }

        // Compute the achieved size from consolidated dimensions.
        achievedWidth = widthForEmptyColumns;
        for (unsigned column = 0u ; column < m_columns ; ++column) {
          achievedWidth += columns[column];
        }

        const utils::Sizef achievedSize(achievedWidth, window.h());

        // Check whether all the space have been used.
        if (achievedSize.fuzzyEqual(window, 1.0f)) {
          // We used up all the available space, no more adjustments to perform.
          allSpaceUsed = true;
          continue;
        }

        // All space has not been used. Update the relevant `spaceToUse` in order to perform
        // the next iteration.
        spaceToUse = computeSpaceAdjustmentNeeded(achievedSize, window).w();

        // Determine the policy to apply based on the achieved size.
        const sdl::core::SizePolicy action = shrinkOrGrow(window, achievedSize, 0.5f);

        log(std::string("Desired ") + window.toString() + ", achieved: " + std::to_string(achievedWidth) + ", space: " + std::to_string(spaceToUse), utils::Level::Info);

        // We now know what should be done to make the `achievedWidth` closer to `desiredWidth`.
        // Based on the `policy` provided by the base class method, we can now determine which
        // column should be used to perform the needed adjustments.
        // Even if in the end we will substitute the corresponding widgets to the individual
        // columns, right now we need to keep the columns.
        // This will prove useful in a second phase where we will try to make column which can
        // `Expand` have priority over column which can only `Grow`. This can only be done by
        // keeping the column's id: if we keep only widgets, we don't have any means (except
        // through exhaustive seach) to find the associated columns to a widget. Thus we will
        // not be easily able to discard entire columns if it can only `Grow`.
        std::unordered_set<unsigned> columnsToUse;

        // Traverse each column to determine whether a widget in this column can be used to
        // perform the required `action`.
        // Based on the `action`, the way we select columns is a bit different. If we need to
        // shrink some columns, we need all widgets inside a column to be shrinkable in order
        // for the column to be declared usable: indeed if only some widgets can be shrunk
        // it also means some cannot shrink and the overall size of the column will not be
        // modified even though the shrinkable widget are shrunk.
        // If the action is meant to gorw the situation is a bit different though: as we still
        // have the possibility to center widgets which are smaller than the total width of
        // the column, a column can be grown as soon as a single widget can be grown inside
        // it.
        // In order to allow for easy detection of columns which can be expanded, we also
        // keep the column's identifiers instead of substituting right away the corresponding
        // widgets. More details can be found in the next comment section.
        for (unsigned column = 0u ; column < widgetsForColumns.size() ; ++column) {
          // Retrieve the widgets associated to this column.
          const std::vector<WidgetData>& widgetsForColumn = widgetsForColumns[column];

          // Distinguish based on the action. Furhtermore we are processing columns so
          // we only care about horizontal behavior.
          if (action.canExtendHorizontally()) {
            // Traverse the widgets for this column: if at least one can be used to `Grow`,
            // consider this column usable to perform the required action.
            for (unsigned widget = 0u ; widget < widgetsForColumn.size() ; ++widget) {
              // Compute the status of the widget for this action.
              const unsigned widgetID = widgetsForColumn[widget].widget;

              std::pair<bool, bool> usable = canBeUsedTo(m_items[widgetID]->getName(), widgets[widgetID], cells[widgetID].box, action);
              if (usable.first) {
                // This column can be used to `Grow` thanks to this widget. No need to go
                // further.
                std::cout << "[LAY] Column " << column << " can be extented horizontally thanks to " << m_items[widgetID]->getName() << std::endl;
                columnsToUse.insert(column);
                break;
              }
            }
          }
          else if (action.canShrinkHorizontally()) {
            // Traverse the widgets for this column: all widgets need to be able to shrink
            // in order for this colum to be marked as shrinkable.
            
            // Assume this column can be shrunk.
            bool canShrink = true;

            for (unsigned widget = 0u ; widget < widgetsForColumn.size() ; ++widget) {
              // Compute the status of the widget for this action.
              const unsigned widgetID = widgetsForColumn[widget].widget;

              std::pair<bool, bool> usable = canBeUsedTo(m_items[widgetID]->getName(), widgets[widgetID], cells[widgetID].box, action);
              if (!usable.first) {
                // As this widget cannot shrink, it means that even if other widgets shrink
                // the width for this column will remain unchanged so we cannot use it.
                // No need to process other widgets.
                std::cout << "[LAY] Column " << column << " cannot be shrunk horizontally because of " << m_items[widgetID]->getName() << std::endl;
                // TODO: We might still want to use this column to shrink if its achieved size is larger than the size
                // required for this fixed-size widget.
                canShrink = false;
                break;
              }
            }

            // Register this column for shrinking if needed.
            if (canShrink) {
              columnsToUse.insert(column);
            }
          }
        }

        // We have a list of columns which can be used to perform the required `action`. There's
        // a last filtering to apply though: if the action requires to make some widgets larger, we need
        // to give priority to widgets which have the `Expand` flag over widgets having `Grow` flag.
        // The tricky part is that we're working on columns here so there's no simple way to assign a
        // global policy to a column.
        // In order to determine whether a column needs to `Expand`, we will check each individual widget
        // registered for this column and if at least one has the corresponding flag we will assume that
        // the column as a whole can be `Expand`ed.
        // Of course this only applies if the `action` includes growing at all.
        if (action.canExtendHorizontally()) {
          // Select only `Expanding` columns if any.
          std::unordered_set<unsigned> columnsToExpand;

          for (std::unordered_set<unsigned>::const_iterator column = columnsToUse.cbegin() ;
               column != columnsToUse.cend() ;
               ++column)
          {
            // Check whether this column can expand: this is done by checking each registered
            // widget in this column for the appropriate flag.
            const std::vector<WidgetData>& widgetsForColumn = widgetsForColumns[*column];

            for (unsigned widget = 0u ; widget < widgetsForColumn.size() ; ++widget) {
              // Only consider horizontal direction as we're processing columns.
              const unsigned widgetID = widgetsForColumn[widget].widget;

              if (widgets[widgetID].policy.canExpandHorizontally()) {
                std::cout << "[LAY] " << m_items[widgetID]->getName() << " can be expanded horizontally" << std::endl;
                columnsToExpand.insert(*column);
                // No need to continue further, the column can be `Expand`ed.
                break;
              }
            }
          }

          std::cout << "[LAY] Saved " << columnsToExpand.size() << " columns which can expand compared to "
                    << columnsToUse.size() << " which can extend"
                    << std::endl;
          // Check whether we could select at least one widget to expand: if this is not the
          // case we can proceed to extend the widget with only a `Grow` flag.
          if (!columnsToExpand.empty()) {
            columnsToUse.swap(columnsToExpand);
          }
        }

        // Update the remaining columns so that we can compute correctly the way to allocate space.
        columnsRemaining = columnsToUse.size();

        // We now have a working set of columns which can be used to perform the required `action` and we
        // took into consideration precedence of `Expand` flag over `Grow` flag. We now only need for each
        // column to substitute the corresponding widgets.
        std::unordered_set<WidgetData> widgetsToUse;

        for (std::unordered_set<unsigned>::const_iterator column = columnsToUse.cbegin() ;
             column != columnsToUse.cend() ;
             ++column)
        {
          widgetsToUse.insert(widgetsForColumns[*column].cbegin(), widgetsForColumns[*column].cend());
        }

        // Use the computed list of widgets to perform the next action in order
        // to reach the desired space.
        widgetsToAdjust.swap(widgetsToUse);
      }

      // Warn the user in case we could not use all the space.
      if (!allSpaceUsed) {
        log(
          std::string("Could only achieve width of ") + std::to_string(achievedWidth) +
          " but available space is " + std::to_string(window.w()),
          utils::Level::Warning
        );
      }

      // Return the consolidated columns' dimensions vector.
      return columns;
    }

    std::vector<float>
    GridLayout::adjustRowHeight(const utils::Sizef& window,
                                const std::vector<WidgetInfo>& widgets,
                                std::vector<CellInfo>& cells) const
    {
      // This method needs to combine several constraints in order to converge to final
      // rows' dimensions:
      // 1) The user might have specified `minimum row height` through the dedicated
      //    handler.
      // 2) Widgets might have policies describing minimum or maximum sizes.
      // 3) Several widgets in a single row might have conflicting expectations.
      //
      // The aim is thus to combine all these constraints in order to produce the final
      // rows' dimensions vector.
      // Note that only widget spanning a single row are considered in this function
      // so that we get simpler algorithms.
      // Multi-cells widgets will be considered in a second time and will only further
      // adapt the dimensions produced by this function.

      // Start by creating the return value: in addition to the individual height for
      // each widget this function return a global rows' dimensions vector where
      // the maximum height for each row is registered. This allows to easily
      // iterate over rows without needing to extract the largest widget in each
      // one.
      std::vector<float> rows(widgets.size(), 0.0f);

      // Now, we need to retrieve for each row the list of widgets related to it:
      // this allows for quick access when iterating to determine rows' dimensions.
      // We also want to provide easy access to multi-cell widget by inserting each
      // one of them in all the rows where it appear.
      // For that to happen we want to first initialize this map with empty vectors
      // and then dynamically populate it.

      std::unordered_map<unsigned, std::vector<WidgetData>> widgetsForRows;
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        widgetsForRows[row] = std::vector<WidgetData>();
      }

      for (LocationsMap::const_iterator item = m_locations.cbegin() ;
            item != m_locations.cend() ;
            ++item)
      {
        // Insert the widget in each row spanned by it.
        const ItemInfo& info = item->second;

        for (unsigned row = 0u ; row < info.h ; ++row) {
          widgetsForRows[info.y + row].push_back(
            WidgetData{
              item->first,
              info.h > 1,
              row == 0u,
              info.h,
              (info.y + row) * m_columns + info.x
            }
          );
        }
      }

      // There's a first part of the optimization process which should be handled
      // right away: the user is allowed to specify a minimum row height for any
      // row of the layout.
      // We can handle the rows where there's at least a widget later on (it's
      // actually part of the optimization process) but here we should handle the
      // rows which do not contain any widgets. As no widget is there to take
      // space such rows will be ignored by the optimization process.
      // In order to handle these nonetheless, we figured that the best way is to
      // simply subtract the required height from the available total `window`: this
      // makes sense as it would have been the height applied anyway because no
      // widget is there to modify it so it would have been accepted right away.

      float heightForEmptyRows = 0.0f;

      // Traverse the `widgetsForRows` map and search for rows with no widgets.
      for (std::unordered_map<unsigned, std::vector<WidgetData>>::const_iterator row = widgetsForRows.cbegin() ;
           row != widgetsForRows.cend() ;
           ++row)
      {
        // Search empty rows.
        if (row->second.empty()) {
          // Assign the minimum height to this row and retain the used space.
          rows[row->first] = m_rowsInfo[row->first].min;
          heightForEmptyRows += rows[row->first];
        }
      }

      log(
        std::string("Total size: ") + std::to_string(window.h()) + ", used for empty rows: " + std::to_string(heightForEmptyRows),
        utils::Level::Info
      );

      // Let's start the optimization process.
      // We start with an available space budget described by the value of `window.h()`.
      // We will first try to allocate fairly this space among all rows. Each row will
      // perform checks to determine whether the proposed dimension is valid based on
      // the criteria of each widget registered for this row.
      // If this is the case the dimension will be adopted. If not, the dimension will
      // be adapted so that it fits all the constraingts for this row.
      // Once all the rows have been processed, we start all over again: the remaining
      // or missing space is computed, and we then try to allocate the difference among
      // all the rows.
      // This process continues until either all the space has been successfully allocated
      // or there's no more rows to use to adjust the size.
      // In order to make things easier, we use a little trick here. Indeed even though
      // we're reasoning with rows here, the individual unit inside a row remains
      // a widget. Thus, when an adjustment should be performed for a row, it is
      // ultimately a widget which should be adjusted. Also, during the process rows
      // are interchangeable which means that any operation can and will be applied
      // indistinctly on all the rows available for adjustments.
      // For all these reasons, it makes more sense to keep a list of widgets to adjust
      // instead of a list of rows: we avoid an indirection to fetch the widget linked
      // to an identifier in a row.
      // We still keep the global `widgetsForRow` which allows to cherry-pick the
      // single-cell widget for a given row, but the whole process is done in terms
      // of widgets.
      // A small drawbacks is that we need to know how many rows can still be used to
      // perform adjustment: indeed this is what defines how we will split fairly the
      // remanining space.

      std::unordered_set<WidgetData> widgetsToAdjust;
      unsigned rowsRemaining = widgetsForRows.size();
      std::unordered_set<unsigned> emptyRows;

      // In a first approach all the rows can be adjusted (except empty ones).
      for (unsigned row = 0u ; row < m_rows ; ++row) {
        if (widgetsForRows[row].empty()) {
          emptyRows.insert(row);
          --rowsRemaining;
        }
        else {
          widgetsToAdjust.insert(widgetsForRows[row].cbegin(), widgetsForRows[row].cend());
        }
      }

      // Also assume that we didn't use up all the available space. The remaining space is
      // the difference between the provided space from `window` minus the space used for
      // rows with no widgets (see two paragraphs above).
      float spaceToUse = window.h() - heightForEmptyRows;
      bool allSpaceUsed = false;

      float achievedHeight = heightForEmptyRows;

      // Loop until no more widgets can be used to adjust the space needed or all the
      // available space has been used up. As discussed, this is roughly equivalent to
      // checking whether a row is available for adjustment: we only avoid another
      // indirection to fetch the widget when performing the adjustments.
      // TODO: Handle cases where the widgets are too large to fit into the rows ?
      while (!widgetsToAdjust.empty() && !allSpaceUsed) {
      
        // Compute the amount of space we will try to allocate to each row still
        // available for adjustment.
        // The `defaultHeight` is computed by dividing equally the remaining `spaceToUse`
        // among all the available rows.
        // The available rows for adjustment is retrieved using the `rowsRemaining`
        // variable and not directly the `widgetsToAdjust.size()` as a single row can
        // have several widgets needing adjustments.
        const float defaultHeight = allocateFairly(spaceToUse, rowsRemaining);

        log(std::string("Default height is ") + std::to_string(defaultHeight), utils::Level::Info);

        // Allocate this space on each widget: as all rows are equivalent, this means
        // that we can directly work on individual widgets.
        for (std::unordered_set<WidgetData>::const_iterator data = widgetsToAdjust.cbegin() ;
             data != widgetsToAdjust.cend() ;
             ++data)
        {
          // Check whether this widget is a shared one: in this case we only need to process
          // it once, for the master instance.
          if (data->shared && !data->master) {
            // Do not process this instance, the `master` will be processed or has already been
            // processed (most likely).
            continue;
          }

          const unsigned widget = data->widget;

          // Retrieve the `ItemInfo` struct for this widget.
          const LocationsMap::const_iterator itemInfo = m_locations.find(widget);
          if (itemInfo == m_locations.cend()) {
            error(
              std::string("Could not retrieve information for widget \"") +
              m_items[widget]->getName() + "\" while updating grid layout"
            );
          }
          const ItemInfo& loc = itemInfo->second;

          // Try to assign the `defaultHeight` to this widget: we use a dedicated handler
          // to handle the case where the provided space is too large/small/not suited
          // to the widget for some reasons, in which case the handler will provide a
          // size which can be applied to the widget.
          // The process is not completely straightforward though as we need to account
          // for widgets spanning multiple cells. The `defaultHeight` should thus
          // be scaled to account for this.

          // Scale the `defaultHeight`.
          const float widgetHeight = defaultHeight * loc.h;

          // Apply the policy for this widget.
          float height = computeHeightFromPolicy(cells[widget].box, widgetHeight, widgets[widget]);
          cells[widget].box.h() = height;

          std::cout << "[LAY] Widget \"" << m_items[widget]->getName() << "\": "
                    << cells[widget].box.x() << ", " << cells[widget].box.y()
                    << ", dims: "
                    << cells[widget].box.w() << ", " << cells[widget].box.h()
                    << std::endl;
        }

        for (unsigned row = 0u ; row < m_rows ; ++row) {
          std::cout << "[LAY] Row " << row << ": ";
          for (unsigned column = 0u ; column < m_columns ; ++column) {
            float w = 0.0f;
            float h = 0.0f;

            for (LocationsMap::const_iterator widget = m_locations.cbegin() ;
                 widget != m_locations.cend() ;
                 ++widget)
            {
              if (widget->second.y == row &&
                  widget->second.x == column)
              {
                w = cells[widget->first].box.w();
                h = cells[widget->first].box.h();
                break;
              }
            }

            std::cout << std::setw(7) << w << "x" << std::setw(7) << h;
            if (column < m_columns - 1) {
              std::cout << " ";
            }
          }
          std::cout << std::endl;
        }

        // We have tried to apply the `defaultHeight` to all the remaining widgets available
        // for adjustments. This might have fail in some cases (for example due to a `Fixed`
        // size policy for a widget in a given row) and thus we might end up with a total
        // size for all the widget different from the one desired and expected when the
        // `defaultHeight` has been computed.
        // In order to fix things, we must compute the deviation from the expected size and
        // try to allocate the remaining space to other widgets (or remove the missing space
        // from widgets which can give up some).
        // For single-cell widget, it is rather easy: the achieved size only contributes to
        // a single row.
        // For multi-cell widgets it is a bit different though: we need to distribute the
        // height across several rows: but how ? The approach we chose is to first account
        // for the height available through single-cell widget and then add the remaining
        // height equally between all the spanned rows for each multi-cell widget.
        // We perform this operation after each optimization operation in order to guarantee
        // that the final value of the `rows` vector will be usable as a valid return value.

        // So first, compute achieved size for single-cell widgets.
        for (unsigned row = 0u ; row < m_rows ; ++row) {
          // Only handle non empty rows.
          if (emptyRows.find(row) == emptyRows.cend()) {
            rows[row] = computeAchievedSize(widgetsForRows[row], cells).h();
          }
        }

        // Consolidate the achieved size with multi-cell widgets.
        for (std::unordered_set<WidgetData>::const_iterator data = widgetsToAdjust.cbegin() ;
             data != widgetsToAdjust.cend() ;
             ++data)
        {
          // Check whether the widget is a multi-cell widget. Additionally we only want to
          // process the master's data.
          if (!data->shared || !data->master) {
            // Move on to the next widget.
            continue;
          }

          // Distribute the height of this widget over the available rows.
          distributeMultiBoxHeight(cells[data->widget], rows);
        }

        // Compute the achieved size from consolidated dimensions.
        achievedHeight = heightForEmptyRows;
        for (unsigned row = 0u ; row < m_rows ; ++row) {
          achievedHeight += rows[row];
        }

        const utils::Sizef achievedSize(window.w(), achievedHeight);

        // Check whether all the space have been used.
        if (achievedSize.fuzzyEqual(window, 1.0f)) {
          // We used up all the available space, no more adjustments to perform.
          allSpaceUsed = true;
          continue;
        }

        // All space has not been used. Update the relevant `spaceToUse` in order to perform
        // the next iteration.
        spaceToUse = computeSpaceAdjustmentNeeded(achievedSize, window).h();

        // Determine the policy to apply based on the achieved size.
        const sdl::core::SizePolicy action = shrinkOrGrow(window, achievedSize, 0.5f);

        log(std::string("Desired ") + window.toString() + ", achieved: " + std::to_string(achievedHeight) + ", space: " + std::to_string(spaceToUse), utils::Level::Info);

        // We now know what should be done to make the `achievedHeight` closer to `desiredHeight`.
        // Based on the `policy` provided by the base class method, we can now determine which
        // row should be used to perform the needed adjustments.
        // Even if in the end we will substitute the corresponding widgets to the individual
        // rows, right now we need to keep the rows.
        // This will prove useful in a second phase where we will try to make row which can
        // `Expand` have priority over row which can only `Grow`. This can only be done by
        // keeping the row's id: if we keep only widgets, we don't have any means (except
        // through exhaustive seach) to find the associated rows to a widget. Thus we will
        // not be easily able to discard entire rows if it can only `Grow`.
        std::unordered_set<unsigned> rowsToUse;

        // Traverse each row to determine whether a widget in this row can be used to
        // perform the required `action`.
        // Based on the `action`, the way we select rows is a bit different. If we need to
        // shrink some rows, we need all widgets inside a row to be shrinkable in order
        // for the row to be declared usable: indeed if only some widgets can be shrunk
        // it also means some cannot shrink and the overall size of the row will not be
        // modified even though the shrinkable widget are shrunk.
        // If the action is meant to gorw the situation is a bit different though: as we still
        // have the possibility to center widgets which are smaller than the total height of
        // the row, a row can be grown as soon as a single widget can be grown inside
        // it.
        // In order to allow for easy detection of rows which can be expanded, we also
        // keep the row's identifiers instead of substituting right away the corresponding
        // widgets. More details can be found in the next comment section.
        for (unsigned row = 0u ; row < widgetsForRows.size() ; ++row) {
          // Retrieve the widgets associated to this row.
          const std::vector<WidgetData>& widgetsForRow = widgetsForRows[row];

          // Distinguish based on the action. Furhtermore we are processing rows so
          // we only care about vertical behavior.
          if (action.canExtendVertically()) {
            // Traverse the widgets for this row: if at least one can be used to `Grow`,
            // consider this row usable to perform the required action.
            for (unsigned widget = 0u ; widget < widgetsForRow.size() ; ++widget) {
              // Compute the status of the widget for this action.
              const unsigned widgetID = widgetsForRow[widget].widget;

              std::pair<bool, bool> usable = canBeUsedTo(m_items[widgetID]->getName(), widgets[widgetID], cells[widgetID].box, action);
              if (usable.second) {
                // This row can be used to `Grow` thanks to this widget. No need to go
                // further.
                std::cout << "[LAY] Row " << row << " can be extented vertically thanks to " << m_items[widgetID]->getName() << std::endl;
                rowsToUse.insert(row);
                break;
              }
            }
          }
          else if (action.canShrinkVertically()) {
            // Traverse the widgets for this row: all widgets need to be able to shrink
            // in order for this colum to be marked as shrinkable.
            
            // Assume this row can be shrunk.
            bool canShrink = true;

            for (unsigned widget = 0u ; widget < widgetsForRow.size() ; ++widget) {
              // Compute the status of the widget for this action.
              const unsigned widgetID = widgetsForRow[widget].widget;

              std::pair<bool, bool> usable = canBeUsedTo(m_items[widgetID]->getName(), widgets[widgetID], cells[widgetID].box, action);
              if (!usable.second) {
                // As this widget cannot shrink, it means that even if other widgets shrink
                // the height for this row will remain unchanged so we cannot use it.
                // No need to process other widgets.
                std::cout << "[LAY] Row " << row << " cannot be shrunk vertically because of " << m_items[widgetID]->getName() << std::endl;
                canShrink = false;
                break;
              }
            }

            // Register this row for shrinking if needed.
            if (canShrink) {
              rowsToUse.insert(row);
            }
          }
        }

        // We have a list of rows which can be used to perform the required `action`. There's
        // a last filtering to apply though: if the action requires to make some widgets larger, we need
        // to give priority to widgets which have the `Expand` flag over widgets having `Grow` flag.
        // The tricky part is that we're working on rows here so there's no simple way to assign a
        // global policy to a row.
        // In order to determine whether a row needs to `Expand`, we will check each individual widget
        // registered for this row and if at least one has the corresponding flag we will assume that
        // the row as a whole can be `Expand`ed.
        // Of course this only applies if the `action` includes growing at all.
        if (action.canExtendVertically()) {
          // Select only `Expanding` rows if any.
          std::unordered_set<unsigned> rowsToExpand;

          for (std::unordered_set<unsigned>::const_iterator row = rowsToUse.cbegin() ;
               row != rowsToUse.cend() ;
               ++row)
          {
            // Check whether this row can expand: this is done by checking each registered
            // widget in this row for the appropriate flag.
            const std::vector<WidgetData>& widgetsForRow = widgetsForRows[*row];

            for (unsigned widget = 0u ; widget < widgetsForRow.size() ; ++widget) {
              // Only consider horizontal direction as we're processing rows.
              const unsigned widgetID = widgetsForRow[widget].widget;

              if (widgets[widgetID].policy.canExpandVertically()) {
                std::cout << "[LAY] " << m_items[widgetID]->getName() << " can be expanded vertically" << std::endl;
                rowsToExpand.insert(*row);
                // No need to continue further, the row can be `Expand`ed.
                break;
              }
            }
          }

          std::cout << "[LAY] Saved " << rowsToExpand.size() << " rows which can expand compared to "
                    << rowsToUse.size() << " which can extend"
                    << std::endl;
          // Check whether we could select at least one widget to expand: if this is not the
          // case we can proceed to extend the widget with only a `Grow` flag.
          if (!rowsToExpand.empty()) {
            rowsToUse.swap(rowsToExpand);
          }
        }

        // Update the remaining rows so that we can compute correctly the way to allocate space.
        rowsRemaining = rowsToUse.size();

        // We now have a working set of rows which can be used to perform the required `action` and we
        // took into consideration precedence of `Expand` flag over `Grow` flag. We now only need for each
        // row to substitute the corresponding widgets.
        std::unordered_set<WidgetData> widgetsToUse;

        for (std::unordered_set<unsigned>::const_iterator row = rowsToUse.cbegin() ;
             row != rowsToUse.cend() ;
             ++row)
        {
          widgetsToUse.insert(widgetsForRows[*row].cbegin(), widgetsForRows[*row].cend());
        }

        // Use the computed list of widgets to perform the next action in order
        // to reach the desired space.
        widgetsToAdjust.swap(widgetsToUse);
      }

      // Warn the user in case we could not use all the space.
      if (!allSpaceUsed) {
        log(
          std::string("Could only achieve height of ") + std::to_string(achievedHeight) +
          " but available space is " + std::to_string(window.h()),
          utils::Level::Warning
        );
      }

      // Return the consolidated rows' dimensions vector.
      return rows;
    }

    void
    GridLayout::distributeMultiBoxHeight(const CellInfo& cell,
                                         std::vector<float>& rows) const
    {
      // We need to distribute the height of the input `cell` over the concerned
      // rows. In order to do so, we first compute the height already handled by
      // the current height of all the rows spanned by the widget, and then distribute
      // the remaining height (if any) equally between rows.

      // Retrieve the widget's location.
      LocationsMap::const_iterator locIt = m_locations.find(cell.widget);
      if (locIt == m_locations.cend()) {
        error(
          std::string("Could not retrieve information for widget \"") +
          m_items[cell.widget]->getName() + "\" while updating grid layout"
        );
      }
      const ItemInfo& loc = locIt->second;

      // First compute the height already covered by the input rows.
      float existingHeight = 0.0f;

      for (unsigned row = 0u ; row < loc.h ; ++row) {
        existingHeight += rows[loc.y + row];
      }

      // Check whether the existing height is enough to absorb the cell's height.
      if (cell.box.h() < existingHeight) {
        // All is well: the widget does not span all the height provided by the rows
        // into which it lies. Let the standard process continue.
        return;
      }

      // The height is not enough to absorb the widget's height. We need to redistribute
      // the additional pixels over all the rows spanned by the widget based on a fair
      // distribution.

      // Compute the height to add to each row.
      const float remainingHeight = (cell.box.h() - existingHeight) / loc.h;

      // Distribute the height over each row spanned by the widget.
      for (unsigned row = 0u ; row < loc.h ; ++row) {
        rows[loc.y + row] += remainingHeight;
      }
    }

    void
    GridLayout::distributeMultiBoxWidth(const CellInfo& cell,
                                        std::vector<float>& columns) const
    {
      // We need to distribute the width of the input `cell` over the concerned
      // columns. In order to do so, we first compute the width already handled by
      // the current width of all the columns spanned by the widget, and then
      // distribute the remaining width (if any) equally between columns.

      // Retrieve the widget's location.
      LocationsMap::const_iterator locIt = m_locations.find(cell.widget);
      if (locIt == m_locations.cend()) {
        error(
          std::string("Could not retrieve information for widget \"") +
          m_items[cell.widget]->getName() + "\" while updating grid layout"
        );
      }
      const ItemInfo& loc = locIt->second;

      // First compute the width already covered by the input columns.
      float existingWidth = 0.0f;

      for (unsigned column = 0u ; column < loc.w ; ++column) {
        existingWidth += columns[loc.x + column];
      }

      // Check whether the existing width is enough to absorb the cell's width.
      if (cell.box.w() < existingWidth) {
        // All is well: the widget does not span all the width provided by the
        // columns into which it lies. Let the standard process continue.
        return;
      }

      // The width is not enough to absorb the widget's width. We need to redistribute
      // the additional pixels over all the columns spanned by the widget based on a fair
      // distribution.

      // Compute the width to add to each column.
      const float remainingWidth = (cell.box.w() - existingWidth) / loc.w;

      // Distribute the width over each column spanned by the widget.
      for (unsigned column = 0u ; column < loc.w ; ++column) {
        columns[loc.x + column] += remainingWidth;
      }
    }

  }
}
