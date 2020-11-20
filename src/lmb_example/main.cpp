#include <lmb/lmb.h>
#include <lmb/solvers/grid_solver.h>
#include <lmb/solvers/default_solver.h>
#include <lmb/solvers/kdtree_solver.h>
#include <lmb/calculators/ao_calculator.h>
#include <lmb/calculators/il_calculator.h>
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

	obj_loader.LoadFile("scene2.obj");

	for(size_t j=0;j<obj_loader.LoadedMeshes.size();j++)
	{

		size_t lightmap = lmb->AddLightmap(128,128);

		for(size_t i =0;i<obj_loader.LoadedMeshes[j].Indices.size();i+=3)
		{
			auto &mesh = obj_loader.LoadedMeshes[j];

			const size_t index0 = mesh.Indices[i];
			const size_t index1 = mesh.Indices[i + 1];
			const size_t index2 = mesh.Indices[i + 2];

			auto va = mesh.Vertices[index0].Position;
			auto vb = mesh.Vertices[index1].Position;
			auto vc = mesh.Vertices[index2].Position;

			auto na = mesh.Vertices[index0].Normal;
			auto nb = mesh.Vertices[index1].Normal;
			auto nc = mesh.Vertices[index2].Normal;

			auto ta = mesh.Vertices[index0].TextureCoordinate;
			auto tb = mesh.Vertices[index1].TextureCoordinate;
			auto tc = mesh.Vertices[index2].TextureCoordinate;

		
			Triangle tri;

			const std::array<vec3,3> pos =
			{
				vec3(va.X,va.Y,va.Z),
				vec3(vb.X,vb.Y,vb.Z),
				vec3(vc.X,vc.Y,vc.Z)
			};

			tri.SetPos(pos);

			const std::array<vec3,3> norm =
			{
				vec3(na.X,na.Y,na.Z),
				vec3(nb.X,nb.Y,nb.Z),
				vec3(nc.X,nc.Y,nc.Z)
			};

			tri.SetNormal(norm);

			const std::array<vec2,3> uv =
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


	auto solver = std::shared_ptr<Solver>(new KDTreeSolver());
	//auto solver = std::shared_ptr<Solver>(new GridSolver(8));
	lmb->SetSolver(solver);

	
	auto t0 = std::chrono::high_resolution_clock::now();

#if 1
	auto calc = std::shared_ptr<Calculator>(new DirectLightCalculator(default_dl_config));
	calc->SetBlend(std::make_shared<CalcBlendAdd>());

#if 1
	Light dir_light(Light::EType::Directional);
	dir_light.SetDir(glm::normalize(vec3(1,1,-1)));
	dir_light.SetSoftness(0.00);
	dir_light.SetColor(vec3(1));
	((DirectLightCalculator*)calc.get())->AddLight(dir_light);
#endif

#if 0
	Light point_light(Light::EType::Point);
	point_light.SetPos(vec3(-1,0.5,0));
	point_light.SetColor(vec3(1,0,0));
	point_light.SetSoftness(0.1);
	((DirectLightCalculator*)calc.get())->AddLight(point_light);
#endif

	lmb->SetCalculator(calc);
	printf("Calculation DL started\n");
	lmb->StartCalc();
	lmb->EndCalc();
#endif

#if 1

	auto gicalc = std::shared_ptr<Calculator>(new IndirectLightCalculator(default_il_config));
	gicalc->SetBlend(std::make_shared<CalcBlendAdd>());
	lmb->SetCalculator(gicalc);
	printf("Calculation IL started\n");
	lmb->StartCalc();
	lmb->EndCalc();

#endif
	
#if 0
	auto aocalc = std::shared_ptr<Calculator>(new AOCalculator(default_ao_config));
	aocalc->SetBlend(std::make_shared<CalcBlendMul>());
	lmb->SetCalculator(aocalc);
	printf("Calculation AO started\n");
	lmb->StartCalc();
	lmb->EndCalc();
#endif

#if 1
	auto denois = std::shared_ptr<Calculator>(new DenoisCalculator(4,0.8));
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

	printf("Time: %dmin %ds .\n",
		std::chrono::duration_cast<std::chrono::minutes>(t1-t0),
		std::chrono::duration_cast<std::chrono::seconds>(t1-t0) % 60);


	for(size_t i=0;i<lmb->GetLightmaps().size();i++)
	{
		auto res = BitmapUtils::ToRGBAFloat(lmb->GetLightmaps()[i]->GetColor());
		//auto res = (lmb->GetLightmaps()[i]->GetNorm());

		char filename[32];

		sprintf(filename,"lightmap_%lu.png",i);

		stbi_write_hdr(filename, res.GetWidth(), res.GetHeight(),4,(float*)res.GetData());
	}


	LMB::Term();

	return 1;
}


