#include "ecs/register_component.hpp"
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
		camera.field_of_view = 90.f;
		camera.near_clip_distance = .01f;
		camera.far_clip_distance = 100.f;

		camera.clear_color = glm::vec3(0, 0.5f, 0.75f);
		camera.clear_setting = rendering::camera::clear_mode::flat_color;

		// init box
		auto& box_e = r_state.add_entity<transforms::transform, rendering::renderable_mesh_static>();
		m_box = box_e.id();

		auto& box_transform = box_e.get_component<transforms::transform>();
		box_transform.position = glm::vec3(0, 0, -10);
		box_transform.scale = glm::vec3(5, 5, 5);

		auto& box_render = box_e.get_component<rendering::renderable_mesh_static>();
		auto& box_mesh = box_render.mesh;

		float box_vertices[] =
		{
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};

		glGenVertexArrays(1, &box_mesh.vao);
		glGenBuffers(1, &box_mesh.vbo);

		glBindBuffer(GL_ARRAY_BUFFER, box_mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(box_vertices), box_vertices, GL_STATIC_DRAW);

		glBindVertexArray(box_mesh.vao);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		box_mesh.num_indices = 36;
	}

	virtual void update(ecs::state& r_state) override
	{
		// spin box
		if (r_state.has_entity(m_box))
		{
			ecs::entity& box_e = r_state.find_entity(m_box);
			transforms::transform& box_transform = box_e.get_component<transforms::transform>();

			box_transform.rotation.y += m_rotation_speed; // needs dt
			box_transform.is_matrix_dirty = true;

			int asdfadsfasdf = 0;
		}
	}

private:
	entity_id m_box = 0;
	float m_rotation_speed = .001f;

};

int main(int argc, char** argv)
{
#ifndef NDEBUG
	bool is_debug = true;
#else
	bool is_debug = false;
#endif

	// init glfw
	if (!glfwInit())
	{
		std::cout << "Error: GLFW failed to initialize." << std::endl;
		return -1;
	}

	uint32_t window_width = 1280;
	uint32_t window_height = 720;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RED_BITS, 32);
	glfwWindowHint(GLFW_GREEN_BITS, 32);
	glfwWindowHint(GLFW_BLUE_BITS, 32);
	glfwWindowHint(GLFW_ALPHA_BITS, 32);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	if (is_debug)
	{
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	}
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Wizard People", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Error: GLFW failed to create window." << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);

	rendering::viewport window_view{ 0, 0, window_width, window_height };

	// init ecs state
	ecs::archetype_pools memory;
	ecs::state state(memory);
	ecs::register_component<transforms::transform>("transform");
	ecs::register_component<rendering::camera>("camera");
	ecs::register_component<rendering::renderable_mesh_static>("renderable_mesh_static");

	// init ecs systems
	rendering::renderer renderer(window, window_view, is_debug);
	tester_system tester(state);
	transforms::transformer transformer;
	rendering::camera_updater camera_updater;
	ecs::systems systems({ &tester, &transformer, &camera_updater, &renderer });
	ecs::world world(systems, state);

	//game loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		world.update();
	}

	glfwTerminate();
}
