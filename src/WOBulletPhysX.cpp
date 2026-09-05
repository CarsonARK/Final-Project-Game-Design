#include "WOBulletPhysx.h"
#include "GameManager.h"
static physx::PxTransform aftrMat4ToPxTransform(Aftr::Mat4 m)
{
    // 1. Get the Translation (Position)
    // The 4th column of an OpenGL/AFTR matrix holds the position (x, y, z)
    Aftr::Vector pos = m.getPosition();
    physx::PxVec3 pxPos(pos.x, pos.y, pos.z);

    // 2. Get the Rotation (Quaternion)
    // AFTR Mat4 has a built-in method to convert its 3x3 rotation part to a Quat
    Aftr::Quat aftrQuat = m.toQuat_fromRotationMat();

    // AFTR Quat is (x, y, z, w), PhysX Quat is also (x, y, z, w)
    physx::PxQuat pxQuat(aftrQuat.a, aftrQuat.b, aftrQuat.c, aftrQuat.d);

    // 3. Return the combined Transform
    return physx::PxTransform(pxPos, pxQuat);
}
namespace Aftr {

    WOBulletPhysX* WOBulletPhysX::New(const std::string& modelPath, Vector scale, MESH_SHADING_TYPE shadingType, physx::PxPhysics* p, Aftr::Mat4 initialPose, float Mass) {
        WOBulletPhysX* wo = new WOBulletPhysX();
        wo->onCreate(modelPath, scale, shadingType, p, initialPose, Mass);
        return wo;
    }

    WOBulletPhysX::~WOBulletPhysX() {
    }

    void WOBulletPhysX::onCreate(const std::string& modelPath, Vector scale, MESH_SHADING_TYPE shadingType, physx::PxPhysics* p, Aftr::Mat4 initialPose, float Mass) {
        WO::onCreate(modelPath, scale, shadingType);
        physx::PxMaterial* material = p->createMaterial(0.5f, 0.5f, 0.5f); // static friction, dynamic friction, restitution
        physx::PxShape* shape = p->createShape(physx::PxBoxGeometry(1.0f, 1.0f, 1.0f), *material, true);
        physx::PxTransform t = aftrMat4ToPxTransform(initialPose);


        physx::PxFilterData filterData;
        filterData.word0 = 1; // My identity (Group 1)
        filterData.word1 = 1; // Who I collide with (Group 1)
        shape->setSimulationFilterData(filterData);

        actor = p->createRigidDynamic(t);
        actor->attachShape(*shape);
        actor->userData = this; // Link the PhysX actor back to this WOPhysX instance for easy access in callbacks
        physx::PxRigidBodyExt::updateMassAndInertia(*(this->actor), 10.0f);
        this->setPhysXActor(actor);


    } // namespace Aftr
    void WOBulletPhysX::updatePoseFromPhysicsEngine()
    {
        // If no physics actor is assigned, we have nothing to sync
        if (this->getPhysXActor() == nullptr) return;

        // 1. Retrieve the global pose (Position + Orientation) from PhysX
        physx::PxTransform pxT = this->actor->getGlobalPose();

        // 2. Convert the PhysX Transform into a 4x4 Matrix
        // We use PxMat44 because it's already in column-major format (perfect for OpenGL/AFTR)
        physx::PxMat44 m(pxT);

        // 3. Extract the raw float array from the matrix and pass it to the AFTR Model
        // AFTR's Mat4 constructor accepts a float[16]
        float rawMatrix[16];

        int incr = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                rawMatrix[incr++] = m[i][j]; // Initialize to zero for safety
            }
        }
        //physx::PxTransform pxTransform = 

        this->setPose(Aftr::Mat4(rawMatrix));
        //this->setPosition(Aftr::Vector(pxT.p.x, pxT.p.y, pxT.p.z));

		//check timestamp for despawning, 10 seconds after firing, despawn the bullet
        long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		if (currentTime - timeFired > 10000 && isCurrentlyFired) {
            onFinishedFiring();
            GameManager::getInstance().NextTurn(); // Switch to the next player's turn
        }

		if (isCurrentlyFired && bulletTarget != nullptr) {
            Vector targetPos = bulletTarget->getPosition();
            Vector bulletPos = this->getPosition();
            float distance = (targetPos - bulletPos).magnitude();

			std::cout << "Distance to target: " << distance << std::endl;
            if(distance < 9.0f) { // If the bullet is within 1 unit of the target
                onFinishedFiring();
				GameManager::getInstance().DealDamage(20.0f); // Deal 20 damage to the target
                GameManager::getInstance().NextTurn(); // Switch to the next player's turn
            }
        }



    }

    // RIGHT: Let the WO constructor handle IFace for you
    // Instead of WOPhysX() : WO()
    WOBulletPhysX::WOBulletPhysX() : IFace((WO*)this), WO()
    {
        this->actor = nullptr;
    }
}

