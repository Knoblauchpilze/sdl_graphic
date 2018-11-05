#ifndef    PICTURECONTAINER_HH
# define   PICTURECONTAINER_HH

#include <memory>
#include <string>
#include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class PictureContainer: public sdl::core::SdlWidget {
      public:

        enum class Mode {
          Crop,
          Fit
        };

      public:

        PictureContainer(const std::string& name,
                         const std::string& picture,
                         const Mode& mode = Mode::Crop,
                         SdlWidget* parent = nullptr,
                         const SDL_Color& backgroundColor = SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE});

        virtual ~PictureContainer();

        void
        setImagePath(const std::string& path);

        void
        setMode(const Mode& mode);

        void
        clear();

      protected:

        virtual SDL_Texture*
        createContentPrivate(SDL_Renderer* renderer) const;

      private:

        void
        loadPicture(SDL_Renderer* renderer) const;

      private:

        std::string m_file;
        Mode m_mode;
        mutable SDL_Texture* m_picture;

    };

    using PictureContainerShPtr = std::shared_ptr<PictureContainer>;
  }
}

# include "PictureContainer.hxx"

#endif    /* PICTURECONTAINER_HH */
