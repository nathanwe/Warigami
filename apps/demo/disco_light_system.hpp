#ifndef GAME_DISCO_LIGHT_SYSTEM_HPP
#define GAME_DISCO_LIGHT_SYSTEM_HPP

#include <transforms/transform.hpp>
#include <rendering/light_point.hpp>
#include <core/frame_timer.hpp>
#include "components/disco_light.h"

class disco_light : public ecs::system_base
{
public:
	disco_light(
	        core::frame_timer& timer,
            asset::scene_hydrater& hydrater,
	        glm::vec3 center = glm::vec3(0,0,0))
	        : m_timer(timer)
	        , m_hydrater(hydrater)
	        , t(0), dir(1)
	        , color(0)
	        , angle(0)
	        , center(0.f) {}

	virtual void update(ecs::state& r_state) override
	{
	    static bool added = false;
	    if (!added)
        {
	        auto& entity = m_hydrater.add_from_prototype("assets/prototypes/disco.json");
	        added = true;
        }

		r_state.each< transforms::transform, rendering::light_point, components::disco_light>([&](
		        transforms::transform& transform,
		        rendering::light_point& light_point,
                components::disco_light& disco)
		{
		    if (center.x == 0 && center.y == 0 && center.z == 0)
		        center = transform.local_to_world[3];

			// Circles around center
			angle += m_timer.smoothed_delta_secs();
			transform.position = center + glm::vec3(sinf(angle), 0, cosf(angle));

			// Glowy in R, G, B, and white
			t += dir * m_timer.smoothed_delta_secs() * 1.5;
			// t goes up and down smoothly
			if (dir == 1 && t >= 0.8) {
				dir = -dir;
			}
			else if (dir == -1 && t <= 0) {
				dir = -dir;
				color = (color + 1) % 4;
			}
			// Each time a different color
			float r = 0, g = 0, b = 0;
			switch (color) {
			case 0:
				r = t;
				break;
			case 1:
				g = t;
				break;
			case 2:
				b = t;
				break;
			case 3:
				r = t;
				g = t;
				b = t;
				break;
			}
			light_point.color = glm::vec3(r, g, b);
		});
	}

private:
	core::frame_timer& m_timer;
    asset::scene_hydrater& m_hydrater;

	float t;
	int dir;
	int color;
	float angle;
	glm::vec3 center;
};

#endif