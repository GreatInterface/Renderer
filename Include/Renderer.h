#pragma once

#include <cstdlib>

#include "Vector.h"

inline Math::Vector3 ViewportTransform(const Math::Vector4 &clipPos, int width, int height)
{
    Math::Vector3 screenPos;

    // Perspective divide to NDC.
    float invW = 1.0f / clipPos.w;
    float ndcX = clipPos.x * invW;
    float ndcY = clipPos.y * invW;
    float ndcZ = clipPos.z * invW;

    // Map NDC [-1, 1] to screen pixels.
    screenPos.x = (ndcX + 1.0f) * 0.5f * (float) width;

    // Flip Y for screen coordinates.
    screenPos.y = (1.0f - ndcY) * 0.5f * (float) height;

    screenPos.z = ndcZ;
    return screenPos;
}

struct VSOutput
{
    Math::Vector4 clipPos{};
    Math::Vector3 worldNormal{};
};

// Transform a position into clip space (column-major).
inline Math::Vector4 VertexShader(const Math::Vector3 &inPos, const Math::Matrix44 &mvp)
{
    // Column-major: clip = M * vec4
    Math::Vector4 clip;

    clip.x =
            inPos.x * mvp.data[0] +
            inPos.y * mvp.data[4] +
            inPos.z * mvp.data[8] +
            1.0f * mvp.data[12];

    clip.y =
            inPos.x * mvp.data[1] +
            inPos.y * mvp.data[5] +
            inPos.z * mvp.data[9] +
            1.0f * mvp.data[13];

    clip.z =
            inPos.x * mvp.data[2] +
            inPos.y * mvp.data[6] +
            inPos.z * mvp.data[10] +
            1.0f * mvp.data[14];

    clip.w =
            inPos.x * mvp.data[3] +
            inPos.y * mvp.data[7] +
            inPos.z * mvp.data[11] +
            1.0f * mvp.data[15];

    return clip;
}

inline VSOutput VertexShader(const Vertex &inV, const Math::Matrix44 &model, const Math::Matrix44 &mvp)
{
    VSOutput output;
    // 1. 位置变换到裁剪空间 (用于光栅化)
    output.clipPos = VertexShader(inV.position, mvp);

    // 2. 法线变换到世界空间 (用于光照计算)
    // 注意：只取 Model 矩阵左上角 3x3 部分进行方向变换
    output.worldNormal.x = inV.normal.x * model.data[0] + inV.normal.y * model.data[4] + inV.normal.z * model.data[8];
    output.worldNormal.y = inV.normal.x * model.data[1] + inV.normal.y * model.data[5] + inV.normal.z * model.data[9];
    output.worldNormal.z = inV.normal.x * model.data[2] + inV.normal.y * model.data[6] + inV.normal.z * model.data[10];
    output.worldNormal.Normalize();

    return output;
}
