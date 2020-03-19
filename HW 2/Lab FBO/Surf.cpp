#include <vector>
#include <glm/glm.hpp>
#include "Surf.h"
#include "include\GL\Surf.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
//#include "Lab FBO.cpp"

static const int N = 50;
static const int NUM_VERTICES = N * N;

//The sinc surface
static glm::vec3 surf(int i, int j)
{
    const float center = 0.5f * N;
    const float xy_scale = 20.0f / N;
    const float z_scale = 10.0f;

    float x = xy_scale * (i - center);
    float y = xy_scale * (j - center);

    float r = sqrt(x * x + y * y);
    float z = 1.0f;

    if (r != 0.0f)
    {
        z = sin(r) / r;
    }
    z = z * z_scale;

    return 0.05f * glm::vec3(x, y, z);
}

//Sinc surface normals
static glm::vec3 normal(int i, int j)
{
    glm::vec3 du = surf(i + 1, j) - surf(i - 1, j);
    glm::vec3 dv = surf(i, j + 1) - surf(i, j - 1);
    return glm::normalize(glm::cross(du, dv));//these are normal vectors


}


GLuint create_surf_vbo()
{
    //Declare a vector to hold N*N vertices
    std::vector<glm::vec3> surf_verts;
    surf_verts.reserve(NUM_VERTICES);

    std::vector<glm::vec3> surf_norm;//normal vectors
    surf_norm.reserve(NUM_VERTICES);

    std::vector<glm::vec2> surf_tex;//normal vectors
    surf_tex.reserve(NUM_VERTICES);

    std::vector<glm::vec3> data;
   
       for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
             
                data.push_back(surf(i, j));//so these are position vectors
                data.push_back(normal(i, j));//normal vectors


            }
        }

       GLuint vbo;
       glGenBuffers(1, &vbo); //Generate vbo to hold vertex attributes for triangle.
       glBindBuffer(GL_ARRAY_BUFFER, vbo); //Specify the buffer where vertex attribute data is stored.

       glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), data.data(), GL_STATIC_DRAW);


 
    return vbo;
}

std::vector<glm::vec4> getColor() {
    std::vector<glm::vec4> colors;
    colors.push_back(glm::vec4(1, 0.1, 0, 1));
    colors.push_back(glm::vec4(0, 0.5, 0.2, 1));
    colors.push_back(glm::vec4(0.6, 0.5, 0, 1));
    colors.push_back(glm::vec4(0.8, 0.8, 0, 1));
    colors.push_back(glm::vec4(0.8, 0, 0.2, 1));
    colors.push_back(glm::vec4(0.1, 0, 0.9, 1));
    colors.push_back(glm::vec4(0, 0.5, 0, 1));
    colors.push_back(glm::vec4(0.3, 0, 0.1, 1));
    colors.push_back(glm::vec4(0.3, 0.9, 0.1, 1));
    return colors;
}


GLuint create_instance_vbo() {


    std::vector<glm::vec4> surf_col=getColor();
   
    GLuint vbo;
    glGenBuffers(1, &vbo); //Generate vbo to hold vertex attributes for triangle.
    glBindBuffer(GL_ARRAY_BUFFER, vbo); //Specify the buffer where vertex attribute data is stored.



    //Upload from main memory to gpu memory.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * surf_col.size(), surf_col.data(), GL_STATIC_DRAW);//all
    //specify per instance



    //Tell opengl how to get the attribute values out of the vbo (stride and offset).
    //In this case, the vertices are at the beginning of the VBO and are tightly packed.
    int col_loc = 6;
    glEnableVertexAttribArray(col_loc);//color
    glVertexAttribPointer(col_loc, 4, GL_FLOAT, false, 4 * sizeof(float), 0);////interleaved check stride+pointer
    glVertexAttribDivisor(col_loc, 1);

    return vbo;
}

std::vector<glm::mat4> getCoordinates() {
    std::vector<glm::mat4> coordinates;

    //column one
   coordinates.push_back(glm::translate(glm::vec3(-1.0f, 1.0f, 0.0f)));
    coordinates.push_back(glm::translate(glm::vec3(-1.0f, 0.0f, 0.0f)));
    coordinates.push_back(glm::translate(glm::vec3(-1.0f, -1.0f, 0.0f)));

    //column two
    coordinates.push_back(glm::translate(glm::vec3(0.0f, 1.0f, 0.0f)));
    coordinates.push_back(glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)));
    coordinates.push_back(glm::translate(glm::vec3(0.0f, -1.0f, 0.0f)));

    //column three
    coordinates.push_back(glm::translate(glm::vec3(1.0f, 1.0f, 0.0f)));
    coordinates.push_back(glm::translate(glm::vec3(1.0f, 0.0f, 0.0f)));
    coordinates.push_back(glm::translate(glm::vec3(1.0f, -1.0f, 0.0f)));



    return coordinates;
}

