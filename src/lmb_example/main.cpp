#include <lmb/lmb.h>
#include <lmb/solvers/grid_solver.h>
#include <lmb/solvers/default_solver.h>
#include <lmb/calculators/ao_calculator.h>
#include <lmb/calculators/direct_light_calculator.h>
#include <lmb/calculators/denois_calculator.h>
#include <lmb/calculators/padding_calculator.h>
#include <lmb/dumper.h>

using namespace LMB;

#include "obj_loader.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

int main()
{
	LMB::Init();

	std::shared_ptr<LMBSession> lmb = std::make_shared<LMBSession>();

	objl::Loader obj_loader;

	obj_loader.LoadFile("scene2_high.obj");

	for(size_t j=0;j<obj_loader.LoadedMeshes.size();j++)
	{

		auto lightmap = lmb->AddLightmap(256,256);

		for(size_t i =0;i<obj_loader.LoadedMeshes[j].Indices.size();i+=3)
		{
			auto va = obj_loader.LoadedMeshes[j].Vertices[obj_loader.LoadedMeshes[j].Indices[i]].Position;
			auto vb = obj_loader.LoadedMeshes[j].Vertices[obj_loader.LoadedMeshes[j].Indices[i+1]].Position;
			auto vc = obj_loader.LoadedMeshes[j].Vertices[obj_loader.LoadedMeshes[j].Indices[i+2]].Position;

			auto ta = obj_loader.LoadedMeshes[j].Vertices[obj_loader.LoadedMeshes[j].Indices[i]].TextureCoordinate;
			auto tb = obj_loader.LoadedMeshes[j].Vertices[obj_loader.LoadedMeshes[j].Indices[i+1]].TextureCoordinate;
			auto tc = obj_loader.LoadedMeshes[j].Vertices[obj_loader.LoadedMeshes[j].Indices[i+2]].TextureCoordinate;

		
			Triangle tri;

			std::array<vec3,3> pos =
			{
				vec3(va.X,va.Y,va.Z),
				vec3(vb.X,vb.Y,vb.Z),
				vec3(vc.X,vc.Y,vc.Z)
			};

			tri.SetPos(pos);

			std::array<vec2,3> uv =
			{
				vec2(ta.X,ta.Y),
				vec2(tb.X,tb.Y),
				vec2(tc.X,tc.Y)
			};


			tri.SetUV2(uv);

			tri.SetLightmap(lightmap);

			lmb->AddTriangle(tri);
		}
	}


	auto solver = std::shared_ptr<Solver>(new GridSolver(8));
	lmb->SetSolver(solver);

	
	auto t0 = std::chrono::high_resolution_clock::now();

#if 1
	auto calc = std::shared_ptr<Calculator>(new DirectLightCalculator(default_dl_config));
	calc->SetBlend(std::make_shared<CalcBlendSet>());

	
	Light dir_light(Light::Type::Directional);
	dir_light.SetDir(glm::vec3(1,1,1));
	dir_light.SetSoftness(0.3f);
	((DirectLightCalculator*)calc.get())->AddLight(dir_light);

	
	Light point_light(Light::Type::Point);
	point_light.SetPos(glm::vec3(-1,0.5,0));
	point_light.SetColor(glm::vec3(1,0,0));
	((DirectLightCalculator*)calc.get())->AddLight(point_light);

	lmb->SetCalculator(calc);
	printf("Calculation started\n");
	lmb->StartCalc();
	lmb->EndCalc();
#endif
	

#if 1
	auto aocalc = std::shared_ptr<Calculator>(new AOCalculator(default_ao_config));
	aocalc->SetBlend(std::make_shared<CalcBlendMul>());
	lmb->SetCalculator(aocalc);
	printf("Calculation AO started\n");
	lmb->StartCalc();
	lmb->EndCalc();
#endif

#if 1
	auto denois = std::shared_ptr<Calculator>(new DenoisCalculator(8,0.1f));
	lmb->SetCalculator(denois);
	printf("Calculation denois started\n");
	lmb->StartCalc();
	lmb->EndCalc();
#endif

#if 1
	auto padding = std::shared_ptr<Calculator>(new PaddingCalculator());
	lmb->SetCalculator(padding);
	printf("Calculation Padding started\n");
	lmb->StartCalc();
	lmb->EndCalc();
#endif

	auto t1 = std::chrono::high_resolution_clock::now();

	printf(
		"Time: %dmin %ds .\n",
		std::chrono::duration_cast<std::chrono::minutes>(t1-t0),
		std::chrono::duration_cast<std::chrono::seconds>(t1-t0) % 60
	);


	for(size_t i=0;i<lmb->GetLightmaps().size();i++)
	{
		auto res = BitmapUtil::ToRGBAFloat(lmb->GetLightmaps()[i]->GetColor());

		char filename[32];

		sprintf(filename,"lightmap_%lu.hdr",i);

		stbi_write_hdr(filename, res.GetWidth(), res.GetHeight(),4,(float*)res.GetData());
	}


	LMB::Term();

	return 1;
}


