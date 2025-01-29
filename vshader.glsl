#version 330

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 fragTexCoord;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main()
{
    fragNormal = normalize((M * vec4(vNormal, 0.0)).xyz);
    fragPosition = (M * vec4(vPosition, 1.0)).xyz;
    fragTexCoord = vTexCoord;
    gl_Position = P * V * M * vec4(vPosition, 1.0);
}
