#include "AftrImGui_Assign_5.h"
#include "AftrImGuiIncludes.h"
#include "WO.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include <fmt/core.h>
#include <chrono>
#include "Vector.h"
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
#include "GLViewAssign_5.h"

#include "GameManager.h"
void Aftr::AftrImGui_assign_2::draw()
{
    this->draw_orbit_controls();
}

void Aftr::AftrImGui_assign_2::draw_orbit_controls()
{
    //auto DrawList = ImGui::GetForegroundDrawList();
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    float size = 10.0f;
    float thickness = 2.0f; 
    ImU32 color = IM_COL32(255, 0, 200, 220); // RGBA color (white, slightly transparent)
    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    // Draw the horizontal line
    drawList->AddLine(
        ImVec2(center.x - size, center.y),
        ImVec2(center.x + size, center.y),
        color,
        thickness
    );

    // Draw the vertical line
    drawList->AddLine(
        ImVec2(center.x, center.y - size),
        ImVec2(center.x, center.y + size),
        color,
        thickness
    );

    std::string drawText;
    GameManager& gm = GameManager::getInstance();
    if (gm.isGameEnded) {
		drawText = "Game Over! \n Winner: Player " + std::to_string(gm.playerOne ? 1 : 2);
    }
    else {
		drawText = "Player " + std::to_string(gm.playerOne ? 1 : 2) + "'s Turn";
		drawText += "\nFuel: " + std::to_string(gm.getActiveTank().CurrentFuel);
		drawText += "\nHealth: " + std::to_string(gm.getActiveTank().CurrentHealth);
		drawText += "\nOpponent Health: " + std::to_string(gm.getInactiveTank().CurrentHealth);
		drawText += "\nShot Power: " + std::to_string(gm.getActiveTank().shotPower);
	}

    //this just highlights it, i dislike the normal highlight text
    drawList->AddText(ImVec2(1+ center.x / 2, 1 +center.y / 2), IM_COL32(0, 0, 0, 255), drawText.c_str());
    drawList->AddText(ImVec2(1+ center.x / 2, -1 + center.y / 2), IM_COL32(0, 0, 0, 255), drawText.c_str());
    drawList->AddText(ImVec2(-1 +center.x / 2, 1 + center.y / 2), IM_COL32(0, 0, 0, 255), drawText.c_str());
    drawList->AddText(ImVec2(-1 +center.x / 2, -1 +center.y / 2), IM_COL32(0, 0, 0, 255), drawText.c_str());


	drawList->AddText(ImVec2(center.x /2 , center.y / 2 ), IM_COL32(255, 0, 255, 255), drawText.c_str());
        /*
        ImGui::Text("Player %d's Turn", gm.playerOne ? 1 : 2);
        ImGui::Text("Fuel: %.2f", gm.getActiveTank().CurrentFuel);
        ImGui::Text("Health: %.2f", gm.getActiveTank().CurrentHealth);
        ImGui::Text("Opponent Health: %.2f", gm.getInactiveTank().CurrentHealth);
        ImGui::SliderFloat("Shot Power", &gm.getActiveTank().shotPower, 0.1f, 1.0f); */
}

void Aftr::AftrImGui_assign_2::clear_wo_vec() {
    wo_vec.clear();
}
void Aftr::AftrImGui_assign_2::push_to_wo_vec(WO* wo) {
    wo_vec.push_back(wo);
}
Aftr::Mat4 Aftr::AftrImGui_assign_2::compute_pose(Mat4 const& origin)
{
    return Mat4();
}