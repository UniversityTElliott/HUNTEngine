
#include <HUNT/Editor/EditorUI.h>
#include <HUNT/Math/float2.h>
#include <HUNT/Graphics/Renderer.h>
#include <HUNT/Scene/Scene.h>
#include <HUNT/SceneHandler.h>
#include <imgui.h>

namespace HUNT {

    struct EditorUI::Impl
    {
        Renderer* renderer;
        SceneHandler* sceneHandler;

        //Shows what objects we selected for the unity style editor
        HUNTObject* selectedObject = nullptr;

        explicit Impl(Renderer* renderer, SceneHandler* sceneHandler)
            : renderer(renderer),
            sceneHandler(sceneHandler)
        {
        }
    };


    EditorUI::EditorUI(Renderer* renderer, SceneHandler* sceneHandler)
        : impl_(std::make_unique<Impl>(renderer, sceneHandler))
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
        ImGui::BeginChild("Object List",ImVec2(leftWidth, topHeight),ImGuiChildFlags_Borders);
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

        Scene* currentScene =
            impl_->sceneHandler->GetCurrentScene();

        if (currentScene == nullptr)
        {
            ImGui::Text("No active scene");
            return;
        }

        const auto& objects = currentScene->GetObjects();

        for (const auto& object : objects)
        {
            HUNTObject* objectPtr = object.get();

            bool isSelected = (impl_->selectedObject == objectPtr);

            ImGui::PushID(objectPtr->GetID());
            //CHANGE THIS TO NAME IF I ADD IT
            std::string label ="Object " + std::to_string(objectPtr->GetID());

            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                impl_->selectedObject = objectPtr;
            }

            ImGui::PopID();
        }
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

        ImGui::Text("Inspector");
        ImGui::Separator();

        //If nothing selected leave empty
        if (impl_->selectedObject == nullptr)
        {
            ImGui::Text("No object selected");
            return;
        }

        //If something is selected show ID
        ImGui::Text("Object ID: %d", impl_->selectedObject->GetID());

        ImGui::Separator();

        //Show all components
        //Transform
        ImGui::Text("Transform");
        ImGui::Separator();

        // Get the selected object's Transform
        auto& transform = *impl_->selectedObject->GetComponent<HUNTTransformComponent>();
        bool transformChanged = false;
        //POSITION
        if (ImGui::DragFloat3("Position", &transform.position.x, 0.1f))
        {
            transformChanged = true;
        }
        //ROTATION
        HUNT::quat rotation = glm::normalize(transform.rotation);
        float angle = glm::degrees(glm::angle(rotation));
        float3 axis = glm::axis(rotation);
        //MAke sure no weird angles
        if (angle < 0.001f)
        {
            axis = float3(0.0f, 1.0f, 0.0f);
        }

        if (ImGui::SliderFloat3("Rotation Axis", &axis.x, -1.f,1.f))
        {
            axis = glm::normalize(axis);
            transform.rotation = glm::angleAxis(glm::radians(angle), axis);
            transformChanged = true;
        }

        if (ImGui::SliderFloat("Rotation Angle", &angle, 0.0f, 360.0f))
        {
            transform.rotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
            transformChanged = true;
        }

        //SCALE
        if (ImGui::DragFloat3("Scale (Does not change physics)", &transform.scale.x, 0.1f))
        {
            transformChanged = true;
        }

        //Check if it has a physics body, if it does overwrite it
        auto* rb = impl_->selectedObject->GetComponent<HUNTRigidBodyComponent>();
        if (rb && transformChanged)
        {
            auto* scene = impl_->sceneHandler->GetCurrentScene();

            if (scene)
            {
                //Clunky implement but ONLY LET THE USER CHANGE NON STATIC OBJECS
                auto& bodyInterface = scene->GetPhysicsWorld()->GetPhysicsSystem()->GetBodyInterface();
                if (bodyInterface.GetMotionType(rb->GetBodyID()) != JPH::EMotionType::Static)
                {
                    scene->GetPhysicsWorld()->SetBodyTransform(rb, &transform);
                }
            }
        }
    }

    void EditorUI::DrawMiscArea()
    {
        ImGui::Text("Blank for another feature");
        ImGui::Separator();
    }
}
