#ifndef Engine_ShaderProgram_hpp
#define Engine_ShaderProgram_hpp

//#include <glew.h>

#include <glad/glad.h>
#include <Core/ResourceManager.h>
#include <Core/Components/Shader.hpp>


#include <lib/Helper.hpp>

namespace Engine
{

	class ShaderProgram
	{
	public:
		ShaderProgram() { _programID = glCreateProgram(); };
		~ShaderProgram() {};

		void CompileShader(const char* source, GLenum shaderType);
		void checkCompileErrors(GLuint shader, GLenum type);
		void SetBinding(std::string name, void* value, VALUE_TYPE type);
		void BindShader();

		
		GLuint GetProgramID(){ return _programID; }
	private:
		GLuint _programID;
		std::vector<Binding*> _bindings;
		std::vector<std::string> sources;
	};

	inline void ShaderProgram::CompileShader(const char* source, GLenum shaderType) {
		sources.push_back(source);

		ResourceManager* RM = ResourceManager::GetInstance();
		Resource* asd = RM->LoadResource(source);

		std::string temp = asd->getTextData();
		const char* fuck = temp.c_str();
		

		GLuint shaderID = glCreateShader(shaderType);
		glShaderSource(shaderID, 1, &fuck, 0);

		glCompileShader(shaderID);
		checkCompileErrors(shaderID, shaderType);
		glAttachShader(_programID, shaderID);

		glLinkProgram(_programID);

		GLAssert();
	}
	void ShaderProgram::SetBinding(std::string name, void* value, VALUE_TYPE type) {
		_bindings.push_back(new Binding(name, value, type));
	};
	inline void ShaderProgram::BindShader() {
		for (Binding* binding : _bindings)
		{
			GLint location;
			switch (binding->_type)
			{
			case VALUE_TYPE::UNIFORM_MAT4:
			{
				location = glGetUniformLocation(_programID, binding->_name.c_str());
				if (location != -1)
				{
					glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)binding->_value);
				}
				break;
			}
			case VALUE_TYPE::UNIFORM_VEC2:
			{
				location = glGetUniformLocation(_programID, binding->_name.c_str());
				if (location != -1)
				{
					glUniform2fv(location, 1, (GLfloat*)binding->_value);
				}
				break;
			}
			case VALUE_TYPE::UNIFORM_VEC3:
			{
				location = glGetUniformLocation(_programID, binding->_name.c_str());
				if (location != -1)
				{
					glUniform3fv(location, 1, (GLfloat*)binding->_value);
				}
				break;
			}
			case VALUE_TYPE::UNIFORM_VEC4:
			{
				location = glGetUniformLocation(_programID, binding->_name.c_str());
				if (location != -1)
				{
					glUniform3fv(location, 1, (GLfloat*)binding->_value);
				}
				break;
			}
			case VALUE_TYPE::UNIFORM_FLOAT:
			{
				location = glGetUniformLocation(_programID, binding->_name.c_str());
				if (location != -1)
				{
					glUniform1fv(location, 1, (GLfloat*)binding->_value);
				}
				break;
			}
			case VALUE_TYPE::UNIFORM_INT:
			{
				location = glGetUniformLocation(_programID, binding->_name.c_str());
				if (location != -1)
				{
					glUniform1i(location, (GLint)binding->_value);
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}
	};
	void ShaderProgram::checkCompileErrors(GLuint shader, GLenum type)
	{
		GLint success;
		GLchar infoLog[1024];


		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	};
}

#endif