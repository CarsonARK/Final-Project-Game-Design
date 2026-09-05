#pragma once
#include "WO.h"
#include "PxPhysicsAPI.h"
#include "GLViewAssign_5.h"
#include "WOBulletPhysx.h"
#include <PxPhysicsAPI.h>  
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h> 
#include "GLView.h"
#include "WorldList.h" //This is where we place all of our WOs
#include "SoundManager.h"
namespace Aftr {

	struct Tank {
		Aftr::WO* tankObject;
		Aftr::WO* aimingComponent;
		physx::PxController* tankController;
		float CurrentHealth;
		float CurrentFuel;
		bool canFire;
		bool isAiming;
		uint8_t playerNum;
		float shotPower;
	};


	struct inputState {
		bool isWPressed;
		bool isAPressed;
		bool isDPressed;
		bool isSPressed;
		bool isSpacePressed;
		bool isLeftClickHeld;
		bool isRightClickHeld;
		bool isFPressed; //for firing
		bool isCPressed; //go into aiming
		float mouseX;
		float mouseY;
	};

	const float MAX_HEALTH = 100.0f;
	const float MAX_FUEL = 100.0f;

	static irrklang::vec3df toIrrklangVec3df(const Aftr::Vector& vec) {
		return irrklang::vec3df(vec.x, vec.y, vec.z);

	}
	

	class GameManager {
	public:
		static GameManager& getInstance() {
			static GameManager instance;
			return instance;
		}


		bool playerOne = true;
		bool isGameEnded = false;
		Tank playerOneTank;
		Tank playerTwoTank;
		WOBulletPhysX* projectile = nullptr;

		void DealDamage(float damage) {
			Tank& currentTank = getInactiveTank();
			currentTank.CurrentHealth -= damage;
			if (currentTank.CurrentHealth <= 0) {
				currentTank.CurrentHealth = 0;
				isGameEnded = true;
				std::cout << "Player " << currentTank.playerNum << " has been defeated!" << std::endl;
			}
		}
		void InitializeProjectile() {
			if (projectile == nullptr) {
				GLViewAssign_5* glv = GLViewAssign_5::New({});
				//std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
				std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/sphereR5Earth.wrl");
				//sphereR5Earth.wrl
				projectile = WOBulletPhysX::New(shinyRedPlasticCube, Vector(0.5f, 0.5f, 0.5f), MESH_SHADING_TYPE::mstAUTO, Aftr::GLViewAssign_5::New({})->getPhysXSDK(), Mat4(), 1);
				projectile->setPose(Mat4());
				projectile->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
				projectile->upon_async_model_loaded([this]()
					{
						ModelMeshSkin& skin = this->projectile->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
						skin.setAmbient(aftrColor4f(0.1f, 0.1f, 0.1f, 1.0f)); //Color of object when it is not in any light
						skin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
						skin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
					});
				projectile->setLabel("bullet");
				glv->getWorldContainer()->push_back(projectile);
				glv->getPhysXScene()->addActor(*projectile->getPhysXActor());
				projectile->onFinishedFiring(); // Reset the projectile state
			}
		}
		void FireProjectile() {
			InitializeProjectile();
			if (projectile && projectile->canBeFired) {
				Tank& currentTank = getActiveTank();
				Vector aimingDirection = currentTank.aimingComponent->getModel()->getLookDirection();
				Vector tankPosition = currentTank.aimingComponent->getPosition();
				physx::PxVec3 fireDirection = physx::PxVec3(aimingDirection.x, aimingDirection.y, aimingDirection.z);
				fireDirection.normalize();
				fireDirection *= currentTank.shotPower * 75.0f; // Adjust the multiplier for desired speed
				projectile->applyInitialVelocity(fireDirection, physx::PxVec3(tankPosition.x, tankPosition.y, tankPosition.z), getInactiveTank().tankObject);
				SoundManager::getSoundManager().playTankFireSound(toIrrklangVec3df(tankPosition));
			}
		}

		void InitializeTank(bool tankOne, Aftr::WO* tankObject, Aftr::WO* aimingComponent, physx::PxController* tankController) {
			Tank tempTank;
			tempTank.tankObject = tankObject;
			tempTank.aimingComponent = aimingComponent;
			tempTank.tankController = tankController;
			tempTank.CurrentHealth = MAX_HEALTH;
			tempTank.CurrentFuel = MAX_FUEL;
			tempTank.canFire = true;
			tempTank.isAiming = false;
			tempTank.playerNum = tankOne ? 1 : 2;
			tempTank.shotPower = 0.5f;

			if (tankOne) {
				playerOneTank = tempTank;
			}
			else {
				playerTwoTank = tempTank;
			}
			return;
		}

		void StartGame() {
			isGameEnded = false;

		}

		Tank& getActiveTank() {
			return playerOne ? playerOneTank : playerTwoTank;
		}
		Tank& getInactiveTank() {
			return playerOne ? playerTwoTank : playerOneTank;
		}
		void NextTurn() {
			if (!isGameEnded) setActivePlayer(!playerOne);
		}
		void setActivePlayer(bool isPlayerOne) {
			playerOne = isPlayerOne;
			getActiveTank().CurrentFuel = MAX_FUEL * (2.f - (getActiveTank().CurrentHealth / 100.f));
			clearInputState();
			getActiveTank().canFire = true;
			getActiveTank().isAiming = false;

			Aftr::GLViewAssign_5* glv = Aftr::GLViewAssign_5::New({});
			glv->setActor(getActiveTank().tankObject);
			glv->setActorChaseType(Aftr::CAM_CHASE_TYPE::CHASEACTORLOOK5);
		}
		inputState currentInputState;
		void Tick();



		inputState& getInputState() {
			return currentInputState;
		}
		void clearInputState() {
			currentInputState.isWPressed = false;
			currentInputState.isAPressed = false;
			currentInputState.isDPressed = false;
			currentInputState.isSPressed = false;
			currentInputState.isSpacePressed = false;
			currentInputState.isLeftClickHeld = false;
			currentInputState.isRightClickHeld = false;
			currentInputState.isFPressed = false;
			currentInputState.isCPressed = false;
			currentInputState.mouseX = 0.0f;
			currentInputState.mouseY = 0.0f;
		}
		physx::PxController* getActiveController() {
			return getActiveTank().tankController;
		}physx::PxController* getInactiveController() {
			return getInactiveTank().tankController;
		}
		bool isGameOver() {
			return isGameEnded;
		}
	};
}