#include <windows.h>

#include <GL/glew.h>

#include <GL/freeglut.h>

#include <GL/gl.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include "InitShader.h"
#include "imgui_impl_glut.h"
#include "LoadMesh.h"
#include "LoadTexture.h"
#include "Surf.h"

static const std::string vertex_shader("fbo_vs.glsl");
static const std::string fragment_shader("fbo_fs.glsl");

float angle = -1.5f;

unsigned int vao = -1;/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint shader_program = -1;//pass 1
GLuint texture_id = -1; //Texture map for fish
GLuint pick_id =-1;//texture map for pick texture

GLuint quad_vao = -1;//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GLuint quad_vbo = -1;//pass2

GLuint fbo_id = -1;// Framebuffer object,
GLuint rbo_id = -1;// and Renderbuffer (for depth buffering)
GLuint fbo_texture = -1;// Texture rendered into.
static const int Num = 9;

int edgeDetection = 0;
//std::vector<float> iterations = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0,8.0};//iterations

static const std::string mesh_name = "Amago0.obj";
static const std::string pick_name = "Rainbow.jpg";
static const std::string texture_name ="AmagoT.bmp";

MeshData mesh_data;
float time_sec = 0.0f;
bool check_framebuffer_status();

GLuint pick_texture = -1;
int pickID=-1;


//Draw the user interface using ImGui
void draw_gui()
{
    ImGui_ImplGlut_NewFrame();
     //ImGui::Image((void*)texture_id, ImVec2(128, 128));
     ImGui::Image((void*)pick_texture, ImVec2(128, 128));//pick texture

    static bool checked = false;

    ImGui::Checkbox("Edge Detection", &checked);///////////////////////////////////////////////////
    if (checked == true) {
        edgeDetection = 1;
    }
    else{
        edgeDetection = 0;
    }
       
    //create a slider to change the angle variables
    //ImGui::SliderFloat("View angle", &angle, -9.141592f, +9.141592f);
    
    ImGui::ShowDemoWindow();
    ImGui::Render();
}


void draw_pass_1()
{
   const int pass = 1;
   
   glm::mat4 P = glm::perspective(90.0f, 1.0f, 0.1f, 10.0f);
   glm::mat4 V = glm::lookAt(glm::vec3(1.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   glm::mat4 M = glm::rotate(angle, glm::vec3(1.0f,1.0f, 1.0f));////change rotation



   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture_id);
   glActiveTexture(GL_TEXTURE0 + 1);
   glBindTexture(GL_TEXTURE_2D, pick_id);

   int pass_loc = glGetUniformLocation(shader_program, "pass");
   if(pass_loc != -1)
   {
      glUniform1i(pass_loc, pass);
   }

   int PVM_loc = glGetUniformLocation(shader_program, "PVM");
   if(PVM_loc != -1)
   {
      glm::mat4 PVM = P * V * M;
      glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   int tex_loc = glGetUniformLocation(shader_program, "texture");
   if(tex_loc != -1)
   {
      glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
   }


   int textwo_loc = glGetUniformLocation(shader_program, "pickTexture");
   if (tex_loc != -1)
   {
       glUniform1i(textwo_loc, 1);
   }
 
 ///////////////////////////////////////need this
   int pick_loc = glGetUniformLocation(shader_program, "pickID");
   if (pick_loc != -1)
   {
       glUniform1i(pick_loc, pickID);
   }
   
   //GLuint data = create_surf_vao(iterations);
   GLuint data = create_surf_vao();
  
    glBindVertexArray(data);
    glDrawElementsInstanced(GL_TRIANGLE_STRIP, 49 * 50 * 2 + 50, GL_UNSIGNED_SHORT, 0, Num);
 
  

}


void draw_pass_2()
{
   const int pass = 2;
   int pass_loc = glGetUniformLocation(shader_program, "pass");
   if(pass_loc != -1)
   {
      glUniform1i(pass_loc, pass);
   }

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, fbo_texture);

   int tex_loc = glGetUniformLocation(shader_program, "texture");
   if(tex_loc != -1)
   {
      glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
   }
 
  int edge_loc = glGetUniformLocation(shader_program, "edgeDetection");////////////////////////////////////////////////////////////////////////////////////
  if (edge_loc != -1)
  {
     glUniform1i(edge_loc,edgeDetection); //////////////////////////////////////////////////////////////////////////////////////
  }

    glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0,4);/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}


