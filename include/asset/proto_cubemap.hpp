#ifndef WIZARD_ASSETS_PROTO_CUBEMAP_HPP
#define WIZARD_ASSETS_PROTO_CUBEMAP_HPP

#include "assets/proto_texture.hpp"

#include <array>
#include <string>

namespace assets
{
	struct proto_texture
	{
		int width = 0;
		int height = 0;
		int channels = 0;
		unsigned char* bytes = nullptr;
	};
}

#endif
