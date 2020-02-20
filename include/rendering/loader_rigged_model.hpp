//
// Created by sava on 2020-02-18.
//

#ifndef WIZARDENGINE_LOADER_RIGGED_MODEL_HPP
#define WIZARDENGINE_LOADER_RIGGED_MODEL_HPP

#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>
#include <rendering/asset_cache.hpp>
#include <chrono>
#include <asset/bone_flattener.hpp>
#include "renderable_mesh_rigged.hpp"

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

        void build_animation_component(const aiScene* mesh, renderable_mesh_rigged& component);
        void load_animation_data(
                const aiScene* mesh,
                renderable_mesh_rigged& component,
                asset::bone_flattener<skeleton_node>& flattener);

        [[nodiscard]] animation_time tick_to_time(double ticks_per_second, double ticks) const;
        [[nodiscard]] glm::mat4 map_matrix(const aiMatrix4x4& mat) const;
        glm::vec3 map_vec3(aiVector3D& vec) const;
        [[nodiscard]] glm::quat map_quat(aiQuaternion q) const;
    };
}


#endif //WIZARDENGINE_LOADER_RIGGED_MODEL_HPP
