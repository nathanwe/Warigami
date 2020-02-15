#ifndef WIZARD_RENDERING_DEBUG_VIEW_HPP
#define WIZARD_RENDERING_DEBUG_VIEW_HPP

#include "engine-ui/view.hpp"
#include "rendering/renderer.hpp"

namespace rendering
{
	class debug_view : public engineui::view
	{
	public:
		debug_view(core::viewport& viewport, renderer& renderer_);

		void draw() override;

	private:
		renderer& _renderer;
	};

}

#endif