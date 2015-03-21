#version 430 core

in vec4 vPositionWorld;
in vec4 vNormalWorld;

out vec4 Output;

void main()
{
    vec3 normal = normalize(vNormalWorld.xyz);
    normal = normal*0.5 + 0.5;

    Output = vec4(normal, 1.0);
}