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

	return ret;

}




const size_t LMBSession::AddLightmap(const size_t width, const size_t height)
{
	const size_t index = m_lightmaps.size();
	
	std::shared_ptr<Lightmap> ret = std::make_shared<Lightmap>(width,height,index);

	m_lightmaps.push_back(ret);
	
	m_changes.lightmaps = true;

	return index;
}





void LMBSession::Calculate()
{
	StartCalc();
	EndCalc();
}




void LMBSession::StartCalc()
{
	if(m_changes.lightmaps || m_changes.triangles)
	{
		for(size_t i=0;i<m_lightmaps.size();i++)
		{
			m_pre_info_calc->SetLightmap(m_lightmaps[i]);
			m_pre_info_calc->StartCalc();
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
		m_changes.triangles = false;
	}

	if(GetCalculator())
	{
		m_pre_info_calc->EndCalc();

		m_changes.calculator = false;

		for(size_t i=0;i<m_lightmaps.size();i++)
		{
			m_calc->SetLightmap(m_lightmaps[i]);
			m_calc->StartCalc();
		}
	}
}




void LMBSession::EndCalc()
{

	if(m_calc)
	{
		m_calc->EndCalc();
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
	
}




void Term()
{

}



}