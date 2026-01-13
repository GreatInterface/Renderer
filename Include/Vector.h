#pragma once

#include <cmath>

#include <array>


using namespace std;

namespace Math
{
    struct Vector3
    {
        float x{0.0f}, y{0.0f}, z{0.0f};

        Vector3()=default;
        Vector3(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ)
        {
        }

        // Dot product.
        static float Dot(const Vector3 &inA, const Vector3 &inB)
        {
            return inA.x * inB.x + inA.y * inB.y + inA.z * inB.z;
        }

        // Cross product.
        static Vector3 Cross(const Vector3 &inA, const Vector3 &inB)
        {
            return {
                inA.y * inB.z - inA.z * inB.y,
                inA.z * inB.x - inA.x * inB.z,
                inA.x * inB.y - inA.y * inB.x
            };
        }

        Vector3 operator-(const Vector3 &inV) const { return {x - inV.x, y - inV.y, z - inV.z}; }
        Vector3 operator+(const Vector3 &inV) const { return {x + inV.x, y + inV.y, z + inV.z}; }
        Vector3 operator*(float inF) const { return {x * inF, y * inF, z * inF}; }

        float Length() const { return std::sqrt(x * x + y * y + z * z); }

        void Normalize()
        {
            float len = Length();
            if (len > 0)
            {
                x /= len;
                y /= len;
                z /= len;
            }
        }
    };

    struct Vector4
    {
        float x, y, z, w;
    };

    struct Matrix44
    {
        // Column-major storage.
        std::array<float, 16> data{};

        Matrix44() { data.fill(0.0f); }

        static Matrix44 Identity()
        {
            Matrix44 mat;
            mat.data[0] = mat.data[5] = mat.data[10] = mat.data[15] = 1.0f;
            return mat;
        }

        // Left-handed look-at view matrix (column-major).
        static Matrix44 LookAtLH(const Vector3 &inEye, const Vector3 &inTarget, const Vector3 &inUp)
        {
            // Forward axis (camera looks toward +Z in LH).
            Vector3 zAxis = inTarget - inEye;
            zAxis.Normalize();

            // Right axis.
            Vector3 xAxis = Vector3::Cross(inUp, zAxis);
            xAxis.Normalize();

            // Up axis.
            Vector3 yAxis = Vector3::Cross(zAxis, xAxis);
            yAxis.Normalize();

            // View matrix with rotation + translation in the last column.
            Matrix44 result = Identity();
            result.data[0] = xAxis.x;
            result.data[1] = xAxis.y;
            result.data[2] = xAxis.z;
            result.data[4] = yAxis.x;
            result.data[5] = yAxis.y;
            result.data[6] = yAxis.z;
            result.data[8] = zAxis.x;
            result.data[9] = zAxis.y;
            result.data[10] = zAxis.z;

            result.data[12] = -Vector3::Dot(xAxis, inEye);
            result.data[13] = -Vector3::Dot(yAxis, inEye);
            result.data[14] = -Vector3::Dot(zAxis, inEye);
            return result;
        }

        // LH perspective projection (column-major).
        static Matrix44 PerspectiveFovLH(float fovRadians, float aspect, float zNear, float zFar)
        {
            Matrix44 result;

            float tanHalfFov = std::tan(fovRadians / 2.0f);
            float yScale = 1.0f / tanHalfFov;
            float xScale = yScale / aspect;

            // Z range: [0, 1] for DirectX/Vulkan.
            float fRange = zFar / (zFar - zNear);

            result.data[0] = xScale;

            result.data[5] = yScale;

            result.data[10] = fRange;
            result.data[11] = 1.0f; // w = z for perspective divide

            result.data[14] = -fRange * zNear;
            result.data[15] = 0.0f;

            return result;
        }

        // Column-major matrix multiply (a * b).
        static Matrix44 Multiply(const Matrix44 &a, const Matrix44 &b)
        {
            Matrix44 res;
            for (int r = 0; r < 4; ++r)
            {
                for (int c = 0; c < 4; ++c)
                {
                    res.data[c * 4 + r] =
                            a.data[0 * 4 + r] * b.data[c * 4 + 0] +
                            a.data[1 * 4 + r] * b.data[c * 4 + 1] +
                            a.data[2 * 4 + r] * b.data[c * 4 + 2] +
                            a.data[3 * 4 + r] * b.data[c * 4 + 3];
                }
            }
            return res;
        }
    };
}
