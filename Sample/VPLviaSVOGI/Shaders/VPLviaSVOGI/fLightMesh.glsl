#version 430 core

in vec2 pTCoord;

out vec4 Output;

uniform sampler2D LightMeshTexture;

void main()
{
    vec4 color = texture(LightMeshTexture, pTCoord);
    Output = color;
}