#ifndef WIZARD_GAME_LETTERBOX_HPP
#define WIZARD_GAME_LETTERBOX_HPP

#include <core/viewport.hpp>

core::viewport letterbox(const core::viewport& view)
{
	float desired_aspect = 16.f / 9.f;
	float actual_aspect = static_cast<float>(view.width) / view.height;
	if (actual_aspect == desired_aspect)
	{
		return view;
	}
	else
	{
		core::viewport letterboxed;
		if (actual_aspect > desired_aspect)
		{
			letterboxed.width = desired_aspect * view.height;
			letterboxed.height = view.height;
			float diff = view.width - letterboxed.width;
			letterboxed.x = diff / 2.f;
			letterboxed.y = 0.f;
		}
		else
		{
			letterboxed.width = view.width;
			letterboxed.height = view.width / desired_aspect;
			float diff = view.height - letterboxed.height;
			letterboxed.x = 0.f;
			letterboxed.y = diff / 2.f;
		}
		return letterboxed;
	}
}

#endif