// glut display callback function.
// This function gets called every time the scene gets redisplayed 
void display()
{

   glUseProgram(shader_program);

   glBindFramebuffer(GL_FRAMEBUFFER, fbo_id); // Render to FBO.

   //Out variable in frag shader will be written to the texture attached to GL_COLOR_ATTACHMENT0 or 1.
   GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
   glDrawBuffers(2, drawBuffers);


   //Make the viewport match the FBO texture size.
   int w, h;
   glBindTexture(GL_TEXTURE_2D, fbo_texture);
   glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
   glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
   glViewport(0, 0, w, h);


   //Clear the FBO.
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Lab assignment: don't forget to also clear depth
   
  draw_pass_1();
 
     
   glBindFramebuffer(GL_FRAMEBUFFER, 0); // Do not render the next pass to FBO.
   glDrawBuffer(GL_BACK); // Render to back buffer.

   //const int wh = glutGet(GLUT_SCREEN_WIDTH);///////////////////////////////////////////////////////////////////////////////////////////////FOR FIRST PASS
   //const int hh = glutGet(GLUT_SCREEN_HEIGHT);

   const int wh = glutGet(GLUT_SCREEN_WIDTH);///////////////////////////////////////////////////////////////////////////////////////////////FOR FIRST PASS
   const int hh = glutGet(GLUT_SCREEN_HEIGHT);

   glViewport(0, 0, wh, hh); //Render to the full viewport
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the back buffer
   draw_pass_2();


 
   draw_gui();//////////////////////////////////////////////////////////////////////////////////////////////////////////PUT THIS HERE IMGUI
   glutSwapBuffers();
}



void idle()
{
	glutPostRedisplay();
    int col_loc = glGetUniformLocation(shader_program, "col");//////////////////////////////////////////////////////////////////////////////////////////////
    

   const int time_ms = glutGet(GLUT_ELAPSED_TIME);
   time_sec = 0.001f*time_ms;

   
   
   glUniform4f(col_loc, sin(time_sec), 0.0f, 0.0f, 1.0f);

   
}

void reload_shader()
{
   GLuint new_shader = InitShader(vertex_shader.c_str(), fragment_shader.c_str());
   
  
   if(new_shader == -1) // loading failed
   {
      glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
   }
   else
   {

        glClearColor(0.35f, 0.35f, 0.35f, 0.0f);
      //glClearColor(1.0f,0.0f, 0.0f, 0.0f);////////////////////////////white
       

      if(shader_program != -1)
      {
         glDeleteProgram(shader_program);
      }
      shader_program = new_shader;

      if(mesh_data.mVao != -1)
      {
         BufferIndexedVerts(mesh_data);
      }
   }
}


// glut keyboard callback function.
// This function gets called when an ASCII key is pressed
void keyboard(unsigned char key, int x, int y)
{
   std::cout << "key : " << key << ", x: " << x << ", y: " << y << std::endl;

   switch(key)
   {
      case 'r':
      case 'R':
         reload_shader();     
      break;
   }

   
}

void reshape(int w, int h) {///////////////////////////////////////////////////////////////////////////////////////////////////////////////RESIZE
   std::cout << w << "," << h << std::endl;
  }

void printGlInfo()
{
   std::cout << "Vendor: "       << glGetString(GL_VENDOR)                    << std::endl;
   std::cout << "Renderer: "     << glGetString(GL_RENDERER)                  << std::endl;
   std::cout << "Version: "      << glGetString(GL_VERSION)                   << std::endl;
   std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)  << std::endl;
}

