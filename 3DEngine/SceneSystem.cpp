//#include "SceneSystem.h"
//
//Scene* SceneSystem::getCurrentScene()
//{
//
//	return currentScene;
//
//}
//void SceneSystem::changeScenes(int id)
//{
//	for (auto it : sceneList)
//	{
//		if (it->sceneId == id)
//		{
//			setScene(it);
//			return;
//		}
//	}
//	std::cout << "scene not found" << std::endl;
//
//}
//void SceneSystem::addScene(Scene* scene)
//{
//	sceneList.push_back(scene);
//}
//void SceneSystem::loadScene(int id)
//{
//	for (auto it : sceneList)
//	{
//		if (it->sceneId == id)
//		{
//			it->load();
//			return;
//		}
//	}
//	std::cout << "scene not found" << std::endl;
//}
//void SceneSystem::unLoadScene(int id)
//{
//	for (auto it : sceneList)
//	{
//		if (it->sceneId == id)
//		{
//			it->unload();
//			return;
//		}
//	}
//	std::cout << "scene not found" << std::endl;
//}