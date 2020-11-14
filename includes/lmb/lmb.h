#pragma once


#include "triangle.h"
#include "bitmap.h"
#include "lightmap.h"
#include "solver.h"
#include "icalculable.h"
#include "calculator.h"
#include "calculators/pre_info_calculator.h"

#include <vector>
#include <memory>


namespace LMB
{

class LMBSession : public ICalculable
{


public:

	LMBSession()
	{
		m_pre_info_calc = std::make_shared<PreInfoCalculator>();
		m_pre_info_calc->SetLmb(this);
	}

    void Calculate();

    void StartCalc();

    void EndCalc();

	const size_t AddTriangle(const Triangle& tri);

	const size_t AddLightmap(const size_t width, const size_t height);


	inline void SetSolver(std::shared_ptr<Solver> &solver)
	{

		EndCalc();

		m_solver = std::move(solver);
		m_solver->SetLmb(this);
		m_changes.solver=true;
	}

	/**
	 * @brief sets the lmb calculator
	 * 
	 * @param calculator the calculator (the shared pointer is moved)
	 */
	inline void SetCalculator(std::shared_ptr<Calculator> &calculator)
	{
		EndCalc();

		m_calc = std::move(calculator);
		m_calc->SetLmb(this);
		m_changes.calculator=true;
	}

	const Solver *GetSolver();

	const Calculator *GetCalculator();

	inline const std::vector<Triangle>& GetTriangles() const
	{
		return m_triangles;
	}

	inline std::vector<std::shared_ptr<Lightmap>>& GetLightmaps()
	{
		return m_lightmaps;
	}



protected:

	std::shared_ptr<PreInfoCalculator> m_pre_info_calc;
	std::shared_ptr<Calculator> m_calc;
	std::shared_ptr<Solver> 	m_solver;

	std::vector<Triangle> m_triangles;
	std::vector<std::shared_ptr<Lightmap>> m_lightmaps;

	struct change_flags_s
	{
		bool triangles:1;
		bool lightmaps:1;
		bool solver:1;
		bool calculator:1;
	};

	change_flags_s m_changes;

};

/**
 * @brief Initiate Initiates the library
 * 
 */
void Init();

/**
 * @brief Terminate Terminates the library
 * 
 */
void Term();


}