#pragma once

#include "lmb_obj.h"
#include "bitmap.h"




namespace LMB
{

class Lightmap;
class Triangle;

class NSHandle : public LMBObject
{

public:

    explicit NSHandle(size_t index)
    {
        m_index = index + 1;
    }

    NSHandle()
    {
        m_index = 0;
    }

    inline const size_t GetIndex()const
    {
        if(m_index > 0)
            return m_index - 1;

        return 0;
    }

    bool IsValid()const
    {
        return (m_index > 0 && GetLMB());
    }

private:

    size_t m_index;
};

class LightmapHandle : public NSHandle
{

public:

    explicit LightmapHandle(size_t index)
    : NSHandle(index)
    {
    }

    LightmapHandle()
    : NSHandle()
    {
    }

    std::shared_ptr<Lightmap> Get()const;

};

class BitmapHandle : public NSHandle
{

public:

    explicit BitmapHandle(size_t index)
    : NSHandle(index)
    {
    }

    BitmapHandle()
    : NSHandle()
    {
    }

    std::shared_ptr<Bitmap<vec4>> Get()const;

};

class TriangleArrayHandle : LMBObject
{

    void AddTriangle(const size_t index)
    {
        m_indices.push_back(index);
    }

    const size_t GetIndex(const size_t index)const;
    const std::vector<size_t> &GetIndices()const;
    const std::vector<Triangle> GetTriangles()const;
    const Triangle *GetTriangle(const size_t index)const;

public:

    std::vector<size_t> m_indices;

};

}