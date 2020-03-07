#ifndef __FREETYPE_SYSTEM_HPP_
#define __FREETYPE_SYSTEM_HPP_

#include <cstdint>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

using namespace gl;

#include <ecs/system_base.hpp>
#include <core/viewport.hpp>
#include <asset/asset_manager.hpp>
#include <util/string_table.hpp>

#include "render_pass.hpp"


namespace rendering
{
    class freetype_system : public ecs::system_base
    {
        static constexpr std::uint16_t NumCharacters = 128;
        struct Character
        {
            GLuint textureID;  // ID handle of the glyph texture
            glm::ivec2 size;       // Size of glyph
            glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
            GLuint advance;    // Offset to advance to next glyph
        };

    public:
        freetype_system(
            core::viewport &window_view,
            asset::asset_manager &assets,
            util::string_table &strings);

        void update(ecs::state &state) override;


    private:
        core::viewport &_window_view;
        asset::asset_manager &_assets;
        util::string_table& _strings;

        Character _characters[NumCharacters]{};

        FT_Library ft{};
        FT_Face face{};
        GLuint VAO{}, VBO{};

        render_pass::description _pass_desc;
        render_pass _pass_freetype;
        render_state _render_state;


        void load_characters();

        static render_pass::description make_pass_description(asset::asset_manager &assets);

    };
}


#endif