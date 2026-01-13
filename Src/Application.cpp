
#include <stdlib.h>
#include <cmath>
#include <algorithm>

#include "../Include/Application.h"


Application::Application(std::string_view inTitle, uint32_t inWidth, uint32_t inHeight)
    : title(inTitle), width(inWidth), height(inHeight)
{
    // Default to black.
    framebuffer.resize(inWidth * inHeight, 0xFF000000);

    // 默认深度 1.0 (最远)
    zBuffer.resize(inWidth * inHeight, 1.0f);
}

Application::~Application()
{
    SDL_Quit();
}

bool Application::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    window.reset(SDL_CreateWindow(title.c_str(),
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));

    if (!window)
    {
        return false;
    }

    renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED));
    if (!renderer) return false;

    // Streaming texture for CPU updates.
    screenTexture.reset(SDL_CreateTexture(

        renderer.get(),
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
        width, height
    ));

    return screenTexture != nullptr;
}

void Application::Run()
{
    bIsRunning = true;
    uint64_t lastTime = SDL_GetPerformanceCounter();

    while (bIsRunning)
    {
        ProcessEvents();

        // Compute delta time.
        const uint64_t currentTime = SDL_GetPerformanceCounter();
        const float deltaTime = static_cast<float>(currentTime - lastTime) / SDL_GetPerformanceFrequency();
        lastTime = currentTime;

        OnUpdate(deltaTime);
        OnRender();

        UpdateScreen();
    }
}

void Application::ProcessEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) bIsRunning = false;
        if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                Resize(static_cast<uint32_t>(event.window.data1),
                       static_cast<uint32_t>(event.window.data2));
            }
        }
        // ImGui event handling can go here.
    }
}

void Application::UpdateScreen() const
{
    // Upload framebuffer to the GPU texture.
    SDL_UpdateTexture(screenTexture.get(), nullptr, framebuffer.data(), width * sizeof(uint32_t));

    SDL_RenderClear(renderer.get());
    SDL_RenderCopy(renderer.get(), screenTexture.get(), nullptr, nullptr);

    // ImGui rendering can go here after the texture upload.

    SDL_RenderPresent(renderer.get());
}

void Application::Resize(uint32_t newWidth, uint32_t newHeight)
{
    if (newWidth == 0 || newHeight == 0) return;
    if (newWidth == width && newHeight == height) return;

    width = newWidth;
    height = newHeight;

    framebuffer.assign(width * height, 0xFF000000);
    zBuffer.assign(width * height, 1.0f);

    screenTexture.reset(SDL_CreateTexture(
        renderer.get(),
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
        width, height
    ));
}

void Application::SetPixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (x < width && y < height)
    {
        framebuffer[y * width + x] = color;
    }
}

void Application::Clear(const uint32_t color)
{
    ranges::fill(framebuffer, color);
    ranges::fill(zBuffer, 1.0f);
}

void Application::DrawLine(int inX0, int inY0, const int inX1, const int inY1, const uint32_t inColor)
{
    const int dx = std::abs(inX1 - inX0);
    const int dy = -std::abs(inY1 - inY0);
    const int sx = inX0 < inX1 ? 1 : -1;
    const int sy = inY0 < inY1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        SetPixel(inX0, inY0, inColor);
        if (inX0 == inX1 && inY0 == inY1) break;

        const int e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            inX0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            inY0 += sy;
        }
    }
}

Math::Vector3 Application::ComputeBarycentric2D(float x, float y, const Math::Vector3 *inV)
{
    float c1 = (x * (inV[1].y - inV[2].y) + (inV[2].x - inV[1].x) * y + inV[1].x * inV[2].y - inV[2].x * inV[1].y) /
               (inV[0].x * (inV[1].y - inV[2].y) + (inV[2].x - inV[1].x) * inV[0].y + inV[1].x * inV[2].y - inV[2].x *
                inV[1].y);
    float c2 = (x * (inV[2].y - inV[0].y) + (inV[0].x - inV[2].x) * y + inV[2].x * inV[0].y - inV[0].x * inV[2].y) /
               (inV[1].x * (inV[2].y - inV[0].y) + (inV[0].x - inV[2].x) * inV[1].y + inV[2].x * inV[0].y - inV[0].x *
                inV[2].y);
    return {c1, c2, 1.0f - c1 - c2};
}

void Application::DrawTriangle(const Math::Vector3 &inV0, const Math::Vector3 &inV1, const Math::Vector3 &inV2,
                               uint32_t inColor)
{
    // Bounding box in screen space.
    // 计算包围盒 (Bounding Box) 并限制在屏幕范围内 (Clamping)
    // 这一步能显著提升性能，防止在屏幕外无效循环
    int minX = static_cast<int>(std::floor(std::min({inV0.x, inV1.x, inV2.x})));
    int maxX = static_cast<int>(std::ceil(std::max({inV0.x, inV1.x, inV2.x})));
    int minY = static_cast<int>(std::floor(std::min({inV0.y, inV1.y, inV2.y})));
    int maxY = static_cast<int>(std::ceil(std::max({inV0.y, inV1.y, inV2.y})));

    minX = std::max(0, minX);
    maxX = std::min(static_cast<int>(width) - 1, maxX);
    minY = std::max(0, minY);
    maxY = std::min(static_cast<int>(height) - 1, maxY);

    const Math::Vector3 pts[3] = {inV0, inV1, inV2};

    // 2. 遍历像素
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            // 3. 计算重心坐标
            const Math::Vector3 bc = ComputeBarycentric2D(x + 0.5f, y + 0.5f, pts);

            // 4. 如果在三角形内部
            if (bc.x >= 0 && bc.y >= 0 && bc.z >= 0)
            {
                // 深度插值 (Z-Interpolation)
                // 注意：在透视投影中，屏幕空间的线性插值在数学上是不完全准确的(应插值 1/w)，
                // 但对于基础软光栅的第一步，直接插值 Z 是标准做法。
                const float depth = bc.x * inV0.z + bc.y * inV1.z + bc.z * inV2.z;

                // 深度测试 (Z-Test)
                // 左手系 DX 风格：Z 越小越近。如果当前像素深度 < 缓冲中的深度，则绘制。
                if (const int idx = y * width + x; depth < zBuffer[idx])
                {
                    zBuffer[idx] = depth;       // 更新深度
                    framebuffer[idx] = inColor; // 写入颜色 (直接操作数组比 SetPixel 更快)
                }
            }
        }
    }
}