void initOpenGl()//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
   glewInit();
  
  glEnable(GL_DEPTH_TEST);

   glEnable(GL_PROGRAM_POINT_SIZE);/////////////////////////////////
   //vao = create_surf_vao(iterations);
    vao = create_surf_vao();

    reload_shader();///////////////////////////////////////////////////////////////////////////////////////////////////

   //mesh and texture for pass 1
   mesh_data = LoadMesh(mesh_name);
   texture_id = LoadTexture(texture_name.c_str());//texture 1
   pick_id = LoadTexture(pick_name.c_str());//texture 2

   //mesh for pass 2 (full screen quadrilateral)///////////////////////////////////////////////////////////////////////////////////////////////////////////////
   glGenVertexArrays(1, &quad_vao);
   glBindVertexArray(quad_vao);

   float vertices[] = {1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
       -1.0f, 1.0f, 0.0f,
       -1.0f, -1.0f, 0.0f};

   //create vertex buffers for vertex coords
   glGenBuffers(1, &quad_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   int pos_loc = glGetAttribLocation(shader_program, "pos_attrib");
   int screen_loc = glGetAttribLocation(shader_program, "screenTexture");//////////////////////////////////////////////
   if(pos_loc >= 0)
   {
       glEnableVertexAttribArray(pos_loc);
	   glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, 0, 0);
   }


   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////RESIZE
   const int w = glutGet(GLUT_SCREEN_WIDTH);
   const int h= glutGet(GLUT_SCREEN_HEIGHT);
   
   //pick texture writing from the shader to it
   //Create texture to render pass 1 into.
   //Lab assignment: make the texture width and height be the window width and height.
   glGenTextures(1, &fbo_texture);
   glBindTexture(GL_TEXTURE_2D, fbo_texture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glBindTexture(GL_TEXTURE_2D, 0);   

   glGenTextures(1, &pick_texture);
   glBindTexture(GL_TEXTURE_2D, pick_texture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glBindTexture(GL_TEXTURE_2D, 0);

   //Lab assignment: Create renderbuffer for depth.
   glGenRenderbuffers(1, &rbo_id);///////////////////////////////////////////////////////
   glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
  

   //Create the framebuffer object
   glGenFramebuffers(1, &fbo_id);
   glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);////////////////////////////////////////////////////////
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pick_texture, 0);////////////////////////////////////////////////////////change 0  to 1

   //Lab assignment: attach depth renderbuffer to FBO
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_id);

   check_framebuffer_status();

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void motion(int x, int y)//pass input to imgui
{
    ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y)
{
    ImGui_ImplGlut_MouseButtonCallback(button, state);

    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
    {
        GLubyte buffer[4];
        
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(x, glutGet(GLUT_WINDOW_HEIGHT) - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        pickID = (int)buffer[0];
       
        if (pickID< Num && pickID>=0) {
            std::cout << "Id: " << pickID << std::endl;//try this one to get the ID
        }

        //std::cout<<"Picked"<<int(buffer[0])<<"  "<<int(buffer[1])<<" "<< int(buffer[2])<< " " << int(buffer[3])<< std::endl;

    }
  
}


int main (int argc, char **argv)
{
   //Configure initial window state
   glutInit(&argc, argv); 
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowPosition (5, 5);
   glutInitWindowSize (1280, 720);
   int win = glutCreateWindow ("Lab FBO solution");

   printGlInfo();

   //Register callback functions with glut. 
   glutDisplayFunc(display); 
   glutKeyboardFunc(keyboard);
   glutIdleFunc(idle);
   glutReshapeFunc(reshape);//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////RESIZE

   //Register callback functions with glut. 
   //glutSpecialFunc(special);//so when special key gets pressed stuff happen
   //glutKeyboardUpFunc(keyboard_up);
   //glutSpecialUpFunc(special_up);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutPassiveMotionFunc(motion);

   initOpenGl();
   ImGui_ImplGlut_Init();///////////////////////////////////////////////////////////////////////////////////////////////NEED THIS IMGUI

   //Enter the glut event loop.
   glutMainLoop();
   glutDestroyWindow(win);
   return 0;		
}

bool check_framebuffer_status() 
{
    GLenum status;
    status = (GLenum) glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE:
            return true;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			printf("Framebuffer incomplete, incomplete attachment\n");
            return false;
        case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("Unsupported framebuffer format\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			printf("Framebuffer incomplete, missing attachment\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			printf("Framebuffer incomplete, missing draw buffer\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			printf("Framebuffer incomplete, missing read buffer\n");
            return false;
    }
	return false;
}


