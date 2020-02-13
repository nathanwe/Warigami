#ifndef WIZARD_TRANSFORM_TREE_HPP
#define WIZARD_TRANSFORM_TREE_HPP

#include "ecs/ecs_types.hpp"
#include "ecs/state.hpp"
#include "transforms/transform.hpp"

#include <algorithm>
#include <vector>

namespace engineui
{
	class transform_tree
	{
		struct node
		{
			entity_id id;
			std::vector<node> children;
		};

	public:
		transform_tree(ecs::state& ecs_state) : _ecs_state(ecs_state)
		{
		}

		void insert(entity_id id)
		{
			assert(_ecs_state.has_entity(id));
			ecs::entity& e = _ecs_state.find_entity(id);
			assert(e.has<transforms::transform>());

			if (contains(id))
			{
				// early exit
				return;
			}

			// prepare new node for insertion
			node self;
			self.id = id;

			auto is_self = [&](auto& n) { return n.id == self.id; };
			auto is_lesser_node = [](const node& a, const node& b) { return a.id < b.id; };

			transforms::transform& t = e.get_component<transforms::transform>();
			if (t.has_parent)
			{
				// recursively verify insertion of parents before self
				insert(t.parent);
				node& parent = *find(t.parent);

				// insert self as child
				if (std::find_if(parent.children.begin(), parent.children.end(), is_self) != parent.children.end())
				{
					parent.children.push_back(self);
					std::sort(parent.children.begin(), parent.children.end(), is_lesser_node);
				}
			}
			else
			{
				// insert self as a root
				_roots.push_back(self);
				std::sort(_roots.begin(), _roots.end(), is_lesser_node);
			}
		}

		node* find(entity_id id)
		{
			assert(contains(id));
			return find_recurse(id, _roots);
		}

		bool contains(entity_id id)
		{
			contains_recurse(id, _roots);
		}

		void clear()
		{
			_roots.clear();
		}

		template <typename TFunc>
		void preorder(TFunc functor)
		{
			preorder_recurse(functor, _roots);
		}

	private:
		node* find_recurse(entity_id id, std::vector<node>& nodes)
		{
			for (auto& n : _roots)
			{
				if (n.id == id)
				{
					return &n;
				}
				find_recurse(id, n.children);
			}
			return nullptr;
		}

		bool contains_recurse(entity_id id, std::vector<node>& nodes)
		{
			for (auto& n : nodes)
			{
				if (n.id == id || contains_recurse(id, n.children))
				{
					return true;
				}
			}
			return false;
		}

		template <typename TFunc>
		void preorder_recurse(TFunc functor, std::vector<node>& nodes)
		{
			for (auto& n : nodes)
			{
				functor(n);
				preorder_recurse(functor, n.children);
			}
		}

	private:
		ecs::state& _ecs_state;
		std::vector<node> _roots;
	};
}

#endif
