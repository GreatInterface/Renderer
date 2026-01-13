#include <SDL2/SDL.h>

#include "Application.h"
#include "Logger.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Renderer.h"

class PrimaryApp : public Application
{
public:
    PrimaryApp(std::string_view inTitle, uint32_t inWidth, uint32_t inHeight) : Application(inTitle, inWidth, inHeight)
    {
        if (!LoadMesh("assets/teapot.obj", mesh)) {
            spdlog::warn("Failed to load model, fallback to cube.");
            mesh = CreateCube();
        }
    }

    void OnUpdate(const float deltaTime) override
    {
        rotationY += deltaTime * 1.0f;
    }

    void OnRender() override
    {
        // Clear the framebuffer.
        Clear(0xFF000000);

        const float c = std::cos(rotationY);
        const float s = std::sin(rotationY);
        Math::Matrix44 model = Math::Matrix44::Identity();

        constexpr float scale = 0.1f;
        model.data[0] = c * scale; model.data[2] = -s * scale; model.data[5] = scale;
        model.data[8] = s * scale; model.data[10] = c * scale; model.data[15] = 1.0f;

        const Math::Vector3 eye{0.0f, 2.0f, -50.0f};
        const Math::Vector3 target{0.0f, 1.0f, 0.0f};
        const Math::Vector3 up{0.0f, 1.0f, 0.0f};
        const Math::Matrix44 view = Math::Matrix44::LookAtLH(eye, target, up);

        const auto screenW = static_cast<float>(GetWidth());
        const auto screenH = static_cast<float>(GetHeight());
        const float aspect = screenW / screenH;
        const Math::Matrix44 proj = Math::Matrix44::PerspectiveFovLH(3.1415926f / 4.0f, aspect, 0.1f, 100.0f);

        const Math::Matrix44 mvp = Math::Matrix44::Multiply(proj, Math::Matrix44::Multiply(view, model));

        for (size_t i = 0; i < mesh.indices.size(); i += 3) {
            const Vertex& v0 = mesh.vertices[mesh.indices[i]];
            const Vertex& v1 = mesh.vertices[mesh.indices[i + 1]];
            const Vertex& v2 = mesh.vertices[mesh.indices[i + 2]];

            VSOutput out0 = VertexShader(v0, model, mvp);
            VSOutput out1 = VertexShader(v1, model, mvp);
            VSOutput out2 = VertexShader(v2, model, mvp);

            Math::Vector3 s0 = ViewportTransform(out0.clipPos, GetWidth(), GetHeight());
            Math::Vector3 s1 = ViewportTransform(out1.clipPos, GetWidth(), GetHeight());
            Math::Vector3 s2 = ViewportTransform(out2.clipPos, GetWidth(), GetHeight());

            DrawTriangle(s0, s1, s2, out0.worldNormal, out1.worldNormal, out2.worldNormal, 0xFFCCCCCC);
        }
    }

private:
    Mesh mesh;
    float rotationY = 0.0f;
};


int main(int argc, char* argv[])
{
    Log::Init();
    spdlog::info("Starting Renderer.");

    PrimaryApp app("Soft Rasterizer v0.1", 800, 600);

    if (!app.Init())
    {
        spdlog::error("Application init failed.");
        return -1;
    }

    app.Run();

    return 0;
}