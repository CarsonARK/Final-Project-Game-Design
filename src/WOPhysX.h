#pragma once
#include "WO.h"
#include "PxPhysicsAPI.h"

namespace Aftr {

    class WOPhysX : public WO {
    public:
        // Factory method
        static WOPhysX* New(const std::string& modelPath, Vector scale = Vector(1, 1, 1), MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO, physx::PxPhysics* p = nullptr, Aftr::Mat4 initialPose = Aftr::Mat4(), float Mass = 1.f);

        virtual ~WOPhysX();

        // The "Bridge": Call this every frame to move the visual model to the physics location
        virtual void updatePoseFromPhysicsEngine();

        // Getter/Setter for the PhysX Actor
        physx::PxRigidDynamic* getPhysXActor() { return this->actor; }
        void setPhysXActor(physx::PxRigidDynamic* actor) { this->actor = actor; }

    protected:
        WOPhysX();
        virtual void onCreate(const std::string& modelPath, Vector scale, MESH_SHADING_TYPE shadingType, physx::PxPhysics* p, Aftr::Mat4 initialPose = Aftr::Mat4(), float Mass = 1.f);

        physx::PxRigidDynamic* actor = nullptr; // The physical body in the PhysX scene
    };

} // namespace Aftr