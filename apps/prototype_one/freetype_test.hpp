#ifndef __FREETYPE_TEST_HPP_
#define __FREETYPE_TEST_HPP_

#include <glm/vec2.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
using namespace gl;

class freetype_test
{
	struct Character {
		GLuint     TextureID;  // ID handle of the glyph texture
		glm::ivec2 Size;       // Size of glyph
		glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
		GLuint     Advance;    // Offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;

public:
	freetype_test()
	{
		FT_Face     face;      /* handle to face object */
		auto error = FT_Init_FreeType(&library);
		if (error)
		{
			std::cerr << "Freetype failed to initialize." << std::endl;
			return 1;
		}

		error = FT_New_Face(library, "/assets/fonts/papercuts-2.ttf", 0, &face);
		if (error == FT_Err_Unknown_File_Format)
		{
			std::cerr
				<< "the font file could be openedand read, but it appears that its font format is unsupported"
				<< std::endl;
		}
		else if (error)
		{
			std::cerr << "Font loading failure" << std::endl;
		}

		FT_Set_Pixel_Sizes(face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (GLubyte c = 0; c < 128; c++)
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
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<GLchar, Character>(c, character));
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}


private:

};

#endif