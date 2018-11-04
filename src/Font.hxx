#ifndef    FONT_HXX
# define   FONT_HXX

# include <algorithm>
# include "Font.hh"
# include "FontException.hh"

namespace sdl {
  namespace graphic {

    inline
    Font::~Font() {
      unloadAll();
    }

    inline
    const std::string&
    Font::getName() const noexcept {
      return m_name;
    }

    inline
    const int&
    Font::getSize() const noexcept {
      return m_size;
    }

    inline
    void
    Font::setSize(const int& size) noexcept {
      unload();
      m_size = std::max(size, 1);
    }

    inline
    SDL_Surface*
    Font::render(const std::string& text, const SDL_Color& color) {
      // Load the font if needed.
      load();

      // Proceed to rendering.
      SDL_Surface* textSurface = TTF_RenderUTF8_Blended(m_font, text.c_str(), color);
      if (textSurface == nullptr) {
        throw FontException(std::string("Could not render text ") + text + " with font \"" + getName() + "\"");
      }

      return textSurface;
    }

    inline
    bool
    Font::loaded() const noexcept {
      return m_font != nullptr;
    }

    inline
    void
    Font::load() {
      if (!loaded()) {
        // Check for already loaded font.
        const std::unordered_map<int, TTF_Font*>::const_iterator font = m_fonts.find(getSize());
        if (font != m_fonts.cend()) {
          m_font = font->second;
          return;
        }

        // The font is not loaded yet.
        TTF_Font* newFont = TTF_OpenFont(m_name.c_str(), getSize());

        // Check that we could effectively load the font.
        if (newFont == nullptr) {
          throw FontException(
            std::string("Unable to create font \"") + getName() + "\": " + TTF_GetError()
          );
        }

        m_fonts[getSize()] = newFont;
        m_font = newFont;
      }
    }

    inline
    void
    Font::unload() {
      m_font = nullptr;
    }

    inline
    void
    Font::unloadAll() {
      for (std::unordered_map<int, TTF_Font*>::iterator font = m_fonts.begin() ; font != m_fonts.end() ; ++font) {
        if (font->second != nullptr) {
          TTF_CloseFont(font->second);
        }
      }
      m_fonts.clear();
    }

    inline
    void
    Font::initializeTTFLib() {
      // Initialize the ttf lib.
      if (!TTF_WasInit() && TTF_Init() == -1) {
        throw FontException(
          std::string("Unable to initialize TTF lib, cannot create font \"") + getName() + "\": " + TTF_GetError()
        );
      }
    }

  }
}

#endif    /* FONT_HXX */
