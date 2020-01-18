#ifndef __TEST_COMPONENTS_HPP
#define __TEST_COMPONENTS_HPP

struct transform : public ecs::component<transform>
{
    float m[16];
    float pitch;
    float roll;
    float yaw;
    glm::vec3 pos;

    transform()
    {
        memset(m, 0, sizeof(float) * 16);
        m[0] = m[5] = m[10] = m[15] = 1.f;
    }

    glm::mat4 to_mat4()
    {
        return glm::make_mat4(m);
    }
};

struct renderdata : public ecs::component<renderdata>
{
    unsigned int texture_id;
    unsigned int vbo_id;
};

template<> const component_shift ecs::component<transform>::component_bitshift = 0;
template<> const component_shift ecs::component<renderdata>::component_bitshift = 1;


#endif
