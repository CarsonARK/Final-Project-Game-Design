#pragma once

#include "GLView.h"
#include "AftrImGui_MenuBar.h"
#include "AftrImGui_WO_Editor.h"
#include "AftrImGui_Assign_5.h"
#include <PxPhysicsAPI.h>  
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>

#include "WOPhysX.h"
namespace Aftr
{
    class Camera;
    class WOImGui;

    /**
       \class GLViewAssign_5
       \author Scott Nykl
       \brief A child of an abstract GLView. This class is the top-most manager of the module.

       Read \see GLView for important constructor and init information.

       \see GLView

        \{
    */

    class GLViewAssign_5 : public GLView
    {
    public:
        static GLViewAssign_5* New(const std::vector< std::string >& outArgs);
        virtual ~GLViewAssign_5();
        virtual void updateWorld() override; ///< Called once per frame
        virtual void loadMap() override; ///< Called once at startup to build this module's scene
        virtual void onResizeWindow(GLsizei width, GLsizei height) override;
        virtual void onMouseDown(const SDL_MouseButtonEvent& e) override;
        virtual void onMouseUp(const SDL_MouseButtonEvent& e) override;
        virtual void onMouseMove(const SDL_MouseMotionEvent& e) override;
        virtual void onKeyDown(const SDL_KeyboardEvent& key) override;
        virtual void onKeyUp(const SDL_KeyboardEvent& key) override;
		
        std::shared_ptr<NetMessengerClient> getClient() { return this->client; }
		physx::PxScene* getPhysXScene() { return this->scene; }
		physx::PxPhysics* getPhysXSDK() { return this->physics; }
		physx::PxControllerManager* getPhysXControllerManager() { return this->myControllerManager; }
        Camera* getCam() { return this->cam; }
		void setMainController(physx::PxController* controller, uint8_t index) { this->Controllers[index] = controller; }
        float GravityX = 0.0f;
        float GravityZ = -4.9f;
        float GravityY = 0.0f;
        void SetActiveControllerFlag(uint8_t flag) { this->ControllerFlag = flag; }
        uint8_t getActiveControllerFlag() { return this->ControllerFlag; }
		physx::PxController* getMainController(uint8_t index = 5) {
			if (index > 1) index = this->ControllerFlag;
			return this->Controllers[index];
        }
    protected:
        GLViewAssign_5(const std::vector< std::string >& args);
        virtual void onCreate();
        virtual void initCamera(float xLoc = 15, float yLoc = 15, float zLoc = 10, float phi = 0, float theta = 0) override;
        WOImGui* gui = nullptr; //The GUI which contains all ImGui widgets
        AftrImGui_MenuBar menu;      //The Menu bar at the top of the GUI window
        AftrImGui_WO_Editor wo_editor;//The WO Editor to mutate a selected WO
        AftrImGui_assign_2 orbit_gui;
        WO* moon = nullptr;
        WO* gulfstream = nullptr;
        std::shared_ptr<NetMessengerClient> client;

        physx::PxFoundation* foundation;
        physx::PxPhysics* physics;
        physx::PxScene* scene;
        physx::PxDefaultErrorCallback gDefaultErrorCallback;
        physx::PxDefaultAllocator gDefaultAllocatorArray;
        physx::PxControllerManager* myControllerManager = nullptr;


		uint8_t ControllerFlag = 0; //0 = player1 tank, 1 = player2 tank, 2 = player1 aiming component, 3 = player2 aiming component
		physx::PxController* Controllers[2] = { nullptr, nullptr}; //0 = player1 tank, 1 = player2 tank, 2 = player1 aiming component, 3 = player2 aiming component
        
    };

    /** \} */

} //namespace Aftr
