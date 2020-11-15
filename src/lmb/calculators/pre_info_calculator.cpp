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
    PreInfoCalculator* calc)
: LightmapChunkJob(x_start,y_start,x_end,y_end,calc->m_lightmap)
, m_calc(calc)
{
    
}

void PreInfoJob::CalculatePixel(const bitmap_size_t x,const bitmap_size_t y)
{
    auto &triangles = m_calc->m_lmb->GetTriangles();

    //(x+0.5) sample the center of the pixel
    real_t xf = (x + to_real(0.5)) / (real_t)m_lightmap->GetPos().GetWidth();
	real_t yf = (y + to_real(0.5)) / (real_t)m_lightmap->GetPos().GetHeight();

    vec3 uvw(0);
	vec3 pos(0);
    vec3 norm(0);

	for(size_t i=0;i<triangles.size();i++)
	{
		const Triangle& tri = triangles[i];

		if(!tri.GetLightmap())
			continue;
		else if(tri.GetLightmap() != m_lightmap->GetIndex())
			continue;

        if(tri.HasUV2())
        {
            if(tri.PointInsideUV2(vec2(xf,yf),uvw))
            {
                pos = tri.BarycentricToPos(uvw);
                norm = tri.BarycentricToNormal(uvw);

                break;
            }
        }
        else
        {
            if(tri.PointInsideUV1(vec2(xf,yf),uvw))
            {
                pos = tri.BarycentricToPos(uvw);
                norm = tri.BarycentricToNormal(uvw);

                break;
            }
        }
	}

	m_lightmap->GetPos().SetPixel(x,y,pos);
    m_lightmap->GetNorm().SetPixel(x,y,norm);
}

void PreInfoCalculator::StartCalc()
{
    DEBUG_LOG("Started Calculating pre info .\n");

    bitmap_size_t chunk_size = std::max((bitmap_size_t)(m_lightmap->GetColor().GetWidth()/(JobManager::Get()->GetNumThreads()*2)),1);

    for(bitmap_size_t x=0;x<m_lightmap->GetColor().GetWidth()/chunk_size;x++)
    {
        for(bitmap_size_t y=0;y<m_lightmap->GetColor().GetHeight()/chunk_size;y++)
        {
            std::shared_ptr<PreInfoJob> job = std::make_shared<PreInfoJob>(
                x*chunk_size,
                y*chunk_size,
                (x+1)*chunk_size,
                (y+1)*chunk_size,
                this
            );

            m_jobs.push_back(job);
            JobManager::Get()->Push(std::static_pointer_cast<Job>(job));
        }
    }
}



}