#version 330

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoord;

out vec4 fcolor;

uniform vec3 i_a;          // ambient color
uniform vec3 i_l;          // lighting color
uniform vec3 l;            // light position
uniform vec3 v;            // viewers position
uniform vec3 am_material;   // ambient material color
uniform vec3 di_material;   // diffuse material color
uniform vec3 spec_material; // specular material color
uniform int shininess;      // shininess exponent

uniform bool useTexture;
uniform sampler2D ourTexture;


void main()
{
    vec3 normals = normalize(fragNormal);
    // take away fragPosition so that light has fixed position
    vec3 light_position = normalize(l - fragPosition);
    vec3 viewers_position = normalize(v - fragPosition);
    vec3 r = reflect(-light_position, normals);

    vec3 ambient = i_a * am_material;
    vec3 diffuse = max(dot(normals, light_position), 0) * i_l * di_material;
    vec3 specular = max(dot(diffuse, vec3(1.0)), 0.0f) * pow(max(dot(r, viewers_position), 0.0), shininess) * i_l * spec_material;

    vec4 textureColor = useTexture ? texture(ourTexture, fragTexCoord) : vec4(1.0, 1.0, 1.0, 1.0);

    fcolor = vec4(ambient + diffuse + specular, 1.0) * textureColor;
}
