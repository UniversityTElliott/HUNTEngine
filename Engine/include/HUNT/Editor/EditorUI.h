
#pragma once

#include <memory>

namespace HUNT {
    class Renderer;
    class SceneHandler;


    class EditorUI {

    public:
        EditorUI(Renderer* renderer, SceneHandler* sceneHandler);
        ~EditorUI();
        void BuildUI();

    private:
        //These are just helpers
        void DrawHierarchy();
        void DrawViewport();
        void DrawInspector();
        void DrawMiscArea();


        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}