#include <asset/model_loader.hpp>

void test_model_loader()
{
    asset::model_loader loader;
    loader.load("assets/models/cube/cube.gltf");
}