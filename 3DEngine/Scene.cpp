//#include "Scene.h"
//
//void Scene::update(DeltaTime dt) 
//{
//	for (auto it : entities)
//	{
//		it.Update(dt);
//	}
//}
//void Scene::draw() 
//{
//	//virtual??
//}
//void Scene::load() 
//{
//	//load entities
//	//load textures
//	//load audio
//	//load other possible stuff
//}
//void Scene::unload() 
//{
//	//unload above mentioned stuff
//}
////void Scene::createJsonFile()
////{
////	Json::Value event;
////
////	for (auto it : entities)
////	{
////		event["Entities"][it.GetName()];
////		for (auto j : it.GetComponents())
////		{
////			event["Entities"][it.GetName()]["Component"] = j->Serialize();
////			
////		}
////
////
////	}
////	
////}