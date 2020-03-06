#ifndef __FREETYPE_SYSTEM_HPP_
#define __FREETYPE_SYSTEM_HPP_

#include <cstdint>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
using namespace gl;


#include <ecs/system_base.hpp>

namespace rendering
{
	class freetype_system : public ecs::system_base
	{
		static constexpr std::uint16_t NumCharacters = 128;
		struct Character {
			GLuint     textureID;  // ID handle of the glyph texture
			glm::ivec2 size;       // Size of glyph
			glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
			GLuint     advance;    // Offset to advance to next glyph
		};

	public:
		freetype_system();
		~freetype_system();

		virtual void update(ecs::state& state) override;


	private:
		Character _characters[NumCharacters];

		FT_Library ft;
		FT_Face face;

		void load_characters();
	};
}


#endif