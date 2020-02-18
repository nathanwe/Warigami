#include <vector>

#include <asset/rigged_mesh.hpp>



asset::rigged_mesh::rigged_mesh(aiScene* scene)
{
    auto assimp_mesh = scene->mMeshes[0];


}

std::vector<asset::rigged_vertex> asset::rigged_mesh::build_vertices(aiMesh *assimp_mesh) {
    // Reorganize data into array of struct instead of separate arrays
    std::vector<rigged_vertex> vertices;
    std::vector<uint32_t> indices;
    rigged_vertex swap_vertex;
    for (size_t i = 0; i < assimp_mesh->mNumVertices; i++)
    {
        swap_vertex.position.x = assimp_mesh->mVertices[i].x;
        swap_vertex.position.y = assimp_mesh->mVertices[i].y;
        swap_vertex.position.z = assimp_mesh->mVertices[i].z;

        if (assimp_mesh->HasNormals())
        {
            swap_vertex.normal.x = assimp_mesh->mNormals[i].x;
            swap_vertex.normal.y = assimp_mesh->mNormals[i].y;
            swap_vertex.normal.z = assimp_mesh->mNormals[i].z;
        }
        if (assimp_mesh->HasTextureCoords(0))
        {
            swap_vertex.tex_coord.x = assimp_mesh->mTextureCoords[0][i].x;
            swap_vertex.tex_coord.y = assimp_mesh->mTextureCoords[0][i].y;
        }
        if (assimp_mesh->HasTangentsAndBitangents())
        {
            swap_vertex.tangent.x = assimp_mesh->mTangents[i].x;
            swap_vertex.tangent.y = assimp_mesh->mTangents[i].y;
            swap_vertex.tangent.z = assimp_mesh->mTangents[i].z;
            swap_vertex.bitangent.x = assimp_mesh->mBitangents[i].x;
            swap_vertex.bitangent.y = assimp_mesh->mBitangents[i].y;
            swap_vertex.bitangent.z = assimp_mesh->mBitangents[i].z;
        }
        vertices.push_back(swap_vertex);
    }
    for (size_t i = 0; i < assimp_mesh->mNumFaces; i++)
    {
        indices.emplace_back(assimp_mesh->mFaces[i].mIndices[0]);
        indices.emplace_back(assimp_mesh->mFaces[i].mIndices[1]);
        indices.emplace_back(assimp_mesh->mFaces[i].mIndices[2]);
    }
    _num_indices = indices.size();
    return vertices;
}
