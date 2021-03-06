#include <lmb/lmb.h>
#include <lmb/solvers/grid_solver.h>
#include <lmb/solvers/default_solver.h>
#include <lmb/solvers/kdtree_solver.h>
#include <lmb/calculators/ao_calculator.h>
#include <lmb/calculators/il_calculator.h>
#include <lmb/calculators/direct_light_calculator.h>
#include <lmb/calculators/denois_calculator.h>
#include <lmb/calculators/padding_calculator.h>

using namespace LMB;



#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


#include "obj_loader.h"
#include "display.h"


#define OBJ_FILE "scene2.1.obj"
#define TEXTURE_ALBEDO "scene2.1_texture.png"
#define TEXTURE_EMISSIVE "scene2.1_emissive.png"



std::shared_ptr<Bitmap<vec4>> LoadBitmap(const char * filename)
{
	int texture_width = 0;
	int texture_height = 0;
	int texture_channels = 0;
	const int required_num_channels = 4;

	stbi_uc* texture_data = stbi_load(
		filename,
		&texture_width,
		&texture_height,
		&texture_channels,
		required_num_channels);

	Bitmap<RGBA8> texture(texture_width,texture_height);

	std::memcpy(
		texture.GetData(),
		texture_data,
		texture_width*texture_height*texture_channels);

	std::shared_ptr<Bitmap<vec4>> ret = 
		std::make_shared<Bitmap<vec4>>(BitmapUtils::ToVec4(texture));
	
	BitmapUtils::FlipVertically(*ret.get());

	return ret;
}


int main()
{
	InitDisplay();

	//Init LMB
	LMB::Init();

	//Create LMB Session
	std::shared_ptr<LMBSession> lmb = std::make_shared<LMBSession>();


	//Load .obj file
	objl::Loader obj_loader;

	obj_loader.LoadFile(OBJ_FILE);


	//Load textures
	auto texture_albedo = lmb->AddBitmap(LoadBitmap(TEXTURE_ALBEDO));
	auto texture_emissive = lmb->AddBitmap(LoadBitmap(TEXTURE_EMISSIVE));

	//Multiply emission texture 
	BitmapUtils::Multiply(*(texture_emissive.Get().get()),6);
	

	for(size_t j=0;j<obj_loader.LoadedMeshes.size();j++)
	{
		//Create lightmap
		LightmapHandle lightmap = lmb->AddLightmap(256);

		//Create triangle info
		TriangleInfo triangle_info;

		triangle_info.SetLightmap(lightmap);
		triangle_info.SetAlbedo(texture_albedo);
		triangle_info.SetEmissive(texture_emissive);

		//Add TriangleInfo to LMBSession
		size_t info = lmb->AddTriangleInfo(triangle_info);

		//Create Triangle and add it to LMBSession
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

			tri.SetInfo(info);

			lmb->AddTriangle(tri);
		}
	}


	//Create Solver 

#if 1
	auto solver = std::shared_ptr<Solver>(new KDTreeSolver());
#elif 1
	auto solver = std::shared_ptr<Solver>(new GridSolver(8));
#else
	auto solver = std::shared_ptr<Solver>(new DefaultSolver());
#endif	

	//Assign Solver to LMBSession
	lmb->SetSolver(solver);

	//Get Start Time
	auto t0 = std::chrono::high_resolution_clock::now();
	
#if 1
	//Create direct lighting calculator
	auto calc = std::shared_ptr<DirectLightCalculator>(new DirectLightCalculator(default_dl_config));
	//Set the calculator blend mode default is BlendSet
	calc->SetBlend(std::make_shared<CalcBlendAdd>());

#if 0
	//Create a directional light and add it to the calculator
	Light dir_light(Light::EType::Directional);
	dir_light.SetDir(glm::normalize(vec3(1,1,1)));
	dir_light.SetSoftness(0.1);
	dir_light.SetColor(vec3(1));
	calc->AddLight(dir_light);
#endif

#if 0
	//Create a point light and add it to the calculator
	Light point_light(Light::EType::Point);
	point_light.SetPos(vec3(-1,0.0,0));
	point_light.SetColor(vec3(1,0,0));
	point_light.SetSoftness(0.05);
	calc->AddLight(point_light);
#endif

	lmb->SetCalculator(std::static_pointer_cast<Calculator>(calc));
	printf("Calculation DL started\n");
	lmb->StartCalc();
	DrawCalcResult(lmb);
	lmb->EndCalc();
#endif

#if 1
	auto denoise = std::shared_ptr<Calculator>(new DenoiseCalculator(64,0.08));
	lmb->SetCalculator(denoise);
	printf("Calculation denoise started\n");
	lmb->StartCalc();
	lmb->EndCalc();
#endif


#if 1
	auto gi_calc = std::shared_ptr<Calculator>(new IndirectLightCalculator(default_il_config));
	gi_calc->SetBlend(std::make_shared<CalcBlendAdd>());
	lmb->SetCalculator(gi_calc);
	printf("Calculation IL started\n");
	lmb->StartCalc();
	DrawCalcResult(lmb);
	lmb->EndCalc();
#endif
	
#if 1
	auto ao_calc = std::shared_ptr<Calculator>(new AOCalculator(default_ao_config));
	ao_calc->SetBlend(std::make_shared<CalcBlendMul>());
	lmb->SetCalculator(ao_calc);
	printf("Calculation AO started\n");
	lmb->StartCalc();
	DrawCalcResult(lmb);
	lmb->EndCalc();
#endif

#if 1
	auto denoise2 = std::shared_ptr<Calculator>(new DenoiseCalculator(16,0.06));
	lmb->SetCalculator(denoise2);
	printf("Calculation denoise started\n");
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

	//Get End Time
	auto t2 = std::chrono::high_resolution_clock::now();

	//Print Calculation Duration Time
	printf("Time: %dmin %ds .\n",
		std::chrono::duration_cast<std::chrono::minutes>(t2-t0),
		std::chrono::duration_cast<std::chrono::seconds>(t2-t0) % 60);


	//Write lightmaps
	for(size_t i=0;i<lmb->GetLightmaps().size();i++)
	{
		
		auto res = BitmapUtils::ToRGBAFloat(lmb->GetLightmaps()[i]->GetColor());
		BitmapUtils::FlipVertically(res);

		char filename[32];

		sprintf(filename,"lightmap_%lu.png",i);

		stbi_write_hdr(filename, res.GetWidth(), res.GetHeight(),4,(float*)res.GetData());
	}


	//Terminate LMB
	LMB::Term();

	TermDisplay();

	return 1;
}


