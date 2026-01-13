#pragma once

#include <string>
#include <vector>
#include <memory>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "Vector.h"

// Custom deleters for SDL resources (RAII).
struct SDLDeleter
{
    void operator()(SDL_Window* w) const { if (w) SDL_DestroyWindow(w); }
    void operator()(SDL_Renderer* r) const { if (r) SDL_DestroyRenderer(r); }
    void operator()(SDL_Texture* t) const { if (t) SDL_DestroyTexture(t); }
};

class Application
{
public:
    Application(std::string_view inTitle, uint32_t inWidth, uint32_t inHeight);

    virtual ~Application();

    // Non-copyable.
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Initialize SDL, window, renderer, and texture.
    bool Init();
    // Main loop: events, update, render.
    void Run();
    uint32_t GetWidth() const { return width; }
    uint32_t GetHeight() const { return height; }

    // Framebuffer drawing helpers.
    void SetPixel(uint32_t x, uint32_t y, uint32_t color);
    void Clear(uint32_t color);

    // Bresenham line in screen space.
    void DrawLine(int inX0, int inY0, int inX1, int inY1, uint32_t inColor);

    // Barycentric coords for a 2D triangle.
    static Math::Vector3 ComputeBarycentric2D(float x, float y, const Math::Vector3* inV);
    // Fill a triangle in screen space.
    void DrawTriangle(const Math::Vector3& inV0, const Math::Vector3& inV1, const Math::Vector3& inV2, uint32_t inColor);

protected:
    // Override hooks.
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}

private:
    void ProcessEvents();
    void UpdateScreen() const;
    // Resize framebuffer and streaming texture.
    void Resize(uint32_t newWidth, uint32_t newHeight);

private:
    std::string title;
    uint32_t width;
    uint32_t height;
    bool bIsRunning{false};

    std::unique_ptr<SDL_Window, SDLDeleter> window;
    std::unique_ptr<SDL_Renderer, SDLDeleter> renderer;
    std::unique_ptr<SDL_Texture, SDLDeleter> screenTexture;

    // CPU framebuffer in RGBA.
    std::vector<uint32_t> framebuffer;

    // 深度缓冲区 (用于处理遮挡关系)
    std::vector<float> zBuffer;
};
