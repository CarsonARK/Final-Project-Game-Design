#include "WOController.h"
#include "AftrImGui_Assign_5.h"
#include <PxPhysicsAPI.h>  
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include "GLViewAssign_5.h"
/*
need to create a WOAimingController or something 


*/
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

    WOController* WOController::New(const std::string& modelPath, Vector scale, uint8_t playerNum, MESH_SHADING_TYPE shadingType, physx::PxPhysics* p, Aftr::Mat4 initialPose, float Mass, Vector position) {
        WOController* wo = new WOController();
        wo->onCreate(modelPath, scale, playerNum, shadingType, p, initialPose, Mass, position);
        return wo;
    }

    WOController::~WOController() {
    }

    void WOController::onCreate(const std::string& modelPath, Vector scale, uint8_t playerNum, MESH_SHADING_TYPE shadingType, physx::PxPhysics* p, Aftr::Mat4 initialPose, float Mass, Vector position) {
        WO::onCreate(modelPath, scale, shadingType);
        physx::PxMaterial* material = p->createMaterial(0.5f, 0.5f, 0.5f); // static friction, dynamic friction, restitution
        physx::PxTransform t = aftrMat4ToPxTransform(initialPose);

        
        physx::PxCapsuleControllerDesc desc;
        desc.height = 4.0f;
        desc.radius = 0.5f;
        desc.slopeLimit = .0f; // Allow climbing up to 45 degrees
        desc.stepOffset = 3.f;
        desc.material = material;
		desc.position = physx::PxExtendedVec3(t.p.x, t.p.y, t.p.z);
		desc.upDirection = physx::PxVec3(position.x, position.y, position.z);
        // Crucial: Provide a pointer to the WO in the userData for later retrieval
        desc.userData = this;

        GLViewAssign_5* glv = GLViewAssign_5::New({});
        physx::PxControllerManager* myControllerManager = glv->getPhysXControllerManager();
        physx::PxController* ctrl = myControllerManager->createController(desc);
		glv->setMainController(ctrl, playerNum);
        std::cout << "Main Controller Set!";
		this->controller = ctrl;

        Vector modelNormal = initialPose.toAxisAngle_fromRotationMat().toVec();
        std::cout << "MODEL NORRRMAL" << modelNormal.toString() << std::endl;
        //this->setPose(initialPose);
        this->getModel()->setLookDirection(modelNormal);



        //now i need to create the look component, first I'll create a sphere
        //WOAimingController or something?
        
    } // namespace Aftr\

    /*
    * this->moon = WO::New();
        MGLIndexedGeometry* mglSphere = MGLIndexedGeometry::New(this->moon);
        IndexedGeometrySphereTriStrip* geoSphere = IndexedGeometrySphereTriStrip::New(3.0f, 12, 12, true, true);
        mglSphere->setIndexedGeometry(geoSphere);
        this->moon->setModel(mglSphere);
        this->moon->setLabel("Moon");
        this->moon->setPosition({ 15,15,15 });
        this->moon->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
        this->worldLst->push_back(this->moon);

        //Place a texture on the sphere, now its a moon
        fmt::print("To the moon...\n");
        Tex tex = *ManagerTex::loadTexAsync(ManagerEnvironmentConfiguration::getSMM() + "/images/moonMap.jpg");
        this->moon->getModel()->getSkin().getMultiTextureSet().at(0) = tex;

        this->moon->setPosition({ 15,2,10 });


        WOController* cube = WOController::New(tank1, Vector(1.0f, 1.0f, 1.0f), 0, MESH_SHADING_TYPE::mstAUTO, this->getPhysXSDK(), startingPose, 10, Vector(0,0,1));
    cube->setPosition(startingPosition);
    cube->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    cube->upon_async_model_loaded([cube]()
        {
            ModelMeshSkin& skin = cube->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
            skin.setAmbient(aftrColor4f(0.1f, 0.1f, 0.1f, 1.0f)); //Color of object when it is not in any light
            //skin.setDiffuse( aftrColor4f( .1f, .1f, .5f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object) // Make it blue? Why not?
            skin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
            skin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
        });
    cube->setLabel(label);
    this->getWorldContainer()->push_back(cube);


    */
    void WOController::updatePoseFromPhysicsEngine()
    {
        //return;
        // If no physics actor is assigned, we have nothing to sync
        if (this->controller == nullptr) return;
        auto pos = this->controller->getPosition();
        this->setPosition(pos.x, pos.y, pos.z);
		aimingController->setPosition(pos.x, pos.y, pos.z + 6);
    }

    // RIGHT: Let the WO constructor handle IFace for you
    // Instead of WOPhysX() : WO()
    WOController::WOController() : IFace((WO*)this), WO()
    {
        this->controller = nullptr;
    }
}

