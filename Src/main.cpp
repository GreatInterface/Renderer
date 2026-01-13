#include <SDL2/SDL.h>

#include "Application.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Renderer.h"

class PrimaryApp : public Application
{
public:
    PrimaryApp(std::string_view inTitle, uint32_t inWidth, uint32_t inHeight) : Application(inTitle, inWidth, inHeight)
    {
        if (!LoadMesh("assets/teapot.obj", mesh)) {
            std::cerr << "Failed to load model, fallback to cube." << std::endl;
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

        // 1. Model 矩阵
        // 茶壶通常比较大，我们把它缩小一点 (Scale 0.1) 并往下移一点
        const float c = std::cos(rotationY);
        const float s = std::sin(rotationY);
        Math::Matrix44 model = Math::Matrix44::Identity();

        // 简单的 Y 轴旋转 + 缩放 (Scale)
        // 组合矩阵：Scale * Rotation * Translation
        // 这里手动写死一个旋转 * 缩放
        constexpr float scale = 0.1f; // 如果茶壶太大，改成 0.1f
        model.data[0] = c * scale;  model.data[2] = -s * scale;
        model.data[5] = scale;      // y 轴缩放
        model.data[8] = s * scale;  model.data[10] = c * scale;
        model.data[15] = 1.0f;

        // 2. View 矩阵
        // 把摄像机拉远一点，茶壶可能比单位立方体大
        const Math::Vector3 eye{0.0f, 2.0f, -50.0f};
        const Math::Vector3 target{0.0f, 1.0f, 0.0f}; // 看向稍微高一点的地方(茶壶中心)
        const Math::Vector3 up{0.0f, 1.0f, 0.0f};
        const Math::Matrix44 view = Math::Matrix44::LookAtLH(eye, target, up);

        const auto screenW = static_cast<float>(GetWidth());
        const auto screenH = static_cast<float>(GetHeight());
        const float aspect = screenW / screenH;
        const Math::Matrix44 proj = Math::Matrix44::PerspectiveFovLH(3.1415926f / 4.0f, aspect, 0.1f, 100.0f);

        // 3. 计算 MVP = Proj * View * Model
        const Math::Matrix44 mvp = Math::Matrix44::Multiply(proj, Math::Matrix44::Multiply(view, model));

        // 4. 遍历索引绘制三角形
        for (size_t i = 0; i < mesh.indices.size(); i += 3)
        {
            // 获取三角形的三个顶点索引
            const uint32_t idx0 = mesh.indices[i];
            const uint32_t idx1 = mesh.indices[i+1];
            const uint32_t idx2 = mesh.indices[i+2];

            // 获取原始顶点
            const Math::Vector3& v0 = mesh.vertices[idx0];
            const Math::Vector3& v1 = mesh.vertices[idx1];
            const Math::Vector3& v2 = mesh.vertices[idx2];

            // Vertex Shader: 变换到裁剪空间
            auto process = [&](const Math::Vector3& v) {
                const Math::Vector4 clip = VertexShader(v, mvp);
                return ViewportTransform(clip, static_cast<int>(screenW), static_cast<int>(screenH));
            };

            Math::Vector3 s0 = process(v0);
            Math::Vector3 s1 = process(v1);
            Math::Vector3 s2 = process(v2);

            // 给每个面不同的颜色以便区分 (简单的 trick: 基于索引变换颜色)
            uint32_t color = 0xFF000000 | (100 + i * 10) << 16 | (50 + i * 5) << 8 | 255;

            // 绘制填充三角形 (前提是你已经按照上一步更新了带 Z-Buffer 的 DrawTriangle)
            DrawTriangle(s0, s1, s2, color);

            // 可选：绘制线框增强轮廓感
            // DrawLine(s0.x, s0.y, s1.x, s1.y, 0xFFFFFFFF);
            // DrawLine(s1.x, s1.y, s2.x, s2.y, 0xFFFFFFFF);
            // DrawLine(s2.x, s2.y, s0.x, s0.y, 0xFFFFFFFF);
        }
    }

private:
    Mesh mesh;
    float rotationY = 0.0f;
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
