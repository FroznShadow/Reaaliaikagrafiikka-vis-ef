#include <memory>
#include <iostream>
#include <vector>
#include <string>

#include "Core/Window.h"
#include "Core/Time.hpp"

#include <windows.h>
#include <Core/Input/Input.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define DRAW_AABB
#include <glad/glad.h>
#include <lib/Helper.hpp>
#include <GLFW/glfw3.h>
#include <Core/Managers/EntityManager.hpp>
#include <Core/Managers/SystemManager.hpp>
#include <Core/Systems/RenderingSystem.hpp>
#include <Core/Systems/PhysicsSystem.hpp>

#include <Core/Components/Render.hpp>
#include <Core/Components/Color.hpp>
#include <Core/Components/AABB.hpp>
#include <Core/Components/Audio.h>
#include <Core/Managers/SceneManager.hpp>
#include <Core/ResourceManager.h>

#include <Core/Camera.hpp>
#include <Scene.h>

#include <Core/Rectangle.hpp>

class player : public Engine::Entity
{
public:
	player(){};
	virtual ~player(){};

	void Init(){};
	void Cleanup(){};
	void Update(DeltaTime dt){};
};

Window window;
GLFWwindow* glfwWindow;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
const unsigned int SCR_WIDTH = 1915;
const unsigned int SCR_HEIGHT = 1050;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
bool bloom = true;
bool bloomKeyPressed = false;
float exposure = 1.0f;
void framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height);
void mouse_callback(GLFWwindow* glfwWindow, double xpos, double ypos);
void scroll_callback(GLFWwindow* glfwWindow, double xoffset, double yoffset);

void processInput(GLFWwindow* window, DeltaTime deltaTime);
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		window.Resize();
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

class TestScene : public Engine::Scene
{
public:
	TestScene() {};
	virtual ~TestScene() {};

	virtual void Init()  override {

		EM = Engine::EntityManager::GetInstance();
		SM = Engine::SystemManager::GetInstance();
		PS = SM->AddSystem<Engine::PhysicsSystem>();
		GLAssert();
		
		//cam = EM->AddEntity("Camera",std::make_shared<player>());
		//EM->AddComponent<Engine::Transform>("Camera", glm::vec3(0.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
		//std::shared_ptr<Engine::Camera> temp = EM->AddComponent<Engine::Camera>("Camera");
		GLAssert();

		SM->AddSystem<Engine::PhysicsSystem>();
		SM->AddSystem<Engine::RenderingSystem>(glfwWindow, "Resources/Vert.txt", "Resources/Frag.txt", SCR_WIDTH, SCR_HEIGHT);
		//SM->AddSystem<Engine::RenderingSystem>(&window, "Resources/Vert.txt", "Resources/Frag.txt");
		if (SM->GetSystem<Engine::RenderingSystem>() != nullptr) {
			SM->GetSystem<Engine::RenderingSystem>()->SetCamera(&camera);
			//SM->GetSystem<Engine::RenderingSystem>()->SetWindowSize(SCR_WIDTH, SCR_HEIGHT);
		}GLAssert();
		player1 = EM->AddEntity("player", std::make_shared<player>());
		rotatingBox = EM->AddEntity("box", std::make_shared<player>());
		skybox = EM->AddEntity("Skybox", std::make_shared<player>());
		//Dragon = EM->AddEntity("dragon", std::make_shared<player>());
		GLAssert();
		box = ResourceManager::GetInstance()->LoadResource("Resources/cube.obj");
		Resource* skybox_res = ResourceManager::GetInstance()->LoadResource("Resources/Skybox.obj");
		//dragon = ResourceManager::GetInstance()->LoadResource("Resources/Dragon/dragon.obj");
		GLAssert();
		//Resource* audiores = ResourceManager::GetInstance()->LoadResource("Resources/bossMusic.wav");
		EM->AddComponent<Engine::Render>("Skybox", skybox_res->_vertices, skybox_res->_indices);
		EM->AddComponent<Engine::Transform>("Skybox", glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f, 100.0f, 100.0f));
		EM->AddComponent<Engine::Texture>("Skybox", "Resources/Skybox.png");
	
