
#include <HUNT/Engine.h>
#include <HUNT/Graphics/Renderer.h>

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

namespace HUNT {
    namespace {
        constexpr double fixedStepTime = 1.0 / 60.0;
    }

    // Define the implementation details of the Engine.
	// You can see that we are specifically using SDL here without exposing it in the header file.
    struct Engine::Impl {
        struct WindowDeleter {
            void operator()(SDL_Window* window) const noexcept { SDL_DestroyWindow(window); }
        };
       
        

        std::unique_ptr<SDL_Window, WindowDeleter> window;
        std::unique_ptr<Renderer> renderer;

        bool sdlInitialised = false;
        bool quit = false;
        bool vsyncEnabled = false;

        ~Impl() { Shutdown(); }

        void Shutdown() noexcept {
            renderer.reset();
            window.reset();

            if (sdlInitialised)
                SDL_Quit();

            sdlInitialised = false;
        }

        void PollEvents() {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                    quit = true;
                }
            }
        }
    };

    Engine::Engine() : impl_(std::make_unique<Impl>()) {}
    Engine::~Engine() = default;

    //This is mostly SDL boilerplate. Don't worry about it too much.
    bool Engine::Initialise(const char* title, int width, int height) {
        if (impl_->sdlInitialised || !title || width <= 0 || height <= 0) 
            return false;
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
            return false;
        }
        impl_->sdlInitialised = true;
        impl_->window.reset(SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE));
        if (!impl_->window) {
            std::fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
            impl_->Shutdown();
            return false;
        }
        //INIT THE RENDERER HERE
        impl_->renderer = std::make_unique<Renderer>(impl_->window.get());
        if (!impl_->renderer) {
            std::fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
            impl_->Shutdown();
            return false;
        }

        impl_->renderer->SetVSync(true);
        return true;
    }

	// This is the main loop of the engine. It handles events, updates, and rendering.
	// Note that it calls the Application's Start, FixedUpdate, Render, and Stop methods. 
    // This is where the engine and the application interact.
    int Engine::Run(Application& application) {
        if (!impl_->renderer) return 1;
        impl_->quit = false;
        using Clock = std::chrono::steady_clock;
        auto previousTime = Clock::now();
        double accumulator = 0.0;
        int result = 0;

        try {
            if (!application.Start(*this)) {
                std::fputs("Application start failed\n", stderr);
                result = 1;
            }
            while (result == 0 && !impl_->quit) {
                impl_->PollEvents();
                if (impl_->quit) break;

                const auto now = Clock::now();
                const double elapsed = std::chrono::duration<double>(now - previousTime).count();
                previousTime = now;

                accumulator += elapsed;
                while (accumulator >= fixedStepTime && !impl_->quit) {
                    application.FixedUpdate(*this, static_cast<float>(fixedStepTime));
                    accumulator -= fixedStepTime;
                }
                if (impl_->quit) break;

                application.Render(*this);

                impl_->renderer->Present();

				//If we're not using vsync, we need to manually delay to avoid a busy loop that maxes CPU usage.
                if (!impl_->vsyncEnabled) 
                    SDL_Delay(1);
            }
        }
        catch (const std::exception& error) {
            std::fprintf(stderr, "Engine run failed: %s\n", error.what());
            result = 1;
        }

        // Stop also runs after a failed Start so partial application setup can unwind.
        try {
            application.Stop(*this);
        }
        catch (const std::exception& error) {
            std::fprintf(stderr, "Application stop failed: %s\n", error.what());
            result = 1;
        }
        return result;
    }

    void Engine::Quit() noexcept { impl_->quit = true; }

    Renderer& Engine::GetRenderer()
    {
        return *impl_->renderer;
    }

   
}
