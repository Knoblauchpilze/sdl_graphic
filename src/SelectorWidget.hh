#ifndef    SELECTOR_WIDGET_HH
# define   SELECTOR_WIDGET_HH

# include <memory>
# include <string>
# include <vector>
# include <unordered_map>
# include <sdl_core/SdlWidget.hh>
# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    class SelectorWidget: public core::SdlWidget {
      public:

        SelectorWidget(const std::string& name,
                       core::SdlWidget* parent = nullptr,
                       const bool switchOnLeftClick = false,
                       const core::engine::Color& color = core::engine::Color(),
                       const utils::Sizef& area = utils::Sizef());

        ~SelectorWidget() = default;

        void
        setActiveWidget(const std::string& name);

        void
        setActiveWidget(const int& index);

        /**
         * @brief - Used to switch to the next item in the layout and activate it.
         *          According to the order into which the items have been inserted
         *          in this widget this method will activate the next available
         *          item in the list.
         *          If no item is currently selected the first one will be selected
         *          and if no more items can be found, it will loop over to the first
         *          item.
         */
        void
        switchToNext();

        /**
         * @brief - Used to insert the input `widget` at the specified `index` in the
         *          internal layout. Raises an error if the insertion cannot be performed
         *          for some reason.
         * @param widget - the widget to insert, should not be null.
         * @param index - the position at which the widget should be inserted, might be
         *                negative (in which case the widget will be inserted in first
         *                position in the layout) or larger than the actual widgets' count
         *                (in which case the widget will be inserted in last position).
         */
        void
        insertWidget(core::SdlWidget* widget,
                     const int& index);

      protected:

        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the `addWidget` method of the base `SdlWidget` class.
         *          This allows to provide a specific behavior when a widget is added as a
         *          child of this widget: we need to insert it into the associated layout so
         *          that we can keep the encapsulation and not expose specific methods to
         *          insert a widget in this object.
         *          By specializing this method we allow for simple use of a `SelectorWidget`
         *          and easy insertion of elements inside it: we just have to use the regular
         *          way of parenting each child widget and it will automatically be added to
         *          the layout.
         * @param widget - the widget which should be assigned `this` as parent: it will be
         *                 inserted in the internal layout during the process.
         */
        void
        addWidget(core::SdlWidget* widget) override;

      private:

        bool
        switchOnClick() const noexcept;

        SelectorLayout&
        getLayout();

      private:

        bool m_switchOnLeftClick;

    };

    using SelectorWidgetShPtr = std::shared_ptr<SelectorWidget>;
  }
}

# include "SelectorWidget.hxx"

#endif    /* SELECTOR_WIDGET_HH */
