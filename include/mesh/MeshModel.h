#pragma once

#include"mesh/Mesh.h"
#include"render/Model.h"

class IcoModel : public virtual Model
{
public:
    IcoModel(uint32_t precision);
    virtual ~IcoModel();

    virtual void Render() const override;

private:
    IcoSphere m_mesh;
    uint32_t m_num_indices;

    GLuint m_v_buff_id;
    GLuint m_i_buff_id;
};