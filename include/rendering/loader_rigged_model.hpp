//
// Created by sava on 2020-02-18.
//

#ifndef WIZARDENGINE_LOADER_RIGGED_MODEL_HPP
#define WIZARDENGINE_LOADER_RIGGED_MODEL_HPP

#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>
#include <rendering/asset_cache.hpp>
#include <chrono>

namespace rendering
{
    class loader_rigged_model : public asset::component_loader
    {
    public:
        loader_rigged_model(asset::asset_manager& assets, rendering::asset_cache& render_cache);

        void load(asset::asset_loader_node& asset_loader_node) override;
        component_bitset components_to_load() override;

    private:
        asset::asset_manager& _assets;
        rendering::asset_cache& _render_cache;

        void build_animation_component(aiScene* mesh, renderable_mesh_rigged& component);
        animation_time tick_to_time(double ticks_per_second, double ticks) const;

        void load_nodes_recurse(
            aiNode* ai_node, 
            skeleton_node* node, 
            renderable_mesh_rigged& mesh,
            std::unordered_map<std::string, skeleton_node*>& name_to_node) const;

        glm::mat4 map_matrix(aiMatrix4x4& aimat) const;
        glm::vec3 map_vec3(aiVector3D& vec) const;
        glm::quat map_quat(aiQuaternion q) const;
    };
}


#endif //WIZARDENGINE_LOADER_RIGGED_MODEL_HPP