GLuint create_transform_vbo() {

    std::vector<glm::mat4> mat_surf = getCoordinates();

    GLuint vbo;
    glGenBuffers(1, &vbo); 
    glBindBuffer(GL_ARRAY_BUFFER, vbo); 

                                        
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mat_surf.size(), mat_surf.data(), GL_STATIC_DRAW);

    int mat_loc = 8;//location


    int mat_loc1 = mat_loc + 0;
    int mat_loc2 = mat_loc + 1;
    int mat_loc3 = mat_loc + 2;
    int mat_loc4 = mat_loc + 3;

    glEnableVertexAttribArray(mat_loc1);
    glEnableVertexAttribArray(mat_loc2);
    glEnableVertexAttribArray(mat_loc3);
    glEnableVertexAttribArray(mat_loc4);


    glVertexAttribPointer(mat_loc1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 0));
    glVertexAttribPointer(mat_loc2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 1));
    glVertexAttribPointer(mat_loc3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 2));
    glVertexAttribPointer(mat_loc4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 3));

    glVertexAttribDivisor(mat_loc1, 1);
    glVertexAttribDivisor(mat_loc2, 1);
    glVertexAttribDivisor(mat_loc3, 1);
    glVertexAttribDivisor(mat_loc4, 1);
   


    return vbo;
}


GLuint create_surf_ebo() {
    //ebo stotes indixes
    GLuint ebo;//buffer for this one//////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<unsigned short int>indices;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); //Specify
    


    for (int i = 0; i < N-1; i++) {////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (int j = 0; j < N; j++) {
            indices.push_back(j+i*N);
            indices.push_back(j+N+i*N);
         
        }
        indices.push_back(0xFFFF);
    }


    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short int)*indices.size(), indices.data(), GL_STATIC_DRAW);


    return ebo;
}
/*GLuint create_vbo_iterations(std::vector<float> iterations) {//need to store the instances here

    std::vector<float> surf_iterations = iterations;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);


    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * surf_iterations.size(), surf_iterations.data(), GL_STATIC_DRAW);

    int iterations_loc = 4;//iterations location

    glEnableVertexAttribArray(iterations_loc);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(iterations_loc, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(iterations_loc, 1);

    return vbo;
}*/

GLuint create_surf_vao()//put iterations here
{
    GLuint vao;

    //std::vector<float> surf_iterations = iterations;

  
    //Generate vao id to hold the mapping from attrib variables in shader to memory locations in vbo
    glGenVertexArrays(1, &vao);

    //Binding vao means that bindbuffer, enablevertexattribarray and vertexattribpointer state will be remembered by vao
    glBindVertexArray(vao);

    GLuint ebo = create_surf_ebo();////////////////////////////////////////////////////////////////////////////////////////////////////////ebo&VBO
    GLuint vbo = create_surf_vbo();
    
  
    int pos_loc = 0;//pos_loc is at 0
    int norm_loc = 1;//morm_attrib is at 1
    
 
    glEnableVertexAttribArray(pos_loc); //Enable the position attribute.
    glEnableVertexAttribArray(norm_loc); //Enable normal
    

    const float stride = 3 * sizeof(float) + 3 * sizeof(float);
    glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, stride, (void*)0);////interleaved
    glVertexAttribPointer(norm_loc, 3, GL_FLOAT, false, stride,(void*) (3 * sizeof(float)));
  
    GLuint vbo3= create_transform_vbo();//matrix
    GLuint vbo4 = create_instance_vbo();///colors
    //GLuint vbo5= create_vbo_iterations(surf_iterations);//iterations for each index

    glBindVertexArray(0); //unbind the vao
    return vao;  
}

//Draw the set of points on the surface
void draw_surf(GLuint vao)
{
     glBindVertexArray(vao);
  
     glEnable(GL_PRIMITIVE_RESTART);///primitive restart
     glPrimitiveRestartIndex(0xFFFF);
     glDrawElementsInstanced(GL_TRIANGLE_STRIP, 49 * 50 * 2 + 50, GL_UNSIGNED_SHORT, 0,9);////////////////////
     

}











