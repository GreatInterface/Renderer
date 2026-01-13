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
    screenPos.x = (ndcX + 1.0f) * 0.5f * (float)width;

    // Flip Y for screen coordinates.
    screenPos.y = (1.0f - ndcY) * 0.5f * (float)height;

    screenPos.z = ndcZ;
    return screenPos;
}

// Transform a position into clip space (column-major).
inline Math::Vector4 VertexShader(const Math::Vector3 &inPos, const Math::Matrix44 &mvp)
{
    // Column-major: clip = M * vec4
    Math::Vector4 clip;

    clip.x =
        inPos.x * mvp.data[0] +
        inPos.y * mvp.data[4] +
        inPos.z * mvp.data[8] +
        1.0f    * mvp.data[12];

    clip.y =
        inPos.x * mvp.data[1] +
        inPos.y * mvp.data[5] +
        inPos.z * mvp.data[9] +
        1.0f    * mvp.data[13];

    clip.z =
        inPos.x * mvp.data[2] +
        inPos.y * mvp.data[6] +
        inPos.z * mvp.data[10] +
        1.0f    * mvp.data[14];

    clip.w =
        inPos.x * mvp.data[3] +
        inPos.y * mvp.data[7] +
        inPos.z * mvp.data[11] +
        1.0f    * mvp.data[15];

    return clip;
}
