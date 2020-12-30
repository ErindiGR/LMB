#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <thread>
#include <vector>

#include <lmb/solvers/kdtree_solver.h>

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


template<typename T>
std::vector<T> Load(const char *filename)
{
    std::vector<T> data;

    FILE* file = fopen(filename,"rb");

    fseek(file,0,SEEK_END);
    size_t count = ftell(file)/sizeof(T);
    fseek(file,0,SEEK_SET);

    data.resize(count);

    fread(data.data(),sizeof(T),count,file);
    fclose(file);

    return std::move(data);
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

    std::vector<LMB::SAABB3DAndDepth> grid = Load<LMB::SAABB3DAndDepth>("dump_""SAABB3DAndDepth"".bin");
    std::vector<LMB::Triangle> triangles = Load<LMB::Triangle>("dump_""Triangle"".bin");
    


    printf("count %lu .\n",grid.size());

    glm::vec3 pos(0,0,-1);

    float rot_y = 0;
    float rot_x=0;
    int depth_lvl = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


        glMatrixMode(GL_MODELVIEW_MATRIX);
        glLoadMatrixf(glm::value_ptr(pv));


        glColor3f(0.4,0.4,0.4);
        glBegin(GL_TRIANGLES);
        for(size_t i=0;i<triangles.size();i++)
        {
            for(int j=0;j<3;j++)
            {
                glVertex3fv(&triangles[i].GetPos()[j].x);
            }
        }
        glEnd();

        glColor3f(1.0f,0,0);
        for(size_t i=0;i<grid.size();i++)
        {
            if(depth_lvl == grid[i].depth)
            DrawCube(grid[i].bbox.GetMin(),grid[i].bbox.GetMax());
        }


        

        //glFlush();
        

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();


        if(ImGui::Begin("Hello, world!")){                       // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::DragFloat("rot x",&rot_x);
            ImGui::DragFloat("rot y",&rot_y);
            ImGui::DragFloat("dist",&pos.z);
            ImGui::DragInt("depth",&depth_lvl);

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