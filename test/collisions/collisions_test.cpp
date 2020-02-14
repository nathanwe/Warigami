#include <collisions/collision_manager.hpp>
#include <collisions/collider.hpp>
#include <iostream>

using namespace collisions;

int main() {
	/*
	collision_manager cm;

	sphere_collider* sphere = new sphere_collider();
	sphere->radius = 1;

	AABB_collider* cube = new AABB_collider();
	cube->left = 1; cube->right = 1; cube->top = 1;
	cube->bottom = 1; cube->front = 1; cube->back = 1;

	std::cout << "Sphere - Sphere\n----------\n";
	std::cout << "\nSame position \n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, 0))? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nTouching sides \n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(1, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(-1, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 1, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, -1, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, -1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nInside \n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0.5, 0.5, 0.5)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(-0.5, -0.5, -0.5)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 1, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(1, 1, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nOutside \n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(2, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 2, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, 2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(-2, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, -2, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, -2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 2, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), sphere, glm::vec3(1, 1, 2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";


	std::cout << "\nCube - Cube\n----------\n";
	std::cout << "\nSame position \n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nTouching sides \n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(1, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(-1, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, 1, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, -1, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, -1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nInside \n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0.5, 0.5, 0.5)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(-0.5, -0.5, -0.5)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, 1, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(1, 1, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nOutside \n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(2, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, 2, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, 2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(-2, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, -2, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, -2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(0, 2, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), cube, glm::vec3(1, 1, 2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";


	std::cout << "\nCube - Sphere\n----------\n";
	std::cout << "\nSame position \n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nTouching sides \n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(1, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(-1, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 1, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, -1, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, -1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nInside \n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0.5, 0.5, 0.5)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(-0.5, -0.5, -0.5)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 1, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(1, 1, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nOutside \n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(2, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 2, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, 2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(-2, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, -2, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 0, -2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(0, 2, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(cube, glm::vec3(0, 0, 0), sphere, glm::vec3(1, 1, 2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";


	std::cout << "\nSphere - Cube\n----------\n";
	std::cout << "\nSame position \n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nTouching sides \n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(1, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(-1, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, 1, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, -1, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, -1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nInside \n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0.5, 0.5, 0.5)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(-0.5, -0.5, -0.5)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, 1, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(1, 1, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";

	std::cout << "\nOutside \n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(2, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, 2, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, 2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(-2, 0, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, -2, 0)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, 0, -2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(0, 2, 1)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	!cm.check_collision(sphere, glm::vec3(0, 0, 0), cube, glm::vec3(1, 1, 2)) ? std::cout << "OK\n" : std::cout << "ERROR\n";
	*/
	return 0;
}