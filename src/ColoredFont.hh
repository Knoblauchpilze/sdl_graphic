#ifndef    COLOREDFONT_HH
# define   COLOREDFONT_HH

# include <memory>
# include "Font.hh"

namespace sdl {
  namespace graphic {

    class ColoredFont {
      public:

        explicit
        ColoredFont(FontShPtr font, const SDL_Color& color);

        virtual ~ColoredFont();

        FontShPtr
        getFont() const noexcept;

        const SDL_Color&
        getColor() const noexcept;

        void
        setSize(const int& size) noexcept;

        void
        setColor(const SDL_Color& color) noexcept;

        SDL_Texture*
        render(SDL_Renderer* renderer, const std::string& text);

      private:

        FontShPtr m_font;
        SDL_Color m_color;
        bool m_dirty;

        SDL_Texture* m_text;

    };

    using ColoredFontShPtr = std::shared_ptr<ColoredFont>;
  }
}

# include "ColoredFont.hxx"

#endif    /* COLOREDFONT_HH */
