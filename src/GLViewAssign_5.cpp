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
#include "SoundManager.h"
using namespace Aftr;

GLViewAssign_5* GLViewAssign_5::New(const std::vector< std::string >& args)
{
    static bool first = false;
    static GLViewAssign_5* glv = new GLViewAssign_5(args);
    if (!first) {
        first = true;
        glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
        glv->onCreate();
    }
    return glv;
}


GLViewAssign_5::GLViewAssign_5(const std::vector< std::string >& args) : GLView(args)
{
    //Initialize any member variables that need to be used inside of LoadMap() here.
    //Note: At this point, the Managers are not yet initialized. The Engine initialization
    //occurs immediately after this method returns (see GLViewAssign_5::New() for
    //reference). Then the engine invoke's GLView::loadMap() for this module.
    //After loadMap() returns, GLView::onCreate is finally invoked.

    //The order of execution of a module startup:
    //GLView::New() is invoked:
    //    calls GLView::init()
    //       calls GLView::loadMap() (as well as initializing the engine's Managers)
    //    calls GLView::onCreate()

    //GLViewAssign_5::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewAssign_5::onCreate()
{
    //GLViewAssign_5::onCreate() is invoked after this module's LoadMap() is completed.
    //At this point, all the managers are initialized. That is, the engine is fully initialized.

    if (this->pe != NULL)
    {
        //optionally, change gravity direction and magnitude here
        //The user could load these values from the module's aftr.conf
        this->pe->setGravityNormalizedVector(Vector(0, 0, -4.90f));
        this->pe->setGravityScalar(Aftr::GRAVITY);
    }
    this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode
    //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewAssign_5::~GLViewAssign_5()
{
    //Implicitly calls GLView::~GLView()
}

#include "WOPhysX.h"
#include "WOController.h"

bool isWPressed = false;
bool isAPressed = false;
bool isDPressed = false;
bool isSPressed = false;
bool isSpacePressed = false;
static bool isCameraAbove = false;
void GLViewAssign_5::updateWorld()
{
    GLView::updateWorld(); 


	//this just updates imgui with which objects are in the worldLst
    this->orbit_gui.clear_wo_vec();
    auto wl = this->worldLst;
    for (size_t it = 0; it < wl->size(); ++it) {
        this->orbit_gui.push_to_wo_vec(wl->at(it));
    }


    this->orbit_gui.set_wo_cam(this->cam);

    GameManager& gm = GameManager::getInstance();
	gm.Tick();
}

void GLViewAssign_5::onResizeWindow(GLsizei width, GLsizei height)
{
    GLView::onResizeWindow(width, height); //call parent's resize method.
}

void GLViewAssign_5::onMouseDown(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseDown(e);
    if (e.button == SDL_BUTTON_LEFT)
        GameManager::getInstance().currentInputState.isLeftClickHeld = true;
    if (e.button == SDL_BUTTON_RIGHT)
        GameManager::getInstance().currentInputState.isRightClickHeld = true;
	
}


void GLViewAssign_5::onMouseUp(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseUp(e);
    if (e.button == SDL_BUTTON_LEFT)
        GameManager::getInstance().currentInputState.isLeftClickHeld = false;
    if (e.button == SDL_BUTTON_RIGHT)
        GameManager::getInstance().currentInputState.isRightClickHeld = false;

}


void GLViewAssign_5::onMouseMove(const SDL_MouseMotionEvent& e)
{
    // This hides the cursor and locks its movement relative to the window for free-look camera movement
    
	GameManager& gm = GameManager::getInstance();
	if (gm.getActiveTank().isAiming) {
        gm.getActiveTank().aimingComponent->getModel()->rotateAboutGlobalZ(-static_cast<float>(e.xrel) * .0025f);
        gm.getActiveTank().aimingComponent->rotateAboutRelY(static_cast<float>(e.yrel) * .001f);
    }
    else {
        gm.getActiveTank().tankObject->rotateAboutGlobalZ(-static_cast<float>(e.xrel) * .005f);
    }
	//gm.currentInputState.mouseX = static_cast<float>(e.xrel);
	//gm.currentInputState.mouseY = static_cast<float>(e.yrel);
    //GLView::onMouseMove(e);
}


void GLViewAssign_5::onKeyDown(const SDL_KeyboardEvent& key)
{
    GLView::onKeyDown(key);
	if (key.keysym.sym == SDLK_w)
    {
        GameManager::getInstance().currentInputState.isWPressed = true;
    }
    if (key.keysym.sym == SDLK_a)
    {
        GameManager::getInstance().currentInputState.isAPressed = true;
    }
    if (key.keysym.sym == SDLK_f)
    {
        GameManager::getInstance().currentInputState.isFPressed = true;
    }
    if (key.keysym.sym == SDLK_s)
    {
        GameManager::getInstance().currentInputState.isSPressed = true;
    }
    if (key.keysym.sym == SDLK_d)
    {
        GameManager::getInstance().currentInputState.isDPressed = true;
    }
    if (key.keysym.sym == SDLK_SPACE)
    {
        GameManager::getInstance().currentInputState.isSpacePressed = true;
    }
	if (key.keysym.sym == SDLK_c)
    {
		GameManager::getInstance().currentInputState.isCPressed = true;
    }
    if(key.keysym.sym == SDLK_0)
    {
		GameManager::getInstance().setActivePlayer(0);
    }
    if (key.keysym.sym == SDLK_1)
	{
        GameManager::getInstance().setActivePlayer(1);
	}
    if (key.keysym.sym == SDLK_UP)
    {
		GameManager::getInstance().getActiveTank().shotPower += 0.1f;
    }
    if (key.keysym.sym == SDLK_DOWN)
    {
        GameManager::getInstance().getActiveTank().shotPower -= 0.1f;
    }
}
#include <cmath>
void GLViewAssign_5::initCamera(float xLoc, float yLoc, float zLoc, float phi, float theta) {
	std::cout << "GLViewAssign_5::initCamera() called with parameters: " << std::endl;
	return GLView::initCamera(xLoc, yLoc, zLoc, phi, theta);
}

void GLViewAssign_5::onKeyUp(const SDL_KeyboardEvent& key)
{
    GLView::onKeyUp(key);
    if (key.keysym.sym == SDLK_w)
    {
        GameManager::getInstance().currentInputState.isWPressed = false;
    }
    if (key.keysym.sym == SDLK_a)
    {
        GameManager::getInstance().currentInputState.isAPressed = false;
    }
    if (key.keysym.sym == SDLK_s)
    {
        GameManager::getInstance().currentInputState.isSPressed = false;
    }
    if (key.keysym.sym == SDLK_d)
    {
        GameManager::getInstance().currentInputState.isDPressed = false;
    }
}
#include "WOGrid.h"
#include <fstream>
#include <vector>
#include "WOGridPhysx.h"


void Aftr::GLViewAssign_5::loadMap()
{
    this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
    this->actorLst = new WorldList();
    this->netLst = new WorldList();


    this->foundation = PxCreateFoundation(PX_PHYSICS_VERSION, this->gDefaultAllocatorArray, this->gDefaultErrorCallback);

    physx::PxPvd* pvd = physx::PxCreatePvd(*foundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), true, pvd);


    physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0, 0, -9.805f);
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
    scene = physics->createScene(sceneDesc);

	this->myControllerManager = PxCreateControllerManager(*scene);
    // Link the scene to PVD
    if (scene->getScenePvdClient()) {
        scene->getScenePvdClient()->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        scene->getScenePvdClient()->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        scene->getScenePvdClient()->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }


    this->client = NetMessengerClient::New();
	//this->client->port = "12684"; //The port that this client will send from and listen for replies on. Must be different than the server's port (12683 by default)

    ManagerOpenGLState::GL_CLIPPING_PLANE(1000.0);
    ManagerOpenGLState::GL_NEAR_PLANE(0.1f);
    ManagerOpenGLState::enableFrustumCulling(false);
    Axes::isVisible = true;
    this->glRenderer->isUsingShadowMapping(false); //set to TRUE to enable shadow mapping, must be using GL 3.2+

    this->cam->setPosition(0, 0, 10);
	this->ActorChaseType = CAM_CHASE_TYPE::CHASEACTORLOOK3; //Default is STANDARDEZNAV mode


    SoundManager::getSoundManager().initializeSounds();
    

    std::vector< std::string > skyBoxImageNames;
    skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg");
    {
        //Create the SkyBox
        WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
        wo->setPosition(Vector(0, 0, 0));
        wo->setLabel("Sky Box");
        wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        worldLst->push_back(wo);
    }



    // Let's make a GUI. We create a WOImGui instance, and then use the strategy pattern to
    // submit/subscribe lambdas/std::functions/callbacks (~same thing) to draw our desired widgets.
    // We nestle the callbacks inside a menu to keep everything organized. The menu uses an on/off
    // toggle -- when on, that menu item's corresponding callback will be invoked each frame, when
    // off that callback is not invoked and therefore the corresponding window is not drawn.

    {
        this->gui = WOImGui::New(nullptr);
        gui->setLabel("My Gui");
        //callbacks -- When the user toggles the checkbox from the menu, call these callbacks:

        //This callback shows the WOEditor window. It will be visible when the user
        //selected Menu -> Edit -> Show WO Editor (as linked up below).
        auto woEditFunc = [this]() { this->wo_editor.draw(this->getLastSelectionQuery(), *this->getWorldContainer(), this->getCamera_functor()); };

        //We will put these demo items under the "Demo" menu
        auto showDemoWindow_ImGui = [this]() { ImGui::ShowDemoWindow(); };
        auto showDemoWindow_AftrDemo = [this]() { WOImGui::draw_AftrImGui_Demo(this->gui); };
        auto showDemoWindow_ImGuiPlot = [this]() { ImPlot::ShowDemoWindow(); };
        auto show_moon_orbit_params = [this]() { this->orbit_gui.draw(); };
        
        this->gui->subscribe_drawImGuiWidget(
            [=, this]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
            {
                //We defined the callbacks above, now hook them into the menu labels
                menu.attach("Edit", "Show WO Editor", woEditFunc);
                menu.attach("Demos", "Show Default ImGui Demo", showDemoWindow_ImGui);
                menu.attach("Demos", "Show Default ImPlot Demo", showDemoWindow_ImGuiPlot);
                menu.attach("Demos", "Show Aftr ImGui w/ Markdown & File Dialogs", showDemoWindow_AftrDemo);
                menu.attach("Orbit Gui", "Show Orbit", show_moon_orbit_params, true);
                menu.draw(); //The menu.draw() is the entry point for your gui. It is called once per frame to draw the GUI.
            });
        this->worldLst->push_back(this->gui);
    }

    this->cam->setCameraLookDirection(Vector(1, 0, 0));
    Mat4 CameraPose = this->cam->getPose();

    Vector startingPosition = Vector(20, 20, 20);
    Vector startingPosition2 = Vector(120, 120, 20);
    Vector startingRotation = Vector(1, 0, 0).normalizeMe();

    WO* aimingComponent1 = WO::New();
    MGLIndexedGeometry* mglSphere = MGLIndexedGeometry::New(aimingComponent1);
    aimingComponent1->setModel(mglSphere);
    aimingComponent1->setLabel("AimComp1");
    aimingComponent1->setPosition({ 15,15,15 });
    aimingComponent1->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
    this->worldLst->push_back(aimingComponent1);



    WO* aimingComponent2 = WO::New();
    MGLIndexedGeometry* mglSphere2 = MGLIndexedGeometry::New(aimingComponent2);
    aimingComponent2->setModel(mglSphere2);
    aimingComponent2->setLabel("AimComp2");
    aimingComponent2->setPosition({ 15,15,15 });
    aimingComponent2->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
    this->worldLst->push_back(aimingComponent2);


    //-1 0 0 is flat but pointed wrong
    //Main Controller Set!MODEL NORRRMAL(-0.000, -1.000, 0.000)

    Mat4 startingPose;
    startingPose.setMeToIdentity();
    startingPose = startingPose.translate(startingPosition);


    Vector worldUp(0, 0, 1);
    Vector right = startingRotation.crossProduct(worldUp).normalizeMe();
    Vector up = right.crossProduct(startingRotation).normalizeMe();

    startingPose.setVecCol(0, right);
    startingPose.setVecCol(1, startingRotation);
    startingPose.setVecCol(2, up);



    Mat4 startingPose2;
    startingPose2.setMeToIdentity();
    startingPose2 = startingPose.translate(startingPosition2);

    startingPose2.setVecCol(0, right);
    startingPose2.setVecCol(1, startingRotation);
    startingPose2.setVecCol(2, up);


    std::string label = "TankController1";


    std::string tank1(ManagerEnvironmentConfiguration::getLMM() + "/models/modifiedtank2.obj");
    WOController* cube = WOController::New(tank1, Vector(1.0f, 1.0f, 1.0f), 0, MESH_SHADING_TYPE::mstAUTO, this->getPhysXSDK(), startingPose, 10, Vector(0,0,1));
	cube->setPosition(startingPosition);
    cube->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    cube->upon_async_model_loaded([cube]()
        {
            for (size_t i = 0; i < cube->getModel()->getModelDataShared()->getModelMeshes().size(); i++) {
                ModelMeshSkin& skin = cube->getModel()->getModelDataShared()->getModelMeshes().at(i)->getSkins().at(0);
                skin.setAmbient(aftrColor4f(0.7f, 0.0f, 0.0f, 1.0f)); //Color of object when it is not in any light
                skin.setSpecular(aftrColor4f(0.1f, 0.1f, 0.1f, 0.3f)); //Specular color component (ie, how "shiney" it is)
                skin.setDiffuse(aftrColor4f(1.f, 0.0f, 0.1f, 0.3f)); //Diffuse color component (ie, how "bright" it is)
                skin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
            }
        });
    cube->setLabel(label);
	cube->setAimingController(aimingComponent1);
    this->getWorldContainer()->push_back(cube);


    std::string label2 = "TankControiller2";
    WOController* cube2 = WOController::New(tank1, Vector(1.0f, 1.0f, 1.0f), 1, MESH_SHADING_TYPE::mstAUTO, this->getPhysXSDK(), startingPose2, 10, Vector(0, 0, 1));
    cube2->setPosition(startingPosition2);
    cube2->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
    cube2->upon_async_model_loaded([cube2]()
        {
            for (size_t i = 0; i < cube2->getModel()->getModelDataShared()->getModelMeshes().size(); i++) {
                ModelMeshSkin& skin = cube2->getModel()->getModelDataShared()->getModelMeshes().at(i)->getSkins().at(0);
                skin.setAmbient(aftrColor4f(0.0f, 0.7f, 0.0f, 1.0f)); //Color of object when it is not in any light
                skin.setSpecular(aftrColor4f(0.1f, 0.1f, 0.1f, 0.3f)); //Specular color component (ie, how "shiney" it is)
				skin.setDiffuse(aftrColor4f(0.0f, 1.0f, 0.1f, 1.0f)); //Diffuse color component (ie, how "bright" it is)
                skin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
            }
        });
    cube2->setLabel(label2);
    cube2->setAimingController(aimingComponent2);
    this->getWorldContainer()->push_back(cube2);

    /*
    void InitializeTank(bool tankOne, Aftr::WO* tankObject, Aftr::WO* aimingComponent, physx::PxController* tankController) {
    */

	GameManager& gm = GameManager::getInstance();
	gm.InitializeTank(true, cube, aimingComponent1, cube->getPhysXController());
    gm.InitializeTank(false, cube2, aimingComponent2, cube2->getPhysXController());

    const int DIM = 512;
    std::string mtn_height(ManagerEnvironmentConfiguration::getLMM() + "/models/testcustom.raw");
    std::ifstream file(mtn_height, std::ios::binary);

    // Initialize your 2D grid
    std::vector<std::vector<VectorD>> hgrid (DIM, std::vector<VectorD>(DIM));

    if (file.is_open()) {
        for (int y = 0; y < DIM; ++y) {
            for (int x = 0; x < DIM; ++x) {
                float zElevation;
                // Read 4 bytes (Float32) directly into our float variable
                file.read(reinterpret_cast<char*>(&zElevation), sizeof(float));


                //zElevation -= 1048.f;
                //zElevation *= .03f;

				hgrid[y][x] = VectorD(static_cast<double>(x-120), static_cast<double>(y-120), static_cast<double>(zElevation));
            }
        }
        file.close();
    }

    WOGridPhysx* physxGrid = WOGridPhysx::New(foundation, physics, scene, hgrid);
    if (physxGrid != nullptr) {
        std::cout << "uhhh adding gridd!!" << std::endl;
        physxGrid->setLabel("hmap");
        worldLst->push_back(physxGrid);
    }
}