#include "ecs/state.hpp"
#include "ecs/world.hpp"
#include "transforms/transform.hpp"
#include "transforms/transformer.hpp"
#include "rendering/camera.hpp"
#include "rendering/camera_updater.hpp"
#include "rendering/renderable_mesh_static.hpp"
#include "rendering/renderer.hpp"

#include "glbinding/gl/gl.h"
#include "glbinding/glbinding.h"
using namespace gl;

class tester_system : public ecs::system_base
{
public:
	tester_system(ecs::state& r_state)
	{
		// init camera
		auto camera_e = r_state.add_entity<transforms::transform, rendering::camera>();
		auto& camera = camera_e.get_component<rendering::camera>();

		camera.aspect_ratio = 16.f / 9.f; // should get from renderer singleton component
		camera.field_of_view = 60.f;
		camera.near_clip_distance = .1f;
		camera.far_clip_distance = 100.f;

		camera.clear_color = glm::vec3(0, 0.5f, 0.75f);
		camera.clear_setting = rendering::camera::clear_mode::flat_color;

		// init box
		auto& box_e = r_state.add_entity<transforms::transform, rendering::renderable_mesh_static>();
		m_box = box_e.id();

		auto& box_transform = box_e.get_component<transforms::transform>();
		box_transform.position = glm::vec3(0, 0, 5);

		auto& box_render = box_e.get_component<rendering::renderable_mesh_static>();
		auto& box_mesh = box_render.mesh;

		float box_vertices[] =
		{
			-0.5, 0.5, -0.5,
			-0.5, 0.5, 0.5,
			0.5, 0.5, 0.5,
			0.5, 0.5, -0.5,
			-0.5, -0.5, -0.5,
			-0.5, -0.5, 0.5,
			0.5, -0.5, 0.5,
			0.5, -0.5, -0.5
		};

		unsigned int box_indices[] =
		{
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0
		};

		glGenVertexArrays(1, &box_mesh.vao);
		glGenBuffers(1, &box_mesh.vbo);
		glGenBuffers(1, &box_mesh.ebo);

		glBindVertexArray(box_mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, box_mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), box_vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box_mesh.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 3 * sizeof(unsigned int), box_indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindVertexArray(0);
	}

	virtual void update(ecs::state& r_state) override
	{
		// spin box
		if (r_state.has_entity(m_box))
		{
			ecs::entity& box_e = r_state.find_entity(m_box);
			transforms::transform& box_transform = box_e.get_component<transforms::transform>();

			box_transform.rotation.y += m_rotation_speed; // needs dt
		}
	}

private:
	entity_id m_box = 0;
	float m_rotation_speed = 1.f;

};

int main(int argc, char** argv)
{
	ecs::archetype_pools memory;
	ecs::state state(memory);

	tester_system tester(state);
	transforms::transformer transformer;
	rendering::camera_updater camera_updater;

	if (!glfwInit())
	{
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Wizard People", nullptr, nullptr);
	rendering::renderer renderer(window);

	ecs::systems systems({ &tester, &transformer, &camera_updater, &renderer });
	ecs::world world(systems, state);

	// loop
	world.update();
}
