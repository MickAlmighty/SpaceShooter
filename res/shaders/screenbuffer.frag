#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 1000.0;  

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // lewy górny
        vec2( 0.0f,    offset), // górny środek
        vec2( offset,  offset), // prawy górny
        vec2(-offset,  0.0f),   // lewy po środku
        vec2( 0.0f,    0.0f),   // środkowy
        vec2( offset,  0.0f),   // prawy po środku
        vec2(-offset, -offset), // lewy dolny
        vec2( 0.0f,   -offset), // dolny środkowy
        vec2( offset, -offset)  // prawy dolny  
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec2 textureReversal = TexCoords;
    //odwracanie textury w osi x 
    //vec2 textureReversal = vec2(1,1);
    // textureReversal.x = 1 - TexCoords.x;
    // textureReversal.y = TexCoords.y;
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, textureReversal + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);
}