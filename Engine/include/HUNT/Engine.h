#pragma once

#include <cstdint>
#include <memory>

namespace HUNT {

    class Renderer;
    class Engine;
    // Application is what the end developer builds to use your engine.
    // Its lifetime is managed by the engine. The end developer respondes to the calls to Start, Render, etc
    class Application {
    public:
        virtual ~Application() = default;
        virtual bool Start(Engine& engine) = 0;
        virtual void FixedUpdate(Engine& engine, float dt) = 0;
        virtual void Render(Engine& engine) = 0;
        virtual void Stop(Engine& engine) = 0;
    };

    // Owns SDL's video lifetime, window, and renderer. PImpl keeps SDL types private.
    class Engine final {
    public:
        Engine();
        ~Engine();

		//We do not want to allow copying or moving of the engine, as it manages resources that should not be duplicated or transferred.
        Engine(const Engine&) = delete;     
        Engine& operator=(const Engine&) = delete;
        Engine(Engine&&) = delete;
        Engine& operator=(Engine&&) = delete;

        bool    Initialise(const char* title = "HUNTEngine", int width = 640*2, int height = 720);
        int     Run(Application& application);
        void    Quit() noexcept;

        Renderer& GetRenderer();

    private:
		// The specifics of our engine implementation are hidden in Impl. 
        // This is useful for managing dependencies, which we don't want to expose in our header files.
        // https://en.cppreference.com/cpp/language/pimpl
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}
