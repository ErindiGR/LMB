#include <lmb/lmb.h>

#include "lmb/debug.h"
#include "lmb/dumper.h"
#include "lmb/calculators/ao_calculator.h"
#include "lmb/solvers/default_solver.h"


namespace LMB
{



const size_t LMBSession::AddTriangle(const Triangle& tri)
{
	const size_t ret = m_triangles.size();
	m_triangles.push_back(tri);

	m_changes.triangles = true;

	Dump_Push(Triangle,tri);

	return ret;

}

const size_t LMBSession::AddTriangleInfo(const TriangleInfo&tri_info)
{
	const size_t ret = m_triangles_info.size();
	m_triangles_info.push_back(tri_info);

	m_changes.triangles = true;

	return ret;
}


const LightmapHandle LMBSession::AddLightmap(const size_t size)
{
	const size_t index = m_lightmaps.size();
	
	std::shared_ptr<Lightmap> lm = std::make_shared<Lightmap>(size,index);

	m_lightmaps.push_back(lm);
	
	m_changes.lightmaps = true;

	auto ret = LightmapHandle(index);

	ret.SetLMB(this);

	return ret;
}

const BitmapHandle LMBSession::AddBitmap(const size_t width, const size_t height)
{
	const size_t index = m_bitmaps.size();
	
	std::shared_ptr<Bitmap<vec4>> lm = std::make_shared<Bitmap<vec4>>(width,height);

	m_bitmaps.push_back(lm);

	auto ret = BitmapHandle(index);

	ret.SetLMB(this);

	return ret;
}

const BitmapHandle LMBSession::AddBitmap(const std::shared_ptr<Bitmap<vec4>>& bitmap)
{
	const size_t index = m_bitmaps.size();

	m_bitmaps.push_back(bitmap);

	auto ret = BitmapHandle(index);

	ret.SetLMB(this);

	return ret;
}

void LMBSession::Calculate()
{
	StartCalc();
	EndCalc();
}




void LMBSession::StartCalc()
{
	for(size_t i=0;i<m_lightmaps.size();i++)
	{
		StartCalc(i);
	}
}

void LMBSession::StartCalc(const size_t lightmap)
{
	if(m_changes.lightmaps || m_changes.triangles)
	{
		m_pre_info_calc->SetLightmaps(m_lightmaps);
		
		for(size_t i=0;i<m_lightmaps.size();i++)
		{
			m_pre_info_calc->StartCalc(i);
		}
		
		m_changes.lightmaps = false;
	}

	if( GetSolver() && (m_changes.solver || m_changes.triangles))
	{
		m_solver->Gen();
		m_changes.solver = false;
	}

	if(m_changes.triangles)
	{
		Dump_Term(Triangle);
		m_changes.triangles = false;
	}

	if(GetCalculator())
	{
		m_pre_info_calc->EndCalc();
		
		m_changes.calculator = false;

		m_calc->SetLightmaps(m_lightmaps);
		m_calc->StartCalc(lightmap);
	}
}




void LMBSession::EndCalc()
{

	if(m_calc)
	{
		m_calc->EndCalc();

		for(size_t i=0;i<m_lightmaps.size();i++)
		{
			m_calc->ApplyResultTempColor(i);
		}
	}
}




const Solver * LMBSession::GetSolver()
{
	if(!m_solver)
		m_solver = std::make_shared<DefaultSolver>();

	return m_solver.get();
}




const Calculator * LMBSession::GetCalculator()
{
	if(!m_calc)
		m_calc = std::make_shared<AOCalculator>(default_ao_config);

	return m_calc.get();
}





void Init()
{
	JobManager::Init();
}




void Term()
{
	JobManager::Term();
}



}