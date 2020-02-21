//
// Created by sava on 2020-02-19.
//

#ifndef WIZARDENGINE_BONE_FLATTENER_HPP
#define WIZARDENGINE_BONE_FLATTENER_HPP

#include <functional>
#include <vector>
#include <cstdint>
#include <assimp/scene.h>

namespace asset
{
    /**
     * This class takes an assimp bone node graph and re-builds the graph in an array.
     * The indices of this array can then reliably be used as bone ids in various parts
     * of the model loading code.
     *
     * The ultimate goal is to have the bone's index in an array be the bone id.
     * @tparam T bone type to map to
     */
    template<typename T>
    class bone_flattener // _\m/
    {
    public:
        /**
         * Constructor
         * @param scene assimp scene, containing the animation info
         * @param storage an array of whatever type the client uses to represent bone
         * @param storage_size size of storage array
         * @param mapper function with header:
         *  `void mapper(aiNode* assimp_node, T* client_node, T* parent_of_client_node)`
         *  parent_of_client_node is nullptr for root.
         */
        bone_flattener(
                const aiScene* scene,
                T* storage,
                size_t storage_size,
                std::function<void(const aiNode*, T*, T*, bone_flattener<T>&)> mapper) 
                : _storage(storage)
                , _storage_size(storage_size)
                , _mapper(mapper)
        {
            const auto* root = scene->mRootNode;
            load_nodes_recurse(root);
        }

        const std::unordered_map<std::string, T*>& name_to_node() const { return _name_to_node; }
        const std::unordered_map<std::string, size_t>& name_to_index() const { return _name_to_index; };
        T* find_node(const std::string& name) { return _name_to_node.find(name)->second; }
        size_t find_node_index(const std::string& name) { return _name_to_index.find(name)->second; }
        T* root() { return _root; }

    private:
        size_t _bone_count{0};

        T* _storage;
        T* _root;
        size_t _storage_size;
        std::function<void(const aiNode*, T*, T*, bone_flattener<T>&)> _mapper;

        std::unordered_map<std::string, T*> _name_to_node;
        std::unordered_map<std::string, size_t> _name_to_index;

        void load_nodes_recurse(const aiNode* ai_node, T* parent = nullptr)
        {
            T* user_node = allocate_and_map_to(ai_node, parent);

            if (parent == nullptr)
                _root = user_node;

            for (size_t i = 0; i < ai_node->mNumChildren; ++i)
            {
                auto* ai_child = ai_node->mChildren[i];
                load_nodes_recurse(ai_child, user_node);
            }
        }

        T* allocate_and_map_to(const aiNode* ai_node, T* parent)
        {
            if (_bone_count == _storage_size)
            {
                std::cerr << "Could not allocate bone - out of space." << std::endl;
                return nullptr;
            }

            auto* user_node = _storage + _bone_count;
            _name_to_node.insert(std::make_pair(ai_node->mName.data, user_node));
            _name_to_index.insert(std::make_pair(ai_node->mName.data, _bone_count++));
            _mapper(ai_node, user_node, parent, *this);
            return user_node;
        }
    };
}

#endif //WIZARDENGINE_BONE_FLATTENER_HPP
