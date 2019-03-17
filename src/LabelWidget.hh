#ifndef    LABELWIDGET_HH
# define   LABELWIDGET_HH

# include <memory>
# include <string>
# include <sdl_core/SdlWidget.hh>
# include <sdl_engine/ColoredFont.hh>

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
                    core::engine::ColoredFontShPtr font,
                    const HorizontalAlignment& hAlignment = HorizontalAlignment::Center,
                    const VerticalAlignment& vAlignment = VerticalAlignment::Center,
                    SdlWidget* parent = nullptr,
                    const bool transparent = false,
                    const core::engine::Palette& palette = core::engine::Palette(),
                    const utils::Sizef& area = utils::Sizef());

        virtual ~LabelWidget();

        void
        setText(const std::string& text) noexcept;

        void
        setFont(core::engine::ColoredFontShPtr font) noexcept;

        void
        setHorizontalAlignment(const HorizontalAlignment& alignment) noexcept;

        void
        setVerticalAlignment(const VerticalAlignment& alignment) noexcept;

        void
        onKeyReleasedEvent(const SDL_KeyboardEvent& keyEvent) override;

      protected:

        void
        drawContentPrivate(const core::engine::Texture::UUID& uuid) const noexcept override;

      private:

        void
        loadText() const;

      private:

        std::string m_text;
        core::engine::ColoredFontShPtr m_font;
        HorizontalAlignment m_hAlignment;
        VerticalAlignment m_vAlignment;
        mutable bool m_textDirty;

        mutable std::shared_ptr<core::engine::Texture::UUID> m_label;

    };

    using LabelWidgetShPtr = std::shared_ptr<LabelWidget>;
  }
}

# include "LabelWidget.hxx"

#endif    /* LABELWIDGET_HH */
