#version 400

uniform sampler2D texture;//first texture
uniform sampler2D pickTexture;//second texture

uniform int pass;
uniform int blur;

uniform vec4 col;
uniform int edgeDetection;

out vec4 fragcolor;           
in vec2 tex_coord;//tex coordinate

flat in int instancedID;

layout(location=0) out vec4 color;//fragcolor
layout(location=1)out vec4 pick;//data;identifier
uniform int pickID;//the ID picked

 
      
void main(void)
{   
	if(pass == 1)
	{
	   
	    //fragcolor=vec4(1.0/255.0);
		//fragcolor = texture2D(texture, tex_coord);
	     pick=vec4(vec3(instancedID/255.0f),1.0f);//either 0 to 11 or 225

	 if(instancedID==pickID){
	      fragcolor = texture2D(pickTexture, tex_coord);//so make sure ID matches
	      }
	 else{
	      fragcolor = texture2D(texture, tex_coord);
	    }
		
	
	}
	else if(pass == 2)
	{

	    if(edgeDetection==0){
	   
	   fragcolor = texture2D(texture, tex_coord);
	  
	                         }
	     else if(edgeDetection==1){

      vec4 left=texelFetch(texture,ivec2(gl_FragCoord.xy)+ivec2(-1,0),0);//texelFetch(sample,ivec2(gl_FragCoord)
	  vec4 right=texelFetch(texture,ivec2(gl_FragCoord.xy)+ivec2(+1,0),0);

	  vec4 above=texelFetch(texture,ivec2(gl_FragCoord.xy)+ivec2(0,+1),0);//texelFetch(sample,ivec2(gl_FragCoord)
	  vec4 below=texelFetch(texture,ivec2(gl_FragCoord.xy)+ivec2(0,-1),0);

	  fragcolor=pow(above-below,vec4(2.0))+pow(left-right,vec4(2.0));
	                               }
	     else{
			fragcolor=texture2D(texture,tex_coord);
			}
	
   }
 	else
	{
		fragcolor = vec4(1.0, 0.0, 1.0, 1.0); //error
	}

	
}




















