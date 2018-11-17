#ifndef    LABELWIDGET_HH
# define   LABELWIDGET_HH

# include <memory>
# include <string>
# include <sdl_core/SdlWidget.hh>
# include "ColoredFont.hh"

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
                    ColoredFontShPtr font,
                    const HorizontalAlignment& hAlignment = HorizontalAlignment::Center,
                    const VerticalAlignment& vAlignment = VerticalAlignment::Center,
                    SdlWidget* parent = nullptr,
                    const SDL_Color& backgroundColor = SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE});

        virtual ~LabelWidget();

        void
        setText(const std::string& text) noexcept;

        void
        setFont(ColoredFontShPtr font) noexcept;

        void
        setHorizontalAlignment(const HorizontalAlignment& alignment) noexcept;

        void
        setVerticalAlignment(const VerticalAlignment& alignment) noexcept;

      protected:

        void
        drawContentPrivate(SDL_Renderer* renderer, SDL_Texture* texture) const noexcept override;

      private:

        void
        createText(SDL_Renderer* renderer) const;

      private:

        std::string m_text;
        ColoredFontShPtr m_font;
        HorizontalAlignment m_hAlignment;
        VerticalAlignment m_vAlignment;
        mutable bool m_textDirty;

        mutable SDL_Texture* m_label;

    };

    using LabelWidgetShPtr = std::shared_ptr<LabelWidget>;
  }
}

# include "LabelWidget.hxx"

#endif    /* LABELWIDGET_HH */
