#pragma once
#pragma once
#include "WO.h"
#include "PxPhysicsAPI.h"
#include "WorldList.h" //This is where we place all of our WOs
namespace Aftr {

    //this class controols the TANKS
    class WOController : public WO {
    public:
        // Factory method
        static WOController* New(const std::string& modelPath, Vector scale = Vector(1, 1, 1), uint8_t playerNum = 0, MESH_SHADING_TYPE shadingType = MESH_SHADING_TYPE::mstAUTO, physx::PxPhysics* p = nullptr, Aftr::Mat4 initialPose = Aftr::Mat4(), float Mass = 1.f, Vector position = Vector(0, 1,0));

        virtual ~WOController();

        // The "Bridge": Call this every frame to move the visual model to the physics location
        virtual void updatePoseFromPhysicsEngine();

		void setAimingController(WO* aimingController) { this->aimingController = aimingController; }
		WO* getAimingController() { return this->aimingController; }
		physx::PxController* getPhysXController() { return this->controller; }
    protected:
        WOController();
        virtual void onCreate(const std::string& modelPath, Vector scale, uint8_t playerNum, MESH_SHADING_TYPE shadingType, physx::PxPhysics* p, Aftr::Mat4 initialPose = Aftr::Mat4(), float Mass = 1.f, Vector position = Vector(0,1,0));

        physx::PxController* controller = nullptr; // The physical body in the PhysX scene
		WO* aimingController = nullptr; // The aiming object associated with this WOController
    };

} // namespace Aftr