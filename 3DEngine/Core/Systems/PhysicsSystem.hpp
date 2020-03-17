#ifndef Engine_PhysicsSystem_hpp
#define Engine_PhysicsSystem_hpp

#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Core/Shaders/ShaderProgram.hpp>
#include <Core/Managers/SystemManager.hpp>

#include <Core/Components/Transform.hpp>
#include <Core/Components/Render.hpp>
#include <Core/Components/Color.hpp>
#include <Core/Components/AABB.hpp>

#include <Core/Systems/Buffer.hpp>

namespace Engine
{
	class PhysicsSystem : public System
	{
	public:
		PhysicsSystem() : System() {};
		virtual ~PhysicsSystem() {};

		virtual void Init() override;
		virtual void Cleanup() override;

		virtual void Pause() override;
		virtual void Resume() override;

		virtual void Update(DeltaTime deltaTime) override;

		bool CheckAABBCollision(std::shared_ptr<Entity> lhsEntity, std::shared_ptr<Entity> rhsEntity);
	private:
		EntityManager* _entityManager;
	};

	inline void PhysicsSystem::Init() {
		_entityManager = EntityManager::GetInstance();
	};

	inline void PhysicsSystem::Cleanup() {

	};

	inline void PhysicsSystem::Pause() {

	};

	inline void PhysicsSystem::Resume() {

	};

	inline void PhysicsSystem::Update(DeltaTime deltaTime) {
		std::vector<std::shared_ptr<Entity>> entities = _entityManager->GetEntities();
		for (auto it = entities.begin(); it != entities.end(); it++) {
			auto transformable = it->get()->GetComponent<Transform>();
			auto renderable = it->get()->GetComponent<Render>();
			auto aabb = it->get()->GetComponent<AABB>();

			if (transformable != nullptr && renderable != nullptr && aabb != nullptr) {
				glm::mat4 scale = glm::mat4(1);
				glm::mat4 rotation = glm::mat4(1);
				glm::mat4 position = glm::mat4(1);

				std::vector<glm::vec3> vertexData = renderable->GetVertexData();
				std::vector<glm::vec4> transVertDat;

				scale = glm::scale(scale, transformable->GetScale());

				rotation = glm::rotate(rotation, transformable->GetRotationRad().x, glm::vec3(1.0f, 0.0f, 0.0f));
				rotation = glm::rotate(rotation, transformable->GetRotationRad().y, glm::vec3(0.0f, 1.0f, 0.0f));
				rotation = glm::rotate(rotation, transformable->GetRotationRad().z, glm::vec3(0.0f, 0.0f, 1.0f));

				glm::mat4 trans;

				trans = rotation * scale;

				for (size_t i = 0; i < vertexData.size(); i++) {
					if (i % 3 == 0) {
						transVertDat.push_back(trans * glm::vec4(vertexData[i], 1));
					}
				}

				glm::vec3 min = glm::vec3(transVertDat[0].x, transVertDat[0].y, transVertDat[0].z);
				glm::vec3 max = glm::vec3(transVertDat[0].x, transVertDat[0].y, transVertDat[0].z);

				for (size_t i = 0; i < transVertDat.size(); i++) {
					if (transVertDat[i].x < min.x) { min.x = transVertDat[i].x; }
					if (transVertDat[i].y < min.y) { min.y = transVertDat[i].y; }
					if (transVertDat[i].z < min.z) { min.z = transVertDat[i].z; }
					if (transVertDat[i].x > max.x) { max.x = transVertDat[i].x; }
					if (transVertDat[i].y > max.y) { max.y = transVertDat[i].y; }
					if (transVertDat[i].z > max.z) { max.z = transVertDat[i].z; }
				} //for (size_t i = 0; i < transVertDat.size(); i++)

				aabb->UpdateAABB(min, max);
			} //if (transformable != nullptr && renderable != nullptr)
		} //for (auto it = entities.begin(); it != entities.end(); it++)
	};

	inline bool PhysicsSystem::CheckAABBCollision(std::shared_ptr<Entity> lhsEntity, std::shared_ptr<Entity> rhsEntity) {
		std::shared_ptr<AABB> lhsAABB = lhsEntity->GetComponent<AABB>();
		std::shared_ptr<AABB> rhsAABB = rhsEntity->GetComponent<AABB>();

		if (lhsAABB != nullptr && rhsAABB != nullptr && lhsEntity->GetName() != rhsEntity->GetName()) {
			std::shared_ptr<Transform> lhsTransform = lhsEntity->GetComponent<Transform>();
			std::shared_ptr<Transform> rhsTransform = rhsEntity->GetComponent<Transform>();

			glm::mat4 lhsModel = glm::translate(glm::mat4(1), lhsTransform->GetPosition());
			glm::mat4 rhsModel = glm::translate(glm::mat4(1), rhsTransform->GetPosition());

			glm::vec4 lhsTransMin = lhsModel * glm::vec4(lhsAABB->_min, 1.0f);
			glm::vec4 lhsTransMax = lhsModel * glm::vec4(lhsAABB->_max, 1.0f);
			glm::vec4 rhsTransMin = rhsModel * glm::vec4(rhsAABB->_min, 1.0f);
			glm::vec4 rhsTransMax = rhsModel * glm::vec4(rhsAABB->_max, 1.0f);

			if (lhsTransMin.x > rhsTransMax.x) return false;
			if (lhsTransMax.x < rhsTransMin.x) return false;
			if (lhsTransMin.y > rhsTransMax.y) return false;
			if (lhsTransMax.y < rhsTransMin.y) return false;
			if (lhsTransMin.z > rhsTransMax.z) return false;
			if (lhsTransMax.z < rhsTransMin.z) return false;

			return true;
		}
		return false;
	};
}

#endif