		GLAssert();
		//add Player
		EM->AddComponent<Engine::Render>("player", box->_vertices, box->_indices);
		EM->AddComponent<Engine::Transform>("player", glm::vec3(2.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		EM->AddComponent<Engine::AABB>("player");
		EM->AddComponent<Engine::Texture>("player", "Resources/Texture1.png");	
		EM->AddComponent<Engine::Material>("player",glm::vec3(0.5f),glm::vec3(1.0f,0.5f,0.31f),glm::vec3(1.0f,0.5f,0.31f),glm::vec3(0.5f,0.5f,0.5f),glm::vec3(0.5f),1.0f,32.0f,1.0f,31.0f,1.0f);
		GLAssert();
		//add rotating box
		EM->AddComponent<Engine::Render>("box", box->_vertices, box->_indices);
		EM->AddComponent<Engine::Transform>("box", glm::vec3(-5.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		EM->AddComponent<Engine::Shader>("box", "Resources/Vert.txt", "Resources/Frag.txt");
		// material =							(glm::vec3 emission,	 glm::vec3 ambient, glm::vec3 diffuse,	glm::vec3 specular, glm::vec3 transmittance, float ior, float shininess, float dissolve, float illum, float dummy);
		EM->AddComponent<Engine::Texture>("box", "Resources/Texture.png");
		EM->AddComponent<Engine::Material>("box", glm::vec3(0.5f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f), 1.0f, 32.0f, 1.0f, 31.0f, 1.0f);

		GLAssert();
		//add floor
		addFloor(glm::vec3(0.0f, -5.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(20.0f, 1.0f, 20.0f));
		addFloor(glm::vec3(10.0f, -5.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(20.0f, 1.0f, 20.0f));
		addFloor(glm::vec3(20.0f, -6.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(20.0f, 1.0f, 20.0f));
		GLAssert();
		//add collectible
		addNewCollectible(box);

		GLAssert();

		boxtrans = rotatingBox->GetComponent<Engine::Transform>();
		playertrans = player1->GetComponent<Engine::Transform>();
		//camtrans = cam->GetComponent<Engine::Transform>();
		precision = 1.0;
		GLAssert();
	};

	virtual void Cleanup()  override {
		EM->Clear();
		SM->Clear();
	};

	virtual void Pause()  override {};

	virtual void Resume()  override {};

	virtual void Update(DeltaTime deltaTime)  override {
		if (SM->GetSystem<Engine::RenderingSystem>() != nullptr) {
			SM->GetSystem<Engine::RenderingSystem>()->SetExposure(exposure);
		}
		if (SM->GetSystem<Engine::RenderingSystem>() != nullptr) {
			SM->GetSystem<Engine::RenderingSystem>()->SetBloom(bloom);
		}
		EM->Update(deltaTime);
		SM->Update(deltaTime);
		//checkPlayerMovement(deltaTime);
		processInput(glfwWindow, deltaTime);
		rotateCube(deltaTime);
		checkForCollectible();
	};
	void addNewCollectible(Resource* res)
	{
		collectible = EM->AddEntity("asd", std::make_shared<player>());
		EM->AddComponent<Engine::Render>("asd", res->_vertices, res->_indices);
		EM->AddComponent<Engine::Transform>("asd", glm::vec3(rand() % 10+-5, -3.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		EM->AddComponent<Engine::AABB>("asd");
		EM->AddComponent<Engine::Texture>("asd", "Resources/Texture4.png");
	}
	void addFloor(glm::vec3 t, glm::vec3 r = glm::vec3(0, 0, 0), glm::vec3 s = glm::vec3(1, 1, 1))
	{
		floor = EM->AddEntity("floor", std::make_shared<player>());
		EM->AddComponent<Engine::Render>("floor", box->_vertices, box->_indices);
		EM->AddComponent<Engine::Transform>("floor", t, r, s);
		EM->AddComponent<Engine::AABB>("floor");
		EM->AddComponent<Engine::Texture>("floor", "Resources/Texture2.png");
	}
	void checkForCollectible()
	{
		for (std::shared_ptr<Engine::Entity> entity : EM->GetEntities())
		{

			if (entity->GetName() == "asd")
			{
				return;
			}
		}
		addNewCollectible(box);
	}
	bool checkCollision()
	{
		for (std::shared_ptr<Engine::Entity> entity : EM->GetEntities())
		{
			if (PS->CheckAABBCollision(entity, player1))
			{
				if (entity->GetName() == "floor")
				{
					floorcollision = true;
				}
				if (entity->GetName() == "asd")
				{
					score++;
					EM->RemoveEntity("asd");
				}
				return true;
			}
			else if (!PS->CheckAABBCollision(entity, player1))
			{
				if (entity->GetName() == "floor")
				{
					floorcollision = false;
				}
			}
			else
				return false;

		}
	}
	void checkPlayerMovement(DeltaTime deltaTime)
	{
		float direction = 1.0f;
		Inputs::Input* inp = nullptr;
		if (inp->getKeyDown(VK_SHIFT)) { direction = -1.0f; };
		if (inp->getKeyDown(VK_CONTROL)) { camtrans->Move(glm::vec3(0, precision*deltaTime * -3, 0)); };

		if (inp->getKeyDown(0x41)) { camtrans->Move(glm::vec3(precision*deltaTime*-5.0f, 0.0f, 0.0f)); }
		//camtrans->Move(glm::vec3(precision*deltaTime*-5.0f, 0.0f, 0.0f)); 
		if (inp->getKeyDown(0x44)) { camtrans->Move(glm::vec3(precision*deltaTime*5.0f, 0.0f, 0.0f)); }

		if (inp->getKeyDown(0x57)) { camtrans->Move(glm::vec3(0.0f, 0.0f, precision*deltaTime*-5.0f)); }
		if (inp->getKeyDown(0x53)) { camtrans->Move(glm::vec3(0.0f, 0.0f, precision*deltaTime*5.0f)); }

		if (inp->getKeyDown(VK_LEFT)) { camtrans->Rotate(glm::vec3(precision*deltaTime*-10.0f, 0.0f, 0.0f)); }
		if (inp->getKeyDown(VK_RIGHT)) { camtrans->Rotate(glm::vec3(precision*deltaTime*10.0f, 0.0f, 0.0f)); }
		if (inp->getKeyDown(VK_UP)) { camtrans->Rotate(glm::vec3(0.0f, precision*deltaTime*-10.0f, 0.0f)); }
		if (inp->getKeyDown(VK_DOWN)) { camtrans->Rotate(glm::vec3(0.0f, precision*deltaTime*10.0f, 0.0f)); }

		if (inp->getKeyDown(VK_SPACE)) { camtrans->Move(glm::vec3(0, precision*deltaTime * 6, 0)); }


	}
	void rotateCube(DeltaTime deltaTime)
	{
		boxtrans->Rotate(glm::vec3(precision*deltaTime*25.0f, 0.0f, 0.0f));
		boxtrans->Rotate(glm::vec3(0.0f, precision*deltaTime*25.0f, 0.0f));
		boxtrans->Rotate(glm::vec3(0.0f, 0.0f, precision*deltaTime*25.0f));
	}
private:
	float precision;
	Engine::EntityManager* EM;
	Engine::SystemManager* SM;
	std::shared_ptr<Engine::PhysicsSystem> PS;
	std::shared_ptr<Engine::Entity> player1;
	std::shared_ptr<Engine::Entity> Dragon;
	std::shared_ptr<Engine::Entity> rotatingBox;
	std::shared_ptr<Engine::Entity> collectible;
	std::shared_ptr<Engine::Transform> camtrans;
	std::shared_ptr<Engine::Transform> playertrans;
	std::shared_ptr<Engine::Transform> boxtrans;
	std::shared_ptr<Engine::Transform> dragonTrans;
	std::shared_ptr<Engine::Entity> cam;
	std::shared_ptr<Engine::Entity> skybox;
	std::shared_ptr<Engine::Entity> floor;
	Resource* box;
	Resource* dragon;
	glm::vec3 t, r, s;
	bool floorcollision;
	int score = 0;
};

#include "splashScreen.h"
void processInput(GLFWwindow* window, DeltaTime deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed)
	{
		bloom = !bloom;
		bloomKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		bloomKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (exposure > 0.0f)
			exposure -= 0.001f;
		else
			exposure = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		exposure += 0.001f;
	}
}
int main(int argc, char** argv) {
	std::cout << "Initializing glfw" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bloom", NULL, NULL);
	if (glfwWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(glfwWindow);
	glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);
	glfwSetCursorPosCallback(glfwWindow, mouse_callback);
	glfwSetScrollCallback(glfwWindow, scroll_callback);

	// tell GLFW to capture our mouse
	// glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// glad: load all OpenGL function pointers
// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	

	//window.createWindow("Dickbutt!", glm::vec2(1920, 1080), glm::vec2(0, 0), "Resources/Cursor.ico", "Resources/Cursor.ico", ENGINE_WINDOWED, WndProc);
	//window.InitOpenGL();
	std::cout << "Initialized Window" << std::endl;
	Engine::Time timer;
	Engine::SceneManager::GetInstance()->ChangeScene(new TestScene());
	while (!glfwWindowShouldClose(glfwWindow)/* window.IsOpen() */) {

		DeltaTime dt = timer.Update();
		
		Engine::SceneManager::GetInstance()->Update(dt);
		glfwSwapBuffers(glfwWindow);
		glfwPollEvents();

	}

	//window.Uninit();
	glfwTerminate();
	return 0;
}