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

	struct SChangeFlags
	{
		bool triangles:1;
		bool lightmaps:1;
		bool solver:1;
		bool calculator:1;
	};

public:

	LMBSession()
	:m_pre_info_calc(std::make_shared<PreInfoCalculator>())
	{
		m_pre_info_calc->SetLMB(this);
	}

    void Calculate();

    void StartCalc();

	void StartCalc(const size_t lightmap) override;

    void EndCalc() override;

	const size_t AddTriangle(const Triangle& tri);

	const LightmapHandle AddLightmap(const size_t size);

	const BitmapHandle AddBitmap(const size_t width, const size_t height);
	const BitmapHandle AddBitmap(const std::shared_ptr<Bitmap<vec4>>& bitmap);

	const size_t AddTriangleInfo(const TriangleInfo&tri_info);

	inline void SetSolver(std::shared_ptr<Solver> &solver)
	{
		EndCalc();

		if(solver->HasLMB())
			return;

		m_solver = std::move(solver);
		m_solver->SetLMB(this);
		m_changes.solver=true;
	}

	/**
	 * @brief sets the lmb calculator
	 * 
	 * @param calculator the calculator (the shared pointer is moved)
	 */
	inline void SetCalculator(const std::shared_ptr<Calculator> &calculator)
	{
		EndCalc();

		if(calculator->HasLMB())
			return;

		m_calc = calculator;
		m_calc->SetLMB(this);
		m_changes.calculator = true;
	}

	const Solver *GetSolver();

	const Calculator *GetCalculator();

	inline const std::vector<Triangle>& GetTriangles() const
	{
		return m_triangles;
	}

	inline const std::vector<TriangleInfo>& GetTrianglesInfo() const
	{
		return m_triangles_info;
	}

	inline std::vector<std::shared_ptr<Lightmap>>& GetLightmaps()
	{
		return m_lightmaps;
	}

	inline std::shared_ptr<Lightmap> GetLightmap(size_t index)
	{
		if(m_lightmaps.size() > index)
			return m_lightmaps[index];
		
		return nullptr;
	}

	inline std::shared_ptr<Bitmap<vec4>> GetBitmap(size_t index)
	{
		if(m_bitmaps.size() > index)
			return m_bitmaps[index];
		
		return nullptr;
	}



protected:

	std::shared_ptr<PreInfoCalculator> 		m_pre_info_calc;
	std::shared_ptr<Calculator> 			m_calc;
	std::shared_ptr<Solver> 				m_solver;
	std::vector<Triangle> 			m_triangles;
	std::vector<TriangleInfo> 		m_triangles_info;
	std::vector<std::shared_ptr<Lightmap>> 		m_lightmaps;
	std::vector<std::shared_ptr<Bitmap<vec4>>> 	m_bitmaps;
	SChangeFlags 				m_changes;

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