#ifndef    FONTFACTORY_HH
# define   FONTFACTORY_HH

# include <unordered_map>
# include "Font.hh"
# include "ColoredFont.hh"

namespace sdl {
  namespace graphic {

    class FontFactory {
      public:

        FontFactory(const FontFactory& other) = delete;

        void operator=(const FontFactory& other) = delete;

        static
        FontFactory&
        getInstance();

        void
        releaseFonts();

        FontShPtr
        createFont(const std::string& name, const int size = 25);

        ColoredFontShPtr
        createColoredFont(const std::string& name,
                          const SDL_Color& color = SDL_Color{255, 255, 255, SDL_ALPHA_OPAQUE},
                          const int size = 25);

      private:

        FontFactory() = default;

        ~FontFactory() = default;

      private:

        std::unordered_map<std::string, FontShPtr> m_fonts;

    };

  }
}

# include "FontFactory.hxx"

#endif    /* FONTFACTORY_HH */
