//
// Created by sava on 2020-02-18.
//

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/gtc/type_ptr.hpp>

#include <rendering/loader_rigged_model.hpp>
#include <rendering/renderable_mesh_rigged.hpp>


rendering::loader_rigged_model::loader_rigged_model(asset::asset_manager& assets, rendering::asset_cache& render_cache) 
    : _assets(assets)
    , _render_cache(render_cache)
{
}

void rendering::loader_rigged_model::load(asset::asset_loader_node& asset_loader_node)
{
    auto& entity = asset_loader_node.entity_resource.entity;
    auto& entity_data = asset_loader_node.entity_resource.entity_data;
    auto& json = entity_data.component_data(rendering::renderable_mesh_rigged::component_bitshift);
    auto& mesh = entity.get_component<rendering::renderable_mesh_rigged>();

    auto& ai_resource = _assets.get_proto_mesh(json["mesh"].get<std::string>());
    
    build_animation_component(ai_resource.assimp_scene, mesh);
}

void rendering::loader_rigged_model::build_animation_component(aiScene* scene, renderable_mesh_rigged& component)
{
    std::unordered_map<std::string, skeleton_node*> name_to_node;
    auto* ai_root = scene->mRootNode;
    auto* root = component.allocate_node();
    load_nodes_recurse(ai_root, root, component, name_to_node);

    if (scene->mNumAnimations > skeleton_node::MaxAnimations)
    {
        std::cerr << "Animation limit exceeded." << std::endl;
        assert(false);
    }

    component.animation_count = scene->mNumAnimations;
    
    for (size_t animation_index = 0; animation_index < scene->mNumAnimations; ++animation_index)
    {
        auto* animation = scene->mAnimations[animation_index];
        auto duration = tick_to_time(animation->mTicksPerSecond, animation->mDuration);

        // channels are the bones
        for (size_t bone_index = 0; bone_index < animation->mNumChannels; ++bone_index)
        {
            auto* ai_bone = animation->mChannels[bone_index];
            std::string name(ai_bone->mNodeName.data);
            auto* node = name_to_node.find(name)->second;            
                        
            for (size_t pos_i = 0; pos_i < ai_bone->mNumPositionKeys; ++pos_i)
            {           
                auto& pos_key = ai_bone->mPositionKeys[pos_i];
                auto& pos = node->animations[animation_index].position;
                pos.times[pos_i] = pos_key.mTime;
                pos.values[pos_i] = map_vec3(pos_key.mValue);
            }
                        
            for (size_t rot_i = 0; rot_i < ai_bone->mNumRotationKeys; ++rot_i)
            {
                auto& rot_key = ai_bone->mRotationKeys[rot_i];
                auto& rot = node->animations[animation_index].rotation;
                rot.times[rot_i] = rot_key.mTime;
                rot.values[rot_i] = map_quat(rot_key.mValue);
            }
                        
            for (size_t scl_i = 0; scl_i < ai_bone->mNumScalingKeys; ++scl_i)
            {
                auto& scl_key = ai_bone->mScalingKeys[scl_i];
                auto& scl = node->animations[animation_index].scale;
                scl.times[scl_i] = scl_key.mTime;
                scl.values[scl_i] = map_vec3(scl_key.mValue);                
            }
        }
    }
}

void rendering::loader_rigged_model::load_nodes_recurse(
    aiNode* ai_node, 
    skeleton_node* node, 
    renderable_mesh_rigged& mesh,
    std::unordered_map<std::string, skeleton_node*>& name_to_node) const
{
    name_to_node.insert(std::make_pair(ai_node->mName, node));
    node->base_transform = map_matrix(ai_node->mTransformation);

    for (size_t i = 0; i < ai_node->mNumChildren; ++i)
    {
        auto* ai_child = ai_node->mChildren[i];
        auto* child = mesh.allocate_node();        
        node->add_child(child);
        load_nodes_recurse(ai_child, child, mesh);
    }
}

rendering::animation_time rendering::loader_rigged_model::tick_to_time(double ticks_per_second, double ticks) const
{
    return animation_time(ticks / ticks_per_second);
}

component_bitset rendering::loader_rigged_model::components_to_load()
{
    return rendering::renderable_mesh_rigged::archetype_bit;
}

glm::mat4 rendering::loader_rigged_model::map_matrix(aiMatrix4x4& source) const
{
    // assume that matrix memory layour is the same everywhere...?
    glm::mat4 target;
    float* target = glm::value_ptr(node->base_transform);
    for (size_t i = 0; i < 16; ++i)
        for (size_t j = 0; j < 16; ++j)
            target[i][j] = source[i][j];
    return target;
}

glm::vec3 rendering::loader_rigged_model::map_vec3(aiVector3D& vec) const
{
    return { vec[0], vec[1], vec[2] };
}

glm::quat rendering::loader_rigged_model::map_quat(aiQuaternion q) const
{
    return glm::quat(q.w, q.x, q.y, q.z);
}