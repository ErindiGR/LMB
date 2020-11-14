#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <thread>

#include "lmb/lmb.h"
#include "lmb/triangle.h"
#include "lmb/ray.h"
#include "lmb/dumper.h"

#include "lmb_ui/imgui/imgui.h"
#include "lmb_ui/imgui/imgui_impl_sdl.h"
#include "lmb_ui/imgui/imgui_impl_opengl2.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>


void DrawCube(const glm::vec3 &min,const glm::vec3 &max)
{

glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
// White side - BACK
glBegin(GL_POLYGON);

glVertex3f(  max.x, min.y, max.z );
glVertex3f(  max.x,  max.y, max.z );
glVertex3f( min.x,  max.y, max.z );
glVertex3f( min.x, min.y, max.z );
glEnd();

// Purple side - RIGHT
glBegin(GL_POLYGON);
glVertex3f( max.x, min.y, min.z);
glVertex3f( max.x, max.y, min.z );
glVertex3f( max.x, max.y,  max.z );
glVertex3f( max.x, min.y,  max.z );
glEnd();

// Green side - LEFT
glBegin(GL_POLYGON);
glVertex3f( min.x,  min.y,  max.z );
glVertex3f( min.x,  max.y,  max.z );
glVertex3f( min.x,  max.y, min.z );
glVertex3f( min.x,  min.y, min.z );
glEnd();

// Blue side - TOP
glBegin(GL_POLYGON);
glVertex3f( max.x,  max.y,  max.z );
glVertex3f( max.x,  max.y, min.z );
glVertex3f( min.x,  max.y, min.z );
glVertex3f( min.x,  max.y,  max.z );
glEnd();

// Red side - BOTTOM
glBegin(GL_POLYGON);
glVertex3f(  max.x,  min.y, min.z );
glVertex3f(  max.x,  min.y,  max.z);
glVertex3f( min.x,  min.y,  max.z);
glVertex3f( min.x,  min.y, min.z );
glEnd();

glPolygonMode(GL_FRONT,GL_FILL);
}


SDL_Window* window;
SDL_GLContext gl_context;
bool quit=false;

int main()
{

    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("LMB UI",0,0,800,600,SDL_WINDOW_OPENGL);
    gl_context = SDL_GL_CreateContext(window);

    //SDL_GL_MakeCurrent(window,NULL);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    auto info = LMB::Dumper::Load("dump.dp");
    std::vector<glm::vec3> grid_color(info->m_grid.size());

    for(size_t i=0;i<info->m_grid.size();i++)
        grid_color[i] = glm::vec3(rand()%255/255.0f,rand()%255/255.0f,rand()%255/255.0f);

    printf("count %lu .\n",info->m_rays.size());

    glm::vec3 pos(0,0,-1);

    float rot_y = 0;
    float rot_x=0;
    bool rays = false;
    bool grid = false;
    bool triangle_aabb = false;

    int start = 0;
    int end = 10;
    int inverse_density = 10;

    glEnable(GL_DEPTH_TEST);

    while(!quit)
    {

        SDL_Event e;

        while(SDL_PollEvent(&e)){

            ImGui_ImplSDL2_ProcessEvent(&e);
            if(e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(window))
                quit = true;
        }

        glm::mat4 lookat = glm::lookAt(
            pos * glm::quat(glm::radians(rot_x),glm::vec3(1,0,0))* glm::quat(glm::radians(rot_y),glm::vec3(0,1,0)),
            glm::vec3(0),
            glm::vec3(0,1,0)
        );

        glm::mat4 persp = glm::perspective(
            glm::radians(90.0f),
            1.0f,
            0.1f,
            100.0f
        );

        glm::mat4 pv = persp * lookat;


        glClearColor(0.2f,0.2f,0.2f,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glMatrixMode(GL_MODELVIEW_MATRIX);
        glLoadMatrixf(glm::value_ptr(pv));


        if(!info)
        {
            break;
        }

        inverse_density = glm::max(inverse_density,1);

        if(rays)
        {

            glBegin(GL_LINES);
            for(size_t i=start;i<info->m_rays.size() && i<end;i+=inverse_density)
            {
                glColor3f(info->m_rays[i].GetLength()/8.0f,0,0);
                glVertex3f(
                    info->m_rays[i].GetStart().x,
                    info->m_rays[i].GetStart().y,
                    info->m_rays[i].GetStart().z
                );

                glVertex3f(
                    info->m_rays[i].GetEnd().x,
                    info->m_rays[i].GetEnd().y,
                    info->m_rays[i].GetEnd().z
                );
            }
            glEnd();
        }


        glPointSize(4.0f);
        glBegin(GL_POINTS);
            for(size_t i=start;i<info->m_rays.size() && i<end;i+=inverse_density)
            {
                glColor3f(0,0,1);

                glVertex3f(
                    info->m_rays[i].GetEnd().x,
                    info->m_rays[i].GetEnd().y,
                    info->m_rays[i].GetEnd().z
                );
            }
        glEnd();


        
        glBegin(GL_TRIANGLES);
        for(size_t i=0;i<info->m_grid.size();i++)
        {
            glColor3f(grid_color[i].x,grid_color[i].y,grid_color[i].z);
            for(size_t j=0;j<info->m_grid[i].GetTriangles().size();j++)
            {   
                size_t ti = info->m_grid[i].GetTriangles()[j];
                for(size_t t=0;t<info->m_triangles[ti].GetPos().size();t++)
                {
                    glVertex3f(
                        info->m_triangles[ti].GetPos()[t].x,
                        info->m_triangles[ti].GetPos()[t].y,
                        info->m_triangles[ti].GetPos()[t].z
                    );
                }
            }
        }
        glEnd();

        if(triangle_aabb)
        {
            for(size_t i=0;i<info->m_grid.size();i++)
            {
                glColor3f(1-grid_color[i].x,1-grid_color[i].y,1-grid_color[i].z);
                for(size_t j=0;j<info->m_grid[i].GetTriangles().size();j++)
                {   
                    size_t ti = info->m_grid[i].GetTriangles()[j];
                    
                    DrawCube(info->m_triangles[ti].GetAABB().GetMin(),
                        info->m_triangles[ti].GetAABB().GetMax());
                }
            }
               
        }

        if(grid)
        {
            for(size_t i=0;i<info->m_grid.size();i++)
                DrawCube(info->m_grid[i].GetAABB().GetMin(),info->m_grid[i].GetAABB().GetMax());
        }
        

        glFlush();
        

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();


        if(ImGui::Begin("Hello, world!")){                       // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::DragFloat("rot x",&rot_x);
            ImGui::DragFloat("rot y",&rot_y);
            ImGui::DragFloat("dist",&pos.z);

            ImGui::DragInt("start",&start);
            ImGui::DragInt("end",&end);

            ImGui::DragInt("density",&inverse_density);

            if(ImGui::Button("rays"))
                rays = !rays;

            if(ImGui::Button("grid"))
                grid = !grid;

            if(ImGui::Button("triangle_aabb"))
                triangle_aabb = !triangle_aabb;

            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        //glClearColor(1,1,1,1);
        //glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);

        //SDL_GL_MakeCurrent(window,NULL);
    }

        // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();


    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}