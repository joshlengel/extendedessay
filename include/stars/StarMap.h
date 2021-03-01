#pragma once

#include"render/Model.h"

#include<cstdint>

class StarMap : public virtual Model
{
public:
    StarMap(uint32_t density, float dist, float max_width, float variance);
    virtual ~StarMap();

    virtual void Render() const override;

private:
    uint32_t m_num_indices;
    
    GLuint m_v_buff_id;
    GLuint m_i_buff_id;
};