#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    // for(int face = 0; face < 6; ++face)
    // {
    //     if(face == 0 || face == 1 || face == 4 || face == 5)
    //     {
    //         gl_Layer = face; // built-in variable that specifies to which face we render.
    //         for(int i = 0; i < 3; ++i) // for each triangle's vertices
    //         {
    //             FragPos = gl_in[i].gl_Position;
    //             gl_Position = shadowMatrices[face] * FragPos;
    //             EmitVertex();
    //         }    
    //         EndPrimitive();
    //     }
    gl_Layer = 0; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) // for each triangle's vertices
    {
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices[0] * FragPos;
        EmitVertex();
    }    
    EndPrimitive();
    // }

    gl_Layer = 1; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) // for each triangle's vertices
    {
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices[1] * FragPos;
        EmitVertex();
    }    
    EndPrimitive();

    gl_Layer = 4; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) // for each triangle's vertices
    {
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices[4] * FragPos;
        EmitVertex();
    }    
    EndPrimitive();

    gl_Layer = 5; // built-in variable that specifies to which face we render.
    for(int i = 0; i < 3; ++i) // for each triangle's vertices
    {
        FragPos = gl_in[i].gl_Position;
        gl_Position = shadowMatrices[5] * FragPos;
        EmitVertex();
    }    
    EndPrimitive();
}  