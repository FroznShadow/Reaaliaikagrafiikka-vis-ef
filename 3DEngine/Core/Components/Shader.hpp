#ifndef Engine_Shader_hpp
#define Engine_Shader_hpp

//#include <glew.h>
#include <Core/ResourceManager.h>

#include <Core/Managers/EntityManager.hpp>
#include <glad/glad.h>
namespace Engine {

	enum class VALUE_TYPE
	{
		UNIFORM_INT,
		UNIFORM_MAT4,
		UNIFORM_VEC2,
		UNIFORM_VEC3,
		UNIFORM_VEC4,
		UNIFORM_FLOAT
	};

	struct Binding
	{
		Binding(std::string name, void* value, VALUE_TYPE type) : _name(name), _value(value), _type(type) {};
		std::string _name;
		void* _value;
		VALUE_TYPE _type;
	};

	class Shader : public Component
	{
	public:
		Shader(const char* vertexSource, const char* fragmentSource);
		Shader();
		virtual ~Shader();

		virtual void Init() override;
		virtual void Cleanup() override;

		virtual void Update(DeltaTime deltaTime) override;

		void CompileShader(const char* source, GLenum shaderType);
		void checkCompileErrors(GLuint shader, GLenum type);
		void SetBinding(std::string name, void* value, VALUE_TYPE type);
		void setInt(const std::string& name, int value) const
		{
			glUniform1i(glGetUniformLocation(_programID, name.c_str()), value);
		}
		void BindShader();
		void use()
		{
			glUseProgram(_programID);
		}
		GLuint GetProgramID() { return _programID; }

	private:
		GLuint _programID;

		std::vector<Binding*> _bindings;
	};

	inline Shader::Shader(const char* vertexSource, const char* fragmentSource) {
		_programID = glCreateProgram();

		CompileShader(vertexSource, GL_VERTEX_SHADER);
		CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
	};

	inline Shader::Shader() {
		_programID = glCreateProgram();
	};

	inline Shader::~Shader() {};

	inline void Shader::Init() {};

	inline void Shader::Cleanup() {
		glDeleteShader(_programID);
	};

	inline void Shader::Update(DeltaTime deltaTime) {};

	inline void Shader::CompileShader(const char* source, GLenum shaderType) {
		Resource* asd = ResourceManager::GetInstance()->LoadResource(source);
		std::string temp = asd->getTextData();
		const char* fuck = temp.c_str();
		GLuint shaderID = glCreateShader(shaderType);
		glShaderSource(shaderID, 1, &fuck, 0);

		glCompileShader(shaderID);
		checkCompileErrors(shaderID, shaderType);
		glAttachShader(_programID, shaderID);

		glLinkProgram(_programID);
	};

	void Shader::SetBinding(std::string name, void* value, VALUE_TYPE type) {
		_bindings.push_back(new Binding(name, value, type));
	};
	void Shader::checkCompileErrors(GLuint shader, GLenum type)
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
	inline void Shader::BindShader() {
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
};

#endif