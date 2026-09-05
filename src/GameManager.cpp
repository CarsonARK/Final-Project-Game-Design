#include "GameManager.h"
#include "GLViewAssign_5.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include "MGLIndexedGeometry.h"
#include "IndexedGeometrySphereTriStrip.h"
#include "WOAxesTubes.h"
#include "AftrTimer.h"
#include <chrono>
#include "NetMessengerClient.h"        // The actual client object
#include <irrKlang.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <PxPhysicsAPI.h>
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WONVStaticPlane.h"
#include "GameManager.h"
#include "WOController.h"
#include "Vector.h"
#include "SoundManager.h"

namespace Aftr {

	void GameManager::Tick() {
		if (isGameEnded) return;
		Tank& currentTank = getActiveTank();

		SoundManager::getSoundManager().setTanksNotMoving();

		//toggle aiming mode when C is pressed
		if (currentInputState.isCPressed) {
			currentTank.isAiming = !currentTank.isAiming;
		}

		if (currentTank.isAiming) {
			Aftr::GLViewAssign_5* glv = Aftr::GLViewAssign_5::New({});
			glv->setActor(getActiveTank().aimingComponent);
			glv->setActorChaseType(Aftr::CAM_CHASE_TYPE::CHASEACTORLOOK3);

			SDL_SetRelativeMouseMode(SDL_TRUE);

			//we will clamp shot power based off the hp of the relative tanks so that the loser can shoot faster, giving them an advantage to catch up
			float HealthAdvantage = getActiveTank().CurrentHealth - getInactiveTank().CurrentHealth; //-80 to 80
			float maxShotPowerModifier = 1.f - (HealthAdvantage / 100.f);
			getActiveTank().shotPower = std::clamp(getActiveTank().shotPower , 0.0f, maxShotPowerModifier);

			//currentTank.aimingComponent->getModel()->rotateAboutGlobalZ(-currentInputState.mouseX * .0025f);
			//currentTank.aimingComponent->rotateAboutRelY(-currentInputState.mouseY * .002f);//rel y works ish

			if (currentInputState.isFPressed && currentTank.canFire) {
				//fire logic

				std::cout << "FIRE!" << std::endl << "FIRE!" << std::endl << "FIRE!" << std::endl << "FIRE!" << std::endl << "FIRE!" << std::endl << "FIRE!" << std::endl << "FIRE!" << std::endl << "FIRE!" << std::endl << "FIRE!" << std::endl;
				currentTank.canFire = false;
				FireProjectile();

				//spawn projectile and apply force based on shotPower

			}
		}
		else {
			Aftr::GLViewAssign_5* glv = Aftr::GLViewAssign_5::New({});
			glv->setActor(getActiveTank().tankObject);
			glv->setActorChaseType(Aftr::CAM_CHASE_TYPE::CHASEACTORLOOK5);

			SDL_SetRelativeMouseMode(SDL_TRUE);

			//currentTank.tankObject->rotateAboutGlobalZ(-currentInputState.mouseX * .0025f);
		}


		//Bullet Logic / Physics 


		//delta time calculations
		static auto before = std::chrono::system_clock::now();
		auto now = std::chrono::system_clock::now();
		auto delta_t = std::chrono::duration<float>(now - before);
		before = now;
		std::chrono::duration ms_in_sec = std::chrono::milliseconds(1000);
		float dt = delta_t / ms_in_sec;


		//physx logic
		auto glv = Aftr::GLViewAssign_5::New({});
		if (glv->getPhysXScene() != nullptr) {
			auto scene = glv->getPhysXScene();
			scene->simulate(dt);
			scene->fetchResults(true);

			if (this->getActiveController() != nullptr) {

	
				Vector look = glv->getCam()->getLookDirection();
				Vector forward(look.x, look.y, 0);
				forward.normalize();
				Vector right = forward.crossProduct(Vector(0, 0, 1)); // Assuming Z is Up
				Vector up = forward.crossProduct(Vector(1, 0, 0)); // Assuming X is Right
				//void setListenerPosition(irrklang::vec3df position, irrklang::vec3df lookDirection, irrklang::vec3df UpVector) {
				SoundManager::getSoundManager().setListenerPosition(toIrrklangVec3df(glv->getCam()->getPosition()), toIrrklangVec3df(glv->getCam()->getLookDirection()), toIrrklangVec3df(up));

				Vector movement(0, 0, 0);
				if (currentInputState.isWPressed) movement += forward;
				if (currentInputState.isSPressed) movement -= forward;
				if (currentInputState.isAPressed) movement -= right;
				if (currentInputState.isDPressed) movement += right;

				movement.normalize();
				float speed = 20.0f; // Increase this to test
				movement *= speed * dt;

				// 2. Handle Gravity & Jumping
				static float verticalVelocity = 0.0f;
				const float jumpImpulse = 75.0f;
				const float gravityForce = -35.0f; // Stronger gravity feels "snappier"

				// Check collisions from the LAST frame
				physx::PxControllerState state;
				this->getActiveController()->getState(state);
				bool isGrounded = (state.collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);

				if (isGrounded) {
					verticalVelocity = -0.1f; // "Glue" to ground
					if (currentInputState.isSpacePressed) {
						verticalVelocity = jumpImpulse;
					}
				}
				else {
					verticalVelocity += gravityForce * dt;
				}

				// 3. Combine and Move
				physx::PxVec3 defaultFalling(0, 0, -100 * dt);
				physx::PxVec3 slowFalling(0, 0, -1 * dt);
				physx::PxVec3 disp(movement.x, movement.y, verticalVelocity * dt);
				physx::PxControllerFilters filters;

				// Perform the move
				if (!currentTank.isAiming && currentTank.CurrentFuel > 0) {
					this->getActiveController()->move(disp, 0.001f, dt, filters);
					this->getInactiveController()->move(defaultFalling, 0.001f, dt, filters);

					const float FUEL_CONSUMPTION_RATE = 40.0f;
					float fuelCost = sqrt(movement.x * movement.x + movement.y * movement.y) * dt * FUEL_CONSUMPTION_RATE;
					currentTank.CurrentFuel -= fuelCost;


					if (movement.x != 0 || movement.y != 0) {
						SoundManager::getSoundManager().setTankMoving(playerOne, true, toIrrklangVec3df(this->getActiveTank().tankObject->getPosition()));
					}
					else {
						SoundManager::getSoundManager().setTankMoving(playerOne, false, toIrrklangVec3df(this->getActiveTank().tankObject->getPosition()));
					}
					
				}
				else {
					this->getActiveController()->move(slowFalling, 0.001f, dt, filters);
					this->getInactiveController()->move(defaultFalling, 0.001f, dt, filters);
				}
				/*
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
	aimingComponent->getPosition()*/
				

				//std::cout << "\n Current Fuel in Tank: " << currentTank.CurrentFuel << std::endl;


				WOController* POT = static_cast<WOController*>(playerOneTank.tankObject);
				WOController* PTT = static_cast<WOController*>(playerTwoTank.tankObject);
				POT->updatePoseFromPhysicsEngine();
				PTT->updatePoseFromPhysicsEngine();
				if(projectile != nullptr)
				projectile->updatePoseFromPhysicsEngine();
			}
		}



		currentInputState.isSpacePressed = false;
		currentInputState.isLeftClickHeld = false;
		currentInputState.isRightClickHeld = false;
		currentInputState.isFPressed = false;
		currentInputState.isCPressed = false;
		currentInputState.mouseX = 0.0f;
		currentInputState.mouseY = 0.0f;
	}
}