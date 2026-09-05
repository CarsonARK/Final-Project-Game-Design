
#pragma once
#include "WO.h"
#include "PxPhysicsAPI.h"
#include <chrono>
namespace Aftr {

    class WOBulletPhysX : public WO {
    public:
        // Factory method
        static WOBulletPhysX* New(const std::string& modelPath, Vector scale = Vector(1, 1, 1), MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO, physx::PxPhysics* p = nullptr, Aftr::Mat4 initialPose = Aftr::Mat4(), float Mass = 1.f);

        virtual ~WOBulletPhysX();

        // The "Bridge": Call this every frame to move the visual model to the physics location
        virtual void updatePoseFromPhysicsEngine();

        // Getter/Setter for the PhysX Actor
        physx::PxRigidDynamic* getPhysXActor() { return this->actor; }
        void setPhysXActor(physx::PxRigidDynamic* actor) { this->actor = actor; }

		void applyInitialVelocity(physx::PxVec3 velocity, physx::PxVec3 fromLocation, WO* target) {
            if (canBeFired && !isCurrentlyFired) {
                isCurrentlyFired = true;
                canBeFired = false;
				bulletTarget = target;
                timeFired = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                if (this->actor) {
                    this->actor->setGlobalPose(physx::PxTransform(fromLocation));
                    this->actor->setLinearVelocity(velocity);
                }
            }
        }

        void onFinishedFiring() {
            isCurrentlyFired = false;
            canBeFired = true;
            bulletTarget = nullptr;
			this->actor->setLinearVelocity(physx::PxVec3(0, 0, 0));
            this->actor->setGlobalPose(physx::PxTransform(500, 0, 0));
		}

        bool canBeFired = true;
		bool isCurrentlyFired = false;
		WO* bulletTarget = nullptr;
        long long timeFired = 0;
        //auto timeFired = std::chrono::system_clock::now();

    protected:
        WOBulletPhysX();
        virtual void onCreate(const std::string& modelPath, Vector scale, MESH_SHADING_TYPE shadingType, physx::PxPhysics* p, Aftr::Mat4 initialPose = Aftr::Mat4(), float Mass = 1.f);

        physx::PxRigidDynamic* actor = nullptr; // The physical body in the PhysX scene

    };

} // namespace Aftr