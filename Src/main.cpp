#include <SDL2/SDL.h>

#include "Application.h"
#include "Renderer.h"

class PrimaryApp : public Application
{
public:
    PrimaryApp(std::string_view inTitle, uint32_t inWidth, uint32_t inHeight) : Application(inTitle, inWidth, inHeight) {}

    void OnRender() override
    {
        // Clear the framebuffer.
        Clear(0xFF000000);

        // Triangle in model space.
        const Math::Vector3 v0{ 0.0f,  0.5f, 0.0f};
        const Math::Vector3 v1{ 0.5f, -0.5f, 0.0f};
        const Math::Vector3 v2{-0.5f, -0.5f, 0.0f};

        const Math::Vector3 eye{0.0f, 0.0f, -2.0f};
        const Math::Vector3 target{0.0f, 0.0f, 1.0f};
        const Math::Vector3 up{0.0f, 1.0f, 0.0f};
        const Math::Matrix44 view = Math::Matrix44::LookAtLH(eye, target, up);

        float screenW = static_cast<float>(GetWidth());
        float screenH = static_cast<float>(GetHeight());
        const float aspect = screenW / screenH;
        const Math::Matrix44 proj = Math::Matrix44::PerspectiveFovLH(3.1415926f / 4.0f, aspect, 0.1f, 100.0f);

        Math::Matrix44 mvp = Math::Matrix44::Multiply(proj, view);

        // Transform and draw.
        auto processVertex = [&](const Math::Vector3& v) {
            Math::Vector4 clip = VertexShader(v, mvp);
            return ViewportTransform(clip, (int)screenW, (int)screenH);
        };
        Math::Vector3 s0 = processVertex(v0);
        Math::Vector3 s1 = processVertex(v1);
        Math::Vector3 s2 = processVertex(v2);

        // Draw red wireframe
        DrawLine((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, 0xFF0000FF);
        DrawLine((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, 0xFF0000FF);
        DrawLine((int)s2.x, (int)s2.y, (int)s0.x, (int)s0.y, 0xFF0000FF);
    }
};


int main(int argc, char* argv[])
{
    PrimaryApp app("Soft Rasterizer v0.1", 800, 600);

    if (!app.Init())
    {
        return -1;
    }

    app.Run();

    return 0;
}
