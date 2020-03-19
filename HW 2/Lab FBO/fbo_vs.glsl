#version 400            

uniform int pass;
uniform mat4 PVM;


layout(location=0) in vec3 pos_attrib;//pos_loc
layout(location=1) in vec3 normal_attrib;//normal
layout(location=2) in vec4 color;


in vec2 tex_coord_attrib;
out vec2 tex_coord;  

flat out int instancedID;//instance id
layout(location=4)in float iteration;//texture index 
layout(location=8) in mat4 model_matrix;


void main(void)
{

	if(pass == 1)
	{
		
		tex_coord = tex_coord_attrib;
        gl_Position = PVM*model_matrix*vec4(pos_attrib, 1.0);// in the future

		instancedID=gl_InstanceID;//get the instances
		
	}
	else
	{
		gl_Position = vec4(pos_attrib, 1.0);
        tex_coord = 0.5*pos_attrib.xy + vec2(0.5);
	}

	
	 
}
