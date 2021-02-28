#pragma once

#include"render/Model.h"

class PlanetModel : public virtual Model
{
public:
    PlanetModel(uint32_t precision);
    ~PlanetModel();

    virtual void Render() const override;

private:
    GLuint m_v_buff_id;
    GLuint m_i_buff_id;

    uint32_t m_num_indices;
};