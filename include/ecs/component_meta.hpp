//
// Created by sava on 10/8/19.
//

#ifndef ECS_ATTEMPT2_COMPONENT_META_H
#define ECS_ATTEMPT2_COMPONENT_META_H

#include <map>
#include <cstdint>
#include <functional>

#include <nlohmann/json.hpp>
#include <utility>
using nlohmann::json;

#include <ecs/ecs_types.hpp>
#include <ecs/component.hpp>

namespace ecs
{
    class component_meta
    {
    public:
        component_meta &operator=(const component_meta &other)
        {
            _size = other._size;
            _align = other._align;
            _shift = other._shift;
            return *this;
        }

        /**
         * Function for constructing a type metadata object of type T. Only default ctor supported for now.
         * @tparam T A component type. must have a static component_bitshift member.
         * @return A metadata object for type T
         */
        template<class T>
        static component_meta of()
        {
            return component_meta(
                sizeof(T),
                alignof(T),
                component<T>::component_bitshift,
                [](void *addr) { new(addr) T(); },
                [](void *addr) { ((T *) addr)->~T(); },
                [](void *destination, void* source) {
                    *((T*)destination) = *((T *)source);
                });
        }

        static std::map<component_shift, component_meta> bit_metas;

        static std::map<std::string, component_shift> type_to_bit;

        size_t size() const { return _size; }

        size_t align() const { return _align; }

        component_shift shift() const { return _shift; }


        void construct(void *addr) { _ctor(addr); }

        void destruct(void *addr) { _dtor(addr); }

        void copy(void *destination, void* source) { _copier(destination, source); }


    private:
        component_meta(
            size_t size,
            size_t align,
            component_shift shift,
            std::function<void(void *)> ctor,
            std::function<void(void *)> dtor,
            std::function<void(void*, void*)> copier) :
            _size(size),
            _align(align),
            _shift(shift),
            _ctor(std::move(ctor)),
            _dtor(std::move(dtor)),
            _copier(std::move(copier))
        {
        }

        // const as far as client goes, but need to assign-construct.
        mutable size_t _size;
        mutable size_t _align;
        mutable std::uint8_t _shift;

        mutable std::function<void(void *)> _ctor;
        mutable std::function<void(void *)> _dtor;
        mutable std::function<void(void*, void*)> _copier;
    };
}

#endif //ECS_ATTEMPT2_COMPONENT_META_H
