#ifndef Engine_RenderingSystem_hpp
#define Engine_RenderingSystem_hpp

#include <chrono>

//#include <glew.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <lib/Helper.hpp>
#include <iostream>
#include <fstream>
#include <Core/Shaders/ShaderProgram.hpp>
#include <Core/Shaders/LOGL_Shader.h>
#include <Core/Managers/SystemManager.hpp>

#include <Core/Components/Transform.hpp>
#include <Core/Components/Render.hpp>
#include <Core/Components/Color.hpp>
#include <Core/Components/AABB.hpp>
#include <Core/Components/Shader.hpp>
#include <Core/Components/Texture.h>
#include <Core/Camera.hpp>
#include <Core/Components/Material.hpp>

#include <Core/Systems/Buffer.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Engine
{
	class RenderingSystem : public System
	{
	public:

		RenderingSystem(GLFWwindow* window, const char* vertexShaderPath = "Resources/Vert.txt", const char* fragmentShaderPath = "Resources/Frag.txt", unsigned int scr_width = 1280, unsigned int scr_height = 720)
			: _window(window), _defaultShader(new ShaderProgram()), System() {

			_defaultShader->CompileShader(vertexShaderPath, GL_VERTEX_SHADER);
			_defaultShader->CompileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
			_SCR_WIDTH = scr_width;
			_SCR_HEIGHT = scr_height;



			GLAssert();
		};
		virtual ~RenderingSystem() {};

		virtual void Init() override;
		virtual void Cleanup() override;

		virtual void Pause() override;
		virtual void Resume() override;

		virtual void Update(DeltaTime deltaTime) override;


		void SetCamera(Camera* cam) { _cam = cam; };
		void SetExposure(float exposure) { _exposure = exposure; };
		void SetBloom(bool bloom) { _bloom = bloom; };
		//void SetWindowSize(int SCR_WIDTH, int SCR_HEIGHT) { _SCR_WIDTH = SCR_WIDTH; _SCR_HEIGHT = SCR_HEIGHT; };
	private:
		EntityManager* _entityManager;
		ShaderProgram* _defaultShader;
		GLFWwindow* _window;
		LOGLShader* shader;
		LOGLShader* shaderLight;
		LOGLShader* shaderBlur;
		LOGLShader* shaderBloomFinal;

		//ShaderProgram*	shaderExtract = new ShaderProgram();
		//ShaderProgram*	shaderExtractLigth = new ShaderProgram();
		//ShaderProgram*	shaderBloomFinal = new ShaderProgram();
		//ShaderProgram*	shaderBloom = new ShaderProgram();
		Buffer _vertexBuffer;
		Buffer _indiceBuffer;
		unsigned int hdrFBO;
		unsigned int colorBuffers[2];
		unsigned int attachments[2];    
		unsigned int pingpongFBO[2];
		unsigned int pingpongColorbuffers[2];
		unsigned int woodTexture;
		unsigned int containerTexture;
		unsigned int _SCR_WIDTH;
		unsigned int _SCR_HEIGHT;
		float _exposure;
		bool _bloom;
		std::vector<glm::vec3> lightColors;
		std::vector<glm::vec3> lightPositions;
		Camera* _cam;

	};

	unsigned int loadTexture(char const* path, bool gammaCorrection)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum internalFormat;
			GLenum dataFormat;
			if (nrComponents == 1)
			{
				internalFormat = dataFormat = GL_RED;
			}
			else if (nrComponents == 3)
			{
				internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
				dataFormat = GL_RGB;
			}
			else if (nrComponents == 4)
			{
				internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
				dataFormat = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	void renderCube()
	{
		// initialize (if necessary)
		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				// bottom face
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				// top face
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	inline void RenderingSystem::Init() {
		_entityManager = EntityManager::GetInstance();

		GLAssert();

		glEnable(GL_DEPTH_TEST);
		GLAssert();

		glUseProgram(0);

	/*	_vertexBuffer.CreateBuffer(GL_ARRAY_BUFFER);
		_indiceBuffer.CreateBuffer(GL_ELEMENT_ARRAY_BUFFER);*/

		//glClearColor(0, 0, 0, 1.0f);

		try
		{
			shader = new LOGLShader("Resources/7.bloom.vs", "Resources/7.bloom.fs");
			shaderLight = new LOGLShader("Resources/7.bloom.vs", "Resources/7.light_box.fs");
			shaderBlur = new LOGLShader("Resources/7.blur.vs", "Resources/7.blur.fs");
			shaderBloomFinal = new LOGLShader("Resources/7.bloom_final.vs", "Resources/7.bloom_final.fs");
			//shaderExtract->CompileShader("Resources/7.bloom.vs", GL_VERTEX_SHADER);
			//shaderExtract->CompileShader("Resources/7.bloom.fs", GL_FRAGMENT_SHADER);
			//shaderExtractLigth->CompileShader("Resources/7.bloom.vs", GL_VERTEX_SHADER);
			//shaderExtractLigth->CompileShader("Resources/7.light_box.fs", GL_FRAGMENT_SHADER);
			//shaderBlur->CompileShader("Resources/7.blur.vs", GL_VERTEX_SHADER);
			//shaderBlur->CompileShader("Resources/7.blur.fs", GL_FRAGMENT_SHADER);
			//shaderBloom->CompileShader("Resources/7.bloom_final.vs", GL_VERTEX_SHADER);
			//shaderBloom->CompileShader("Resources/7.bloom_final.fs", GL_FRAGMENT_SHADER);

			woodTexture = loadTexture("Resources/Texture.png", true); // note that we're loading the texture as an SRGB texture
			containerTexture = loadTexture("Resources/Texture1.png", true); // note that we're loading the texture as an SRGB texture


			glGenFramebuffers(1, &hdrFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

			glGenTextures(2, colorBuffers);
			for (unsigned int i = 0; i < 2; i++)
			{
				glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_RGB16F,  _SCR_WIDTH, _SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL
				);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// attach texture to framebuffer
				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
				);
			}

			unsigned int rboDepth;
			glGenRenderbuffers(1, &rboDepth);
			glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _SCR_WIDTH, _SCR_HEIGHT);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
			// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
			unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers(2, attachments);
			// finally check if framebuffer is complete
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// ping-pong-framebuffer for blurring
			
			glGenFramebuffers(2, pingpongFBO);
			glGenTextures(2, pingpongColorbuffers);
			for (unsigned int i = 0; i < 2; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
				glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _SCR_WIDTH, _SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
				// also check if framebuffers are complete (no need for depth buffer)
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "Framebuffer not complete!" << std::endl;
			}
			// lighting info
			// -------------
			// positions
			lightPositions.push_back(glm::vec3(0.0f, 0.5f, -9.5f));
			lightPositions.push_back(glm::vec3(-4.5f, 0.5f, -8.0f));
			lightPositions.push_back(glm::vec3(4.0f, 0.5f, -9.0f));
			lightPositions.push_back(glm::vec3(2.5f, 0.5f, -8.f));
			// colors
			lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
			lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
			lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
			lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

			shader->use();
			shader->setInt("diffuseTexture", 0);
			shaderBlur->use();
			shaderBlur->setInt("image", 0);
			shaderBloomFinal->use();
			shaderBloomFinal->setInt("scene", 0);
			shaderBloomFinal->setInt("bloomBlur", 1);
			//glUseProgram(shaderExtract->GetProgramID());
			//shaderExtract->SetBinding("diffuseTexture", 0, Engine::VALUE_TYPE::UNIFORM_INT);
			//glUseProgram(shaderBlur->GetProgramID());
			//shaderBlur->SetBinding("image", 0, Engine::VALUE_TYPE::UNIFORM_INT);
			//glUseProgram(shaderBloom->GetProgramID());
			//shaderBloom->SetBinding("scene", 0, Engine::VALUE_TYPE::UNIFORM_INT);
			//shaderBloom->SetBinding("bloomBlur", 0, Engine::VALUE_TYPE::UNIFORM_INT);
			//glUniform1i(glGetUniformLocation(shaderExtract->GetProgramID(), "diffuseTexture"), 0);
			//glUseProgram(shaderBlur->GetProgramID());
			//glUniform1i(glGetUniformLocation(shaderBlur->GetProgramID(), "image"), 0);
			//glUseProgram(shaderBloom->GetProgramID());
			//glUniform1i(glGetUniformLocation(shaderBloom->GetProgramID(), "scene"), 0);
			//glUniform1i(glGetUniformLocation(shaderBloom->GetProgramID(), "bloomBlur"), 0);

			GLAssert();
		}
		catch (std::ifstream::failure e) {
			std::cout << e.what() << std::endl;
		}
	};

	inline void RenderingSystem::Cleanup() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//SwapBuffers(_window->GetHDC());
		glfwSwapBuffers(_window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	};

	inline void RenderingSystem::Pause() {
		if (!_paused) {
			_paused = true;
		}
	};

	inline void RenderingSystem::Resume() {
		if (_paused) {
			_paused = false;
		}
	};

	inline void RenderingSystem::Update(DeltaTime deltaTime) {
		if (!_paused) {
			GLAssert();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			GLAssert();
			// 1. render scene into floating point framebuffer
			// -----------------------------------------------
			glm::mat4 View = _cam->GetViewMatrix();
			glm::mat4 Projection = glm::perspective(glm::radians(_cam->Zoom), (float)_SCR_WIDTH / (float)_SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 Model = glm::mat4(1);
			glm::mat4 model = glm::mat4(1);
			shader->use();
			GLAssert();
			shader->setMat4("projection", Projection);
			GLAssert();
			shader->setMat4("view", View);
			GLAssert();
			/*glUseProgram(shaderExtract->GetProgramID());
			shaderExtract->SetBinding("projection", &Projection[0][0], Engine::VALUE_TYPE::UNIFORM_MAT4);
			shaderExtract->SetBinding("view", &View[0][0], Engine::VALUE_TYPE::UNIFORM_MAT4);*/
			//glUniformMatrix4fv(glGetUniformLocation(shaderExtract->GetProgramID(), "projection"), 1, GL_FALSE, &Projection[0][0]);
			//glUniformMatrix4fv(glGetUniformLocation(shaderExtract->GetProgramID(), "view"), 1, GL_FALSE, &View[0][0]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, containerTexture);
			GLAssert();
			glm::vec3 ViewPosition = _cam->Position;
			glm::vec3 LightPosition = glm::vec3(150.0f, 150.0f, 0.0f);
			glm::vec3 LightColor = glm::vec3(5.0f, 5.0f, 5.0f);
			// set lighting uniforms
			for (unsigned int i = 0; i < lightPositions.size(); i++)
			{
				shader->setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
				shader->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
				//shaderExtract->SetBinding("lights[" + std::to_string(i) + "].Position", &lightPositions[i], Engine::VALUE_TYPE::UNIFORM_VEC3);
				//shaderExtract->SetBinding("lights[" + std::to_string(i) + "].Color", &lightColors[i], Engine::VALUE_TYPE::UNIFORM_VEC3);
				//
			}
			//glUniform3fv(glGetUniformLocation(shaderExtract->GetProgramID(), "lightPosition"), 1, glm::value_ptr(LightPosition));
			//glUniform3fv(glGetUniformLocation(shaderExtract->GetProgramID(), "lightColor"), 1, glm::value_ptr(LightColor));
			GLAssert();
			shader->setVec3("viewPos", ViewPosition);
			//shaderExtract->SetBinding("viewPos", &ViewPosition, Engine::VALUE_TYPE::UNIFORM_VEC3);

			// create one large cube that acts as the floor
			/*model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
			model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
			shader->setMat4("model", model);
			shader->setMat4("model", model);*/
			//shaderExtract->SetBinding("model", &model, Engine::VALUE_TYPE::UNIFORM_MAT4);
			//shaderExtract->SetBinding("model", &model, Engine::VALUE_TYPE::UNIFORM_MAT4);
			//renderCube();
			//// then create multiple cubes as the scenery
			//glBindTexture(GL_TEXTURE_2D, containerTexture);
			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
			//model = glm::scale(model, glm::vec3(0.5f));
			//shader->setMat4("model", model);
			////shaderExtract->SetBinding("model", &model, Engine::VALUE_TYPE::UNIFORM_MAT4);
			//renderCube();

			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
			//model = glm::scale(model, glm::vec3(0.5f));
			//shader->setMat4("model", model);
			////shaderExtract->SetBinding("model", &model, Engine::VALUE_TYPE::UNIFORM_MAT4);
			//renderCube();

			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
			//model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			//shader->setMat4("model", model);
			////shaderExtract->SetBinding("model", &model, Engine::VALUE_TYPE::UNIFORM_MAT4);
			//renderCube();

			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
			//model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			//model = glm::scale(model, glm::vec3(1.25));
			//shader->setMat4("model", model);
			////shaderExtract->SetBinding("model", &model, Engine::VALUE_TYPE::UNIFORM_MAT4);
			//renderCube();

			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
			//model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			//shader->setMat4("model", model);
			////shaderExtract->SetBinding("model", &model, Engine::VALUE_TYPE::UNIFORM_MAT4);
			//renderCube();

			//model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
			//model = glm::scale(model, glm::vec3(0.5f));
			//shader->setMat4("model", model);
			////shaderExtract->SetBinding("model", &model, Engine::VALUE_TYPE::UNIFORM_MAT4);
			//renderCube();

			std::vector<std::shared_ptr<Entity>> entities = _entityManager->GetEntities();
			for (std::shared_ptr<Entity> entity : entities) {

				//Get components needded for rendering
				std::shared_ptr<Transform>	transform			= entity->GetComponent<Transform>();
				std::shared_ptr<Render>		render				= entity->GetComponent<Render>();
				std::shared_ptr<AABB>		aabb				= entity->GetComponent<AABB>();
				std::shared_ptr<Texture>	texture				= entity->GetComponent<Texture>();
				std::shared_ptr<Shader>		entityShader		= entity->GetComponent<Shader>();
				std::shared_ptr<Material>	material			= entity->GetComponent<Material>();
				GLAssert();
				//Check for the bare minimum components needed for rendering
				if (transform != nullptr && render != nullptr) {

					//If entity has shader component use it. Otherwise use default shader
					GLuint shaderID = shader->ID;
					/*if (entityShader != nullptr) { shaderID = entityShader->GetProgramID(); }
					else { shaderID = _defaultShader->GetProgramID(); }
					glUseProgram(shaderID);
					GLAssert();*/

					//GLuint shaderID = shaderExtract->GetProgramID();
				
					Model = glm::translate(glm::mat4(1), transform->GetPosition());
					glm::mat4 Scale = glm::scale(glm::mat4(1), transform->GetScale());
					glm::mat4 Rotate = glm::toMat4(transform->GetRotationQuat());
					Model = Model * Rotate * Scale;
					//entityShader->SetBinding("model", &Model, Engine::VALUE_TYPE::UNIFORM_MAT4);
					shader->setMat4("model", Model);

					renderCube();
					GLAssert();
				} //if (transform != nullptr && render != nullptr)
			} //for (std::shared_ptr<Entity> entity : entities)
			shaderLight->use();
			shaderLight->setMat4("projection", Projection);
			shaderLight->setMat4("view", View);
			//glUseProgram(shaderExtractLigth->GetProgramID());
			//shaderExtractLigth->SetBinding("projection", &Projection, Engine::VALUE_TYPE::UNIFORM_MAT4);
			//shaderExtractLigth->SetBinding("view", &View, Engine::VALUE_TYPE::UNIFORM_MAT4);
			for (unsigned int i = 0; i < lightPositions.size(); i++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(lightPositions[i]));
				model = glm::scale(model, glm::vec3(0.25f));
				shaderLight->setMat4("model", model);
				shaderLight->setVec3("lightColor", lightColors[i]);
				//shaderExtractLigth->SetBinding("model",&model,Engine::VALUE_TYPE::UNIFORM_MAT4);
				//shaderExtractLigth->SetBinding("lightColor",&lightColors[i],Engine::VALUE_TYPE::UNIFORM_VEC3);
				renderCube();
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			// 2. blur bright fragments with two-pass Gaussian Blur 
					// --------------------------------------------------
			bool horizontal = true, first_iteration = true;
			unsigned int amount = 10;
			shaderBlur->use();
			//glUseProgram(shaderBlur->GetProgramID());
			for (unsigned int i = 0; i < amount; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				shaderBlur->setInt("horizontal", horizontal);
				//shaderBlur->SetBinding("horizontal", &horizontal, Engine::VALUE_TYPE::UNIFORM_INT);// glUniform1i(glGetUniformLocation(shaderBlur->GetProgramID(), "horizontal"), horizontal);
				glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
				renderQuad();
				GLAssert();
				horizontal = !horizontal;
				if (first_iteration)
					first_iteration = false;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			GLAssert();
			// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
			// --------------------------------------------------------------------------------------------------------------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaderBloomFinal->use();
			//glUseProgram(shaderBloom->GetProgramID());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
			shaderBloomFinal->setInt("bloom", _bloom);
			shaderBloomFinal->setFloat("exposure", _exposure);
			/*shaderBloom->SetBinding("bloom", &bloom, Engine::VALUE_TYPE::UNIFORM_INT);
			shaderBloom->SetBinding("exposure", &exposure, Engine::VALUE_TYPE::UNIFORM_FLOAT);*/
			//glUniform1i(glGetUniformLocation(shaderBloom->GetProgramID(), "bloom"), true);
			//glUniform1f(glGetUniformLocation(shaderBloom->GetProgramID(), "exposure"), 1.0f);
			//Draw object
			renderQuad();
			GLAssert();

			std::cout << "bloom: " << (_bloom ? "on" : "off") << "| exposure: " << _exposure << std::endl;
			//SwapBuffers(_window->GetHDC());
			//glfwSwapBuffers(_window);
			/*glUseProgram(0);
			GLAssert();*/

		} //if (!_paused)

	};
}
#endif