#include "lmb/calculators/pre_info_calculator.h"

#include "lmb/debug.h"
#include "lmb/lmb.h"


namespace LMB
{

PreInfoJob::PreInfoJob( 
    const bitmap_size_t x_start,
    const bitmap_size_t y_start,
    const bitmap_size_t x_end,
    const bitmap_size_t y_end,
    const size_t lightmap,
    PreInfoCalculator* calc)
: LightmapChunkJob(x_start,y_start,x_end,y_end,lightmap,calc)
{   
    m_interpolate = false;
}

void PreInfoJob::CalculatePixel(const bitmap_size_t x,const bitmap_size_t y)
{
    auto &temp_lightmap =  m_calc->GetTempLightmapColor(m_lightmap);

    PreInfoCalculator* calc = static_cast<PreInfoCalculator*>(m_calc);

    size_t cell_index = 0;

    if(!calc->GetCell(vec2(to_real(x) / temp_lightmap->GetWidth(),
        to_real(y) / temp_lightmap->GetHeight()),cell_index))
        return;

    const auto &triangles = m_calc->GetLMB()->GetTriangles();
    auto &triangle_indices = calc->m_grid[cell_index].triangle_indexes;
    auto &triangles_info = m_calc->GetLMB()->GetTrianglesInfo();
    auto &lightmap =  m_calc->GetRealLightmap(m_lightmap);
    


    const vec2 pixel_pos = GetPixelCenter(x,y);

    const vec2 pixel_tl = GetPixelPoint(x,y,1,0);
    const vec2 pixel_tr = GetPixelPoint(x,y,1,1);
    const vec2 pixel_bl = GetPixelPoint(x,y,0,0);
    const vec2 pixel_br = GetPixelPoint(x,y,0,1);

    vec3 uvw(0);
	vec3 pos(0);
    vec3 norm(0);
    Lightmap::EFlags used = Lightmap::EFlags::UnUsed;

	for(size_t i=0;i<triangle_indices.size();i++)
	{
		const Triangle& tri = triangles[triangle_indices[i]];

		if(triangles_info[tri.GetInfo()].GetLightmap().GetIndex() != lightmap->GetIndex())
			continue;
            
        if(
            tri.PointInsideLightmapUV(pixel_pos,uvw)||
            tri.PointInsideLightmapUV(pixel_tl,uvw) ||
            tri.PointInsideLightmapUV(pixel_tr,uvw) ||
            tri.PointInsideLightmapUV(pixel_bl,uvw) ||
            tri.PointInsideLightmapUV(pixel_br,uvw))
        {
            pos = tri.BarycentricToPos(uvw);
            norm = tri.BarycentricToNormal(uvw);
            used = Lightmap::EFlags::Used;
            break;
        }
	}

	lightmap->GetPos().SetPixel(x,y,pos);
    lightmap->GetNorm().SetPixel(x,y,norm);
    lightmap->GetFlags().SetPixel(x,y,used);
}

void PreInfoCalculator::GenerateGrid()
{
    auto &triangles = GetLMB()->GetTriangles();

    const size_t grid_size = 32;

    const real_t cell_size = to_real(1) / to_real(grid_size);

    for(size_t y=0;y<grid_size;y++)
    {
        for(size_t x=0;x<grid_size;x++)
        {
            AABB2D bbox;
            bbox.SetMin(vec2(y*cell_size,x*cell_size));
            bbox.SetMax(vec2((y+1)*cell_size,(x+1)*cell_size));

            std::vector<size_t> triangle_indexes;

            for(size_t i=0;i<triangles.size();i++)
            {
                if(Intersect2AABB2D(bbox,triangles[i].GetLightmapAABBUV()))
                {
                    triangle_indexes.push_back(i);
                }
            }

            if(triangle_indexes.size() > 0)
            {
                PIGridCell cell;
                cell.bbox = bbox;
                cell.triangle_indexes = std::move(triangle_indexes);
                m_grid.push_back(cell);
            }
            
        }
    }
}

const bool PreInfoCalculator::GetCell(const vec2 &point,size_t &out_cell_index)
{
    for(size_t i=0;i<m_grid.size();i++)
    {
        if(PointInsideBox(point,m_grid[i].bbox))
        {
            out_cell_index = i;
            return true;
        }
    }

    return false;
}

void PreInfoCalculator::StartCalc(const size_t lightmap)
{
    Calculator::StartCalc(lightmap);

    GenerateGrid();

    CalcLightmapHelper calc_helper(lightmap,this);
    
    DEBUG_LOG("Started Calculating pre info .\n");

    bitmap_size_t chunk_size = std::max((bitmap_size_t)(calc_helper.GetLightmapWidth()/(JobManager::GetNumThreads()*2)),1);

    for(bitmap_size_t y=0;y<calc_helper.GetLightmapHeight()/chunk_size;y++)
    {
        for(bitmap_size_t x=0;x<calc_helper.GetLightmapWidth()/chunk_size;x++)
        {
            std::shared_ptr<PreInfoJob> job = std::make_shared<PreInfoJob>(
                x*chunk_size,
                y*chunk_size,
                (x+1)*chunk_size,
                (y+1)*chunk_size,
                lightmap,
                this);

            m_jobs.push_back(job);
            JobManager::Push(std::static_pointer_cast<Job>(job));
        }
    }
}



}