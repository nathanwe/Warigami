#include <glm/vec2.hpp>

#include <rendering/freetype_system.hpp>
#include <rendering/renderable_text.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/mat4x4.hpp>


rendering::freetype_system::freetype_system(
        core::viewport &window_view,
        asset::asset_manager &assets,
        util::string_table &strings)
    : _window_view(window_view)
    , _assets(assets)
    , _strings(strings)
    , _pass_desc(make_pass_description(assets))
    , _pass_freetype(_pass_desc)
{	
	if (FT_Init_FreeType(&ft))
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	
	if (FT_New_Face(ft, "assets/fonts/papercuts-2.ttf", 0, &face))
		std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

    load_characters();

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void rendering::freetype_system::update(ecs::state& state)
{
	state.each<renderable_text>([&](renderable_text& text) {
        glm::mat4 projection = glm::ortho(
                0.0f,
                (float)_window_view.width,
                0.0f,
                (float)_window_view.height);

        _pass_freetype.bind(_render_state);

        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

        _pass_freetype.set_mat4(0, projection);
        _pass_freetype.set_float3(1, text.color);


        glBindVertexArray(VAO);

        auto x = text.position.x;
        auto y = text.position.y;
        auto scale = text.scale;

        // Iterate through all characters
        std::string::const_iterator c;
        auto& str = _strings[text.string_hash];
        for (c = str.begin(); c != str.end(); c++)
        {
            Character& ch = _characters[*c];

            GLfloat xpos = x + ch.bearing.x * scale;
            GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

            GLfloat w = ch.size.x * scale;
            GLfloat h = ch.size.y * scale;

            // Update VBO for each character
            GLfloat vertices[6][4] = {
                    { xpos,     ypos + h,   0.0, 0.0 },
                    { xpos,     ypos,       0.0, 1.0 },
                    { xpos + w, ypos,       1.0, 1.0 },
                    { xpos,     ypos + h,   0.0, 0.0 },
                    { xpos + w, ypos,       1.0, 1.0 },
                    { xpos + w, ypos + h,   1.0, 0.0 }
            };
            // Render glyph texture over quad
            //glBindTexture(GL_TEXTURE_2D, ch.textureID);
            _pass_freetype.set_texture(0, ch.textureID);

            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

    });
}

void rendering::freetype_system::load_characters()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (GLubyte c = 0; c < NumCharacters; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use
        _characters[c].textureID = texture;
        _characters[c].size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        _characters[c].bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        _characters[c].advance = face->glyph->advance.x;
    }
}

rendering::render_pass::description rendering::freetype_system::make_pass_description(asset::asset_manager &assets)
{
    render_pass::description pass_freetype_desc(
            assets.get_proto_shader("assets/shaders/freetype.vert"),
            assets.get_proto_shader("assets/shaders/freetype.frag"));

    pass_freetype_desc.state.polygon_mode = GL_FILL;
    pass_freetype_desc.state.uses_cull_face = GL_TRUE;
    pass_freetype_desc.state.uses_depth_test = GL_TRUE;
    pass_freetype_desc.state.culled_face = GL_BACK;
    pass_freetype_desc.state.depth_func = GL_LESS;
    pass_freetype_desc.state.depth_mask = GL_FALSE;

    pass_freetype_desc.state.uses_blend = GL_TRUE;
    pass_freetype_desc.state.blend_src = GL_SRC_ALPHA;
    pass_freetype_desc.state.blend_dest = GL_ONE_MINUS_SRC_ALPHA;

    return pass_freetype_desc;
}

