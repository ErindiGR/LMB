#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>

#ifdef DRAW

SDL_Window* g_window;
SDL_GLContext g_glcontext;


void InitDisplay()
{
	SDL_Init(SDL_INIT_EVERYTHING);

    g_window = SDL_CreateWindow("LMB Example",0,0,400,400,SDL_WINDOW_OPENGL);
    g_glcontext = SDL_GL_CreateContext(g_window);
}

std::vector<GLuint> g_lightmaps;

void BindLigthmaps(const std::shared_ptr<LMB::LMBSession> &lmb)
{
	auto &lightmaps = lmb->GetLightmaps();

	if(g_lightmaps.size() != lightmaps.size())
	{
		g_lightmaps.resize(lightmaps.size(),0);
	}
	else
	{
		glDeleteTextures(lightmaps.size(),g_lightmaps.data());
	}

	glGenTextures(lightmaps.size(),g_lightmaps.data());

	auto calc = lmb->GetCalculator();
	
	for(size_t i=0;i< lightmaps.size();i++)
	{

		glBindTexture(GL_TEXTURE_2D,g_lightmaps[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
			lightmaps[i]->GetColor().GetWidth(),
			lightmaps[i]->GetColor().GetHeight(),
			0,GL_RGBA,GL_FLOAT,
			calc->GetTempLightmapColor(i)->GetData());
		
		glBindTexture(GL_TEXTURE_2D,0);
	}

	
}

void DrawWorld(const std::shared_ptr<LMB::LMBSession> &lmb)
{

	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
	}

	BindLigthmaps(lmb);

	auto &triangles = lmb->GetTriangles();
	auto &triangles_info = lmb->GetTrianglesInfo();

	glm::mat4 lookat = glm::lookAt(
        glm::vec3(5,5,5),
        glm::vec3(0),
        glm::vec3(0,1,0));

    glm::mat4 persp = glm::perspective(
        glm::radians(90.0f),
        1.0f,
        0.1f,
        100.0f);

    glm::mat4 pv = persp * lookat;

	glEnable(GL_FRAMEBUFFER_SRGB); 
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.2f,0.2f,0.2f,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    glMatrixMode(GL_MODELVIEW_MATRIX);
    glLoadMatrixf(glm::value_ptr(pv));

	//glActiveTexture(GL_TEXTURE0);
	//size_t lightmap = triangles_info[triangles[i].GetInfo()].GetLightmap();
	glBindTexture(GL_TEXTURE_2D, g_lightmaps[0]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);
	for(size_t i=0;i< triangles.size();i++)
	{
		for(size_t j=0;j<3;j++)
		{
			glTexCoord2fv(&triangles[i].GetLightmapUV()[j].x);
			glVertex3fv(&triangles[i].GetPos()[j].x);
		}
	}
	glEnd();

	SDL_GL_SwapWindow(g_window);
}

void TermDisplay()
{
	SDL_GL_DeleteContext(g_glcontext);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

void DrawCalcResult(std::shared_ptr<LMBSession> lmb)
{
    while(1)
	{
		DrawWorld(lmb);

		if(lmb->GetCalculator()->GetProgress().GetVal() >= 100)
			break;
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

#else

#define InitDisplay()
#define BindLigthmaps(x)
#define DrawWorld(x)
#define TermDisplay()
#define DrawCalcResult(x)

#endif