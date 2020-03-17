#version 330

in vec3 in_Position;
in vec2 in_TexCoord;
in vec3 in_Normal;

out vec2 ex_TexCoord;
out vec3 ex_Normal;
out vec3 ex_FragPos;

//out vec3 ex_View;
//out vec3 ex_Light;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 ViewPosition;
uniform vec3 LightPosition;
//uniform mat4 MatNorm;

void main()
{
	ex_TexCoord = in_TexCoord;
	ex_Normal = mat3(transpose(inverse(Model))) * in_Normal;
	ex_FragPos = vec3(Model * vec4(in_Position, 1.0f));

	gl_Position = Projection * View * Model * vec4(in_Position, 1.0);
}