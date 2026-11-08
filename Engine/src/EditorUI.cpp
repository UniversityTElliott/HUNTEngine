
#include <HUNT/Editor/EditorUI.h>
#include <HUNT/Math/float2.h>
#include <HUNT/Graphics/Renderer.h>
#include <imgui.h>

namespace HUNT {

    struct EditorUI::Impl
    {
        Renderer* renderer;
        explicit Impl(Renderer* renderer)
            : renderer(renderer)
        {
        }
    };


    EditorUI::EditorUI(Renderer* renderer)
        : impl_(std::make_unique<Impl>(renderer))
    {
    }
    EditorUI::~EditorUI() = default;
    //Using imgui vec2 cuz I dont wanna write a wrapper
    //TODO MAYBE WRITE A WRAPPER FOR IT BUT ITS BACKEND SO IDK
    void EditorUI::BuildUI() {

        ImGuiViewport* mainViewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(mainViewport->WorkPos);
        ImGui::SetNextWindowSize(mainViewport->WorkSize);

        ImGuiWindowFlags windowFlags =ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoCollapse |ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("HUNT Editor",nullptr,windowFlags);

        ImVec2 available =ImGui::GetContentRegionAvail();

        // Layout proportions
        float leftWidth = available.x * 0.18f;
        float rightWidth = available.x * 0.20f;

        float bottomHeight = available.y * 0.30f;
        float topHeight = available.y - bottomHeight;

        //ACtual UI

        ImGui::BeginChild("TopSection",ImVec2(available.x, topHeight),ImGuiChildFlags_Borders);

        //Hierarchy
        ImGui::BeginChild("Hierarchy",ImVec2(leftWidth, topHeight),ImGuiChildFlags_Borders);
        DrawHierarchy();
        ImGui::EndChild();
        ImGui::SameLine();

        //Viewport
        ImGui::BeginChild("Viewport",ImVec2(available.x - leftWidth - rightWidth,topHeight),ImGuiChildFlags_Borders);
        DrawViewport();
        ImGui::EndChild();
        ImGui::SameLine();

        // RIGHT: Inspector
        ImGui::BeginChild("Inspector",ImVec2(rightWidth, topHeight),ImGuiChildFlags_Borders);
        DrawInspector();
        ImGui::EndChild();
        ImGui::EndChild();
  

        ImGui::BeginChild("BottomPanel", ImVec2(available.x, bottomHeight), ImGuiChildFlags_Borders);
        DrawMiscArea();
        ImGui::EndChild();
        ImGui::End();

    }

    void EditorUI::DrawHierarchy()
    {
        ImGui::Text("Hierarchy");
        ImGui::Separator();
    }

    void EditorUI::DrawViewport()
    {
        ImGui::BeginChild("Viewport");

        ImVec2 viewportSize =ImGui::GetContentRegionAvail();
      
        // Tell the renderer how large the viewport is.
        impl_->renderer->SetViewportSize(viewportSize.x,viewportSize.y);

        SDL_GPUTexture* viewportTexture =impl_->renderer->GetViewportTexture();

        if (viewportTexture != nullptr)
        {
            ImGui::Image((ImTextureID)viewportTexture,viewportSize);
        }

        ImGui::EndChild();

    }

    void EditorUI::DrawInspector()
    {

        ImGui::Text("Transform");
        ImGui::Separator();
    }

    void EditorUI::DrawMiscArea()
    {
        ImGui::Text("Blank for another feature");
        ImGui::Separator();
    }

}
