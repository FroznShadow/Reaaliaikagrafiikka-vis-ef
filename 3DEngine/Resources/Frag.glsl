#version 330

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;
precision mediump float;

#define saturate(x) clamp(x, 0.0, 1.0);

in vec2 ex_TexCoord;
in vec3 ex_Normal;
in vec3 ex_FragPos;

out vec4 gl_FragColor;

uniform sampler2D tex;
uniform vec3 ViewPosition;
uniform vec3 LightPosition;

void main()
{
	vec3 lightColor = vec3(1.0,1.0,1.0);
	float ambientStrenght = 0.5f;
	vec3 ambient = material.ambient * vec3(1.0f, 1.0f, 1.0f);

	vec3 norm = normalize(ex_Normal);
	vec3 lightDir = normalize(LightPosition - ex_FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * material.diffuse) * lightColor;

	float specularStrength = 0.5f;
	vec3 viewDir = normalize(ViewPosition - ex_FragPos);
	vec3 reflectionDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectionDir), 0.0), 32);
	vec3 specular = material.specular * spec * lightColor;

	gl_FragColor = texture(tex, ex_TexCoord);
	gl_FragColor = vec4((ambient + diffuse + specular) * gl_FragColor.xyz, 1.0f);
}