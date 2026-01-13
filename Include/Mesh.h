#pragma once

#include <vector>
#include "Vector.h"

struct Vertex {
    Vertex() = default;

    Vertex(const Math::Vector3& inPos, const Math::Vector3& inNormal = {0, 0, 0})
            : position(inPos), normal(inNormal)
    {}

    Math::Vector3 position;
    Math::Vector3 normal{};   // 顶点法线
};


struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};


// 创建一个单位立方体，中心在原点，边长为 1
// 范围: [-0.5, 0.5]
inline Mesh CreateCube()
{
    Mesh mesh;

    // 1. 定义 8 个顶点
    mesh.vertices = {
        // Front face (Z = -0.5)
        Math::Vector3{-0.5f, 0.5f, -0.5f}, // 0: Top-Left
        Math::Vector3{0.5f, 0.5f, -0.5f}, // 1: Top-Right
        Math::Vector3{0.5f, -0.5f, -0.5f}, // 2: Bottom-Right
        Math::Vector3{-0.5f, -0.5f, -0.5f}, // 3: Bottom-Left

        // Back face (Z = 0.5)
        Math::Vector3{-0.5f, 0.5f, 0.5f}, // 4: Top-Left
        Math::Vector3{0.5f, 0.5f, 0.5f}, // 5: Top-Right
        Math::Vector3{0.5f, -0.5f, 0.5f}, // 6: Bottom-Right
        Math::Vector3{-0.5f, -0.5f, 0.5f} // 7: Bottom-Left
    };

    // 2. 定义 12 个三角形 (顺时针绕序)
    mesh.indices = {
        // Front Face
        0, 1, 2, 0, 2, 3,
        // Back Face
        5, 4, 7, 5, 7, 6,
        // Top Face
        4, 5, 1, 4, 1, 0,
        // Bottom Face
        3, 2, 6, 3, 6, 7,
        // Left Face
        4, 0, 3, 4, 3, 7,
        // Right Face
        1, 5, 6, 1, 6, 2
    };

    return mesh;
}
