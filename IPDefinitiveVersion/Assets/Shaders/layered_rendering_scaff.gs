#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

#define LAYER_COUNT 6
uniform mat4 u_layer_matrices[LAYER_COUNT];

out vec4 gsout;

void main()
{
    for(int l = 0; l < LAYER_COUNT; ++l)
    {
        gl_Layer = l;
        for(int i = 0; i < 3; ++i)
        {
            vec4 inpos = gl_in[i].gl_Position;
            gl_Position = u_layer_matrices[l] * inpos;
			//set outputs to whatever you need in fragment shader
			gsout = inpos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  