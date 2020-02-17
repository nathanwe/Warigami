#ifndef WIZARD_ASSETS_PROTO_TEXTURE_HDR_HPP
#define WIZARD_ASSETS_PROTO_TEXTURE_HDR_HPP

namespace asset
{
	struct proto_texture_hdr
	{
		int width = 0;
		int height = 0;
		int channels = 0;
		float* bytes = nullptr;
	};
}

#endif
