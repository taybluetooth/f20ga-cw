#version 430 core

out vec4 color;

in VS_OUT
{
    vec2 tc;
    vec4 normals;
    vec4 fragPos;
} fs_in;

layout(binding=0) uniform sampler2D tex;
layout(binding=1) uniform sampler2D white;
layout(binding=2) uniform sampler2D metallic;
layout(binding=3) uniform sampler2D wood;

uniform mat4 model_matrix;

uniform vec4 lightPosition;
uniform vec4 viewPosition;

uniform vec4 ia;			// Ambient colour
uniform float ka;			// Ambient constant
uniform vec4 id;			// Diffuse colour
uniform float kd;			// Diffuse constant
uniform vec4 is;			// Specular colour
uniform float ks;			// Specular constant
uniform float shininess;	// Shininess constant

void main(void){

	// Diffuse
	vec4 lightDir = normalize(lightPosition - fs_in.fragPos);
	float diff = max(dot(normalize(fs_in.normals), lightDir), 0.0);
	vec4 diffuse = diff * id;

	// Specular
	vec4 viewDir = normalize(viewPosition - fs_in.fragPos);
	vec4 reflectDir = reflect(-lightDir, normalize(fs_in.normals));
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	// Light
	color = vec4(ka * ia.rgb + kd * id.rgb * diffuse.rgb + ks * is.rgb * spec, 1.0) * texture(tex, fs_in.tc) * texture(white, fs_in.tc) * texture(metallic, fs_in.tc) * texture(wood, fs_in.tc);

}

