#ifndef WIZARD_ASSETS_PROTO_TEXTURE_HPP
#define WIZARD_ASSETS_PROTO_TEXTURE_HPP

namespace asset
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
