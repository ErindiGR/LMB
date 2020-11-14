#include "lmb/dumper.h"


#include <stdio.h>
#include <typeindex>


namespace LMB
{


std::vector<uint8_t> Dumper::s_data;
size_t Dumper::s_pop_index = 0;
size_t Dumper::s_writen = 0;
FILE* Dumper::s_file = nullptr;
std::mutex Dumper::s_push_mutex;


template<>
void Dumper::Pop<GridCell>(GridCell &cell);

template<>
void Dumper::Pop<Ray>(Ray &ray);

template<>
void Dumper::Pop(Triangle &tri);

template<>
void Dumper::Pop(real_t &f);

template<>
void Dumper::Pop(int &i);

template<>
void Dumper::Pop(size_t &i);


void Dumper::Init(const std::string &filename)
{
    s_file = fopen(filename.c_str(),"wb");

    if(!s_file)
    {
        printf("error: unable to open file %s .\n",filename.c_str());
        return;
    }

    s_data.reserve(50000);
}

void Dumper::Term()
{
    fclose(s_file);
}

std::shared_ptr<DumpInfo> Dumper::Load(const std::string &filename)
{
    s_file = fopen(filename.c_str(),"rb");

    if(!s_file)
    {
        printf("error: unable to open file %s .\n",filename.c_str());
        return nullptr;
    }

    fseek(s_file,0,SEEK_END);
    size_t size = ftell(s_file);
    fseek(s_file,0,SEEK_SET);

    s_data.resize(size);
    fread(s_data.data(),sizeof(uint8_t),size,s_file);

    std::shared_ptr<DumpInfo> info = std::make_shared<DumpInfo>();

    while(s_data.size() > s_pop_index)
    {
        size_t type_id = 0;
        Pop(type_id);

        if(type_id == typeid(Triangle).hash_code())
        {
            Triangle tri;
            Pop(tri);
            info->m_triangles.push_back(tri);
        }
        else if(type_id == typeid(Ray).hash_code())
        {
            Ray ray;
            Pop(ray);
            info->m_rays.push_back(ray);
        }
        else if(type_id == typeid(GridCell).hash_code())
        {
            GridCell gc;
            Pop(gc);
            info->m_grid.push_back(gc);
        }

    }

    fclose(s_file);

    //clear the s_data vector
    std::vector<uint8_t>().swap(s_data);

    return info;
}


void Dumper::Save()
{
    const size_t chunk_size = 2048 * 16;
    if(chunk_size < s_data.size())
    {
        fwrite(s_data.data(),sizeof(uint8_t),s_data.size(),s_file);
        s_data.clear();
    }
}



template<>
void Dumper::__Push(const real_t &f)
{
    const uint8_t * p = (const uint8_t *)&f;

    for(size_t i=0;i<sizeof(f);i++)
    {
        s_data.push_back(*(p));
        ++p;
    }
}

template<>
void Dumper::__Push(const int &t)
{
    const uint8_t * p = (const uint8_t *)&t;

    for(size_t i=0;i<sizeof(t);i++)
    {
        s_data.push_back(*(p));
        ++p;
    }
}

template<>
void Dumper::__Push(const size_t &t)
{
    const uint8_t * p = (const uint8_t *)&t;

    for(size_t i=0;i<sizeof(t);i++)
    {
        s_data.push_back(*(p));
        ++p;
    }
}


template<>
void Dumper::__Push(const Triangle &tri)
{
    __Push(typeid(tri).hash_code());

    for(size_t i=0;i<tri.GetPos().size();i++)
    {
        __Push(tri.GetPos()[i].x);
        __Push(tri.GetPos()[i].y);
        __Push(tri.GetPos()[i].z);
    }
}

template<>
void Dumper::__Push(const Ray &ray)
{
    __Push(typeid(ray).hash_code());

    __Push(ray.GetStart().x);
    __Push(ray.GetStart().y);
    __Push(ray.GetStart().z);

    __Push(ray.GetEnd().x);
    __Push(ray.GetEnd().y);
    __Push(ray.GetEnd().z);

}

template<>
void Dumper::__Push(const GridCell &cell)
{
    __Push(typeid(cell).hash_code());

    __Push(cell.GetAABB().GetMin().x);
    __Push(cell.GetAABB().GetMin().y);
    __Push(cell.GetAABB().GetMin().z);

    __Push(cell.GetAABB().GetMax().x);
    __Push(cell.GetAABB().GetMax().y);
    __Push(cell.GetAABB().GetMax().z);

    __Push(cell.GetTriangles().size());

    for(size_t i=0;i<cell.GetTriangles().size();i++)
    {
        __Push(cell.GetTriangles()[i]);
    }

}

template<>
void Dumper::Pop(real_t &f)
{
    f = *(real_t*)(s_data.data() + s_pop_index);
    s_pop_index += sizeof(real_t);
}

template<>
void Dumper::Pop(int &i)
{
    i = *(int*)(s_data.data() + s_pop_index);
    s_pop_index += sizeof(int);
}

template<>
void Dumper::Pop(size_t &i)
{
    i = *(size_t*)(s_data.data() + s_pop_index);
    s_pop_index += sizeof(size_t);
}


template<>
void Dumper::Pop(Triangle &tri)
{

    std::array<vec3,3> pos;

    for(int i=0;i<3;i++)
    {
        Pop(pos[i].x);
        Pop(pos[i].y);
        Pop(pos[i].z);
    }

    tri.SetPos(pos);
}

template<>
void Dumper::Pop<Ray>(Ray &ray)
{
    vec3 s(0),e(0);

    Pop(s.x);
    Pop(s.y);
    Pop(s.z);

    Pop(e.x);
    Pop(e.y);
    Pop(e.z);

    ray.SetStart(s);
    ray.SetEnd(e);
}

template<>
void Dumper::Pop<GridCell>(GridCell &cell)
{
    vec3 min(0),max(0);

    Pop(min.x);
    Pop(min.y);
    Pop(min.z);

    Pop(max.x);
    Pop(max.y);
    Pop(max.z);

    AABB3D bbox;

    bbox.SetMax(max);
    bbox.SetMin(min);
   
    cell = GridCell(bbox);

    size_t size=0;

    Pop(size);

    for(int i=0;i<size;i++)
    {
        size_t t=0;
        Pop(t);
        cell.AddTriangle(t);
    }
}




}