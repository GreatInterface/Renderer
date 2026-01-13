#include <SDL2/SDL.h>

#include "Application.h"
#include "Mesh.h"
#include "Renderer.h"

class PrimaryApp : public Application
{
public:
    PrimaryApp(std::string_view inTitle, uint32_t inWidth, uint32_t inHeight) : Application(inTitle, inWidth, inHeight)
    {
        cubeMesh = CreateCube();
    }

    void OnUpdate(const float deltaTime) override
    {
        rotationY += deltaTime * 1.57f;
    }

    void OnRender() override
    {
        // Clear the framebuffer.
        Clear(0xFF000000);

        // 1. 构建 Model 矩阵 (旋转)
        // 手写一个简单的 Y 轴旋转矩阵
        float c = std::cos(rotationY);
        float s = std::sin(rotationY);

        Math::Matrix44 model = Math::Matrix44::Identity();
        model.data[0] = c;  model.data[2] = -s;
        model.data[8] = s;  model.data[10] = c;

        // 2. View & Projection (保持不变)
        const Math::Vector3 eye{0.0f, 1.0f, -2.0f}; // 把摄像机稍微抬高一点，看清立体感
        const Math::Vector3 target{0.0f, 0.0f, 0.0f};
        const Math::Vector3 up{0.0f, 1.0f, 0.0f};
        const Math::Matrix44 view = Math::Matrix44::LookAtLH(eye, target, up);

        const auto screenW = static_cast<float>(GetWidth());
        const auto screenH = static_cast<float>(GetHeight());
        const float aspect = screenW / screenH;
        const Math::Matrix44 proj = Math::Matrix44::PerspectiveFovLH(3.1415926f / 4.0f, aspect, 0.1f, 100.0f);

        // 3. 计算 MVP = Proj * View * Model
        const Math::Matrix44 mvp = Math::Matrix44::Multiply(proj, Math::Matrix44::Multiply(view, model));

        // 4. 遍历索引绘制三角形
        for (size_t i = 0; i < cubeMesh.indices.size(); i += 3)
        {
            // 获取三角形的三个顶点索引
            const uint32_t idx0 = cubeMesh.indices[i];
            const uint32_t idx1 = cubeMesh.indices[i+1];
            const uint32_t idx2 = cubeMesh.indices[i+2];

            // 获取原始顶点
            const Math::Vector3& v0 = cubeMesh.vertices[idx0];
            const Math::Vector3& v1 = cubeMesh.vertices[idx1];
            const Math::Vector3& v2 = cubeMesh.vertices[idx2];

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
    Mesh cubeMesh;
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
