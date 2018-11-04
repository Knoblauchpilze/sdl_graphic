#ifndef    FONTFACTORY_HXX
# define   FONTFACTORY_HXX

# include "FontFactory.hh"

namespace sdl {
  namespace graphic {

    FontFactory& FontFactory::getInstance() {
      static FontFactory instance;
      return instance;
    }

    void FontFactory::releaseFonts() {
      m_fonts.clear();
    }

    inline
    FontShPtr
    FontFactory::createFont(const std::string& name, const int size) {
      // Try to find this couple.
      std::unordered_map<std::string, FontShPtr>::const_iterator font = m_fonts.find(name);
      if (font != m_fonts.cend()) {
        return font->second;
      }

      m_fonts[name] = std::make_shared<Font>(name, size);

      return m_fonts[name];
    }

    inline
    ColoredFontShPtr
    FontFactory::createColoredFont(const std::string& name,
                                   const SDL_Color& color,
                                   const int size)
    {
      return std::make_shared<ColoredFont>(createFont(name, size), color);
    }

  }
}

#endif    /* FONTFACTORY_HXX */
