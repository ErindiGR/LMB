#include "lmb/handles.h"
#include "lmb/lmb.h"
#include "lmb/triangle.h"

namespace LMB
{

std::shared_ptr<Lightmap> LightmapHandle::Get()const
{
    assert(GetLMB());

    return GetLMB()->GetLightmap(GetIndex());
} 

std::shared_ptr<Bitmap<vec4>> BitmapHandle::Get()const
{
    assert(GetLMB());

    return GetLMB()->GetBitmap(GetIndex());
}

const size_t TriangleArrayHandle::GetIndex(const size_t index)const
{
    if(m_indices.size() > index)
        return m_indices[index];
    
    return 0;
}

const std::vector<size_t> &TriangleArrayHandle::GetIndices()const
{
    return m_indices;
}

const std::vector<Triangle> TriangleArrayHandle::GetTriangles()const
{
    std::vector<Triangle> ret(m_indices.size());

    for(size_t i=0;i<ret.size();i++)
        ret[i] = GetLMB()->GetTriangles()[m_indices[i]];
    
    return ret;
}

const Triangle *TriangleArrayHandle::GetTriangle(const size_t index)const
{
    if(m_indices.size() > index)
            return &GetLMB()->GetTriangles()[m_indices[index]];
        
    return nullptr;
}

}