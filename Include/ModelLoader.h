#pragma once

#define TINYOBJLOADER_IMPLEMENTATION // 必须在一个 .cpp 文件中定义这个宏来实现库
#include "tiny_obj_loader.h"
#include <iostream>

#include "Mesh.h"


inline bool LoadMesh(const std::string &filepath, Mesh &outMesh)
{
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./assets/"; // 材质文件路径 (虽然现在还没用到)
    reader_config.triangulate = true; // 关键：自动将多边形拆分为三角形

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filepath, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error() << std::endl;
        }
        return false;
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning() << std::endl;
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();

    // 1. 提取顶点数据
    // TinyObj 的 vertices 是一个展平的 float 数组 [x, y, z, x, y, z, ...]
    // 我们需要将其转换为 Math::Vector3
    outMesh.vertices.clear();
    outMesh.vertices.reserve(attrib.vertices.size() / 3);

    for (size_t i = 0; i < attrib.vertices.size(); i += 3)
    {
        outMesh.vertices.emplace_back(
            attrib.vertices[i + 0], // x
            attrib.vertices[i + 1], // y
            attrib.vertices[i + 2] // z
        );
    }

    // 2. 提取索引数据
    // 目前我们只关心位置索引 (vertex_index)
    outMesh.indices.clear();

    for (const auto &shape: shapes)
    {
        for (const auto &index: shape.mesh.indices)
        {
            // tinyobj 的 index.vertex_index 就是对应 attrib.vertices 的下标
            outMesh.indices.push_back(index.vertex_index);
        }
    }

    std::cout << "Loaded " << filepath << ": "
            << outMesh.vertices.size() << " vertices, "
            << outMesh.indices.size() / 3 << " triangles." << std::endl;

    return true;
}
