#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Logger.h"
#include "Mesh.h"


inline bool LoadMesh(const std::string &filepath, Mesh &outMesh)
{
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./assets/"; // �����ļ�·�� (��Ȼ���ڻ�û�õ�)
    reader_config.triangulate = true; // �ؼ����Զ�������β��Ϊ������

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filepath, reader_config))
    {
        if (!reader.Error().empty())
        {
            spdlog::error("TinyObjReader: {}", reader.Error());
        }
        return false;
    }

    if (!reader.Warning().empty())
    {
        spdlog::warn("TinyObjReader: {}", reader.Warning());
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();

    outMesh.indices.clear();
    outMesh.vertices.clear();

    size_t indexCount = 0;
    for (const auto &shape : shapes)
    {
        indexCount += shape.mesh.indices.size();
    }
    outMesh.indices.reserve(indexCount);
    outMesh.vertices.reserve(indexCount);

    const bool hasNormals = !attrib.normals.empty();

    for (const auto &shape : shapes)
    {
        for (size_t i = 0; i + 2 < shape.mesh.indices.size(); i += 3)
        {
            const auto &idx0 = shape.mesh.indices[i + 0];
            const auto &idx1 = shape.mesh.indices[i + 1];
            const auto &idx2 = shape.mesh.indices[i + 2];

            Math::Vector3 p0{
                attrib.vertices[3 * idx0.vertex_index + 0],
                attrib.vertices[3 * idx0.vertex_index + 1],
                attrib.vertices[3 * idx0.vertex_index + 2]
            };
            Math::Vector3 p1{
                attrib.vertices[3 * idx1.vertex_index + 0],
                attrib.vertices[3 * idx1.vertex_index + 1],
                attrib.vertices[3 * idx1.vertex_index + 2]
            };
            Math::Vector3 p2{
                attrib.vertices[3 * idx2.vertex_index + 0],
                attrib.vertices[3 * idx2.vertex_index + 1],
                attrib.vertices[3 * idx2.vertex_index + 2]
            };

            Math::Vector3 n0{};
            Math::Vector3 n1{};
            Math::Vector3 n2{};

            if (hasNormals && idx0.normal_index >= 0 && idx1.normal_index >= 0 && idx2.normal_index >= 0)
            {
                n0 = {
                    attrib.normals[3 * idx0.normal_index + 0],
                    attrib.normals[3 * idx0.normal_index + 1],
                    attrib.normals[3 * idx0.normal_index + 2]
                };
                n1 = {
                    attrib.normals[3 * idx1.normal_index + 0],
                    attrib.normals[3 * idx1.normal_index + 1],
                    attrib.normals[3 * idx1.normal_index + 2]
                };
                n2 = {
                    attrib.normals[3 * idx2.normal_index + 0],
                    attrib.normals[3 * idx2.normal_index + 1],
                    attrib.normals[3 * idx2.normal_index + 2]
                };
            }
            else
            {
                // OBJ has no normals; derive a flat normal per triangle.
                Math::Vector3 faceNormal = Math::Vector3::Cross(p1 - p0, p2 - p0);
                faceNormal.Normalize();
                n0 = faceNormal;
                n1 = faceNormal;
                n2 = faceNormal;
            }

            outMesh.vertices.emplace_back(p0, n0);
            outMesh.indices.push_back(static_cast<uint32_t>(outMesh.vertices.size() - 1));

            outMesh.vertices.emplace_back(p1, n1);
            outMesh.indices.push_back(static_cast<uint32_t>(outMesh.vertices.size() - 1));

            outMesh.vertices.emplace_back(p2, n2);
            outMesh.indices.push_back(static_cast<uint32_t>(outMesh.vertices.size() - 1));
        }
    }

    spdlog::info(SPDLOG_FMT_RUNTIME("Loaded {}: {} vertices, {} triangles."), filepath,
                 outMesh.vertices.size(), outMesh.indices.size() / 3);

    return true;
}
