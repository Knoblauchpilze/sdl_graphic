#ifndef    LABELWIDGET_HH
# define   LABELWIDGET_HH

# include <memory>
# include <string>
# include <core_utils/Uuid.hh>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class LabelWidget: public sdl::core::SdlWidget {
      public:

        enum class VerticalAlignment {
          Top,
          Center,
          Bottom
        };

        enum class HorizontalAlignment {
          Left,
          Center,
          Right
        };

      public:

        LabelWidget(const std::string& name,
                    const std::string& text,
                    const std::string& font,
                    const unsigned& size = 15,
                    const HorizontalAlignment& hAlignment = HorizontalAlignment::Center,
                    const VerticalAlignment& vAlignment = VerticalAlignment::Center,
                    SdlWidget* parent = nullptr,
                    const core::engine::Color& color = core::engine::Color(),
                    const utils::Sizef& area = utils::Sizef());

        virtual ~LabelWidget();

        void
        setText(const std::string& text) noexcept;

        void
        setHorizontalAlignment(const HorizontalAlignment& alignment) noexcept;

        void
        setVerticalAlignment(const VerticalAlignment& alignment) noexcept;

      protected:

        void
        drawContentPrivate(const utils::Uuid& uuid) const override;

      private:

        void
        loadText() const;

      private:

        std::string m_text;
        std::string m_fontName;
        unsigned m_fontSize;
        mutable utils::Uuid m_font;
        HorizontalAlignment m_hAlignment;
        VerticalAlignment m_vAlignment;
        mutable bool m_textDirty;

        mutable utils::Uuid m_label;

    };

    using LabelWidgetShPtr = std::shared_ptr<LabelWidget>;
  }
}

# include "LabelWidget.hxx"

#endif    /* LABELWIDGET_HH */
