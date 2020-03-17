#ifndef mainMenu_H
#define mainMenu_H

#include <Core\Managers\EntityManager.hpp>
#include <Core\Managers\SystemManager.hpp>
#include <Core\Managers\SceneManager.hpp>
#include <Core/ResourceManager.h>

#include <Core/Systems/RenderingSystem.hpp>

#include <Core/Components/Render.hpp>
#include <Core/Components/Color.hpp>
#include <Core/Components/Audio.h>

#include <Core\Time.hpp>
#include <Core\Window.h>
#include <Core\Camera.hpp>
#include <Core\Input\Input.h>


class mainMenu : public Engine::Scene
{
public:
	mainMenu();
	virtual ~mainMenu();

	virtual void Init();
	virtual void Cleanup();
	virtual void Pause();
	virtual void Resume();
	virtual void Update(DeltaTime deltaTime);

private:
	Engine::EntityManager* EM;
	Engine::SystemManager* SM;

	DeltaTime Duration = 3;
	Inputs::Input* input;
	std::shared_ptr<Engine::Entity> Play;
	std::shared_ptr<Engine::Entity> Quit;
	std::shared_ptr<Engine::Entity> cam;
};

mainMenu::mainMenu(){}
mainMenu::~mainMenu(){}

void mainMenu::Init()
{
	EM = Engine::EntityManager::GetInstance();
	SM = Engine::SystemManager::GetInstance();
	cam = EM->AddEntity("Camera", std::make_shared<player>());
	EM->AddComponent<Engine::Transform>("Camera", glm::vec3(0.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
	//std::shared_ptr<Engine::Camera> temp = EM->AddComponent<Engine::Camera>("Camera");
	//SM->AddSystem<Engine::RenderingSystem>(&window, "Resources/Vert.txt", "Resources/Frag.txt");

	if (SM->GetSystem<Engine::RenderingSystem>() != nullptr) {
		//SM->GetSystem<Engine::RenderingSystem>()->SetCamera(temp);
	}
	Resource* box = ResourceManager::GetInstance()->LoadResource("Resources/cube.obj");

	Play = EM->AddEntity("player", std::make_shared<player>());
	Quit = EM->AddEntity("box", std::make_shared<player>());

	EM->AddComponent<Engine::Render>("player", box->_vertices, box->_indices);
	EM->AddComponent<Engine::Transform>("player", glm::vec3(0.0f, 2.5f, -15.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	EM->AddComponent<Engine::AABB>("player");
	EM->AddComponent<Engine::Texture>("player", "Resources/Texture1.png");
	EM->AddComponent<Engine::Render>("box", box->_vertices, box->_indices);
	EM->AddComponent<Engine::Transform>("box", glm::vec3(0.0f, -2.5f, -17.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	EM->AddComponent<Engine::AABB>("box");
	EM->AddComponent<Engine::Texture>("box", "Resources/Texture4.png");
}
void mainMenu::Cleanup()
{
	EM->Clear();
	SM->Clear();
}
void mainMenu::Pause(){}
void mainMenu::Resume(){}
void mainMenu::Update(DeltaTime deltaTime)
{
	EM->Update(deltaTime);
	SM->Update(deltaTime);
	if (input->getMouseDown(1))
	{
		Engine::SceneManager::GetInstance()->ChangeScene(new TestScene());
	}
}
#endif