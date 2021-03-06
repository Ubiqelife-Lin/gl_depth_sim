#include "gl_depth_sim/mesh_loader.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

static std::unique_ptr<gl_depth_sim::Mesh> process(const aiScene* scene)
{
  const int num_meshes = scene->mNumMeshes;
  std::cout << "Scene has " << num_meshes << " meshes\n";

  if (num_meshes != 1)
  {
    std::cerr << "Scene must have 1 mesh";
    return {};
  }

  gl_depth_sim::EigenAlignedVec<Eigen::Vector3f> vertices;
  std::vector<unsigned> indices;

  const aiMesh* mesh = scene->mMeshes[0];

  const int nface = mesh->mNumFaces;
  const int nvert = mesh->mNumVertices;
  std::cout << "Mesh has n faces/verts: " << nface << "/" << nvert << "\n";

  for (int i = 0; i < nvert; ++i)
  {
    const aiVector3D& v =  mesh->mVertices[i];
    vertices.push_back({v.x, v.y, v.z});
  }

  for (int i = 0; i < nface; ++i)
  {
    const aiFace& f = mesh->mFaces[i];
    assert(f.mNumIndices == 3);
    indices.push_back(f.mIndices[0]);
    indices.push_back(f.mIndices[1]);
    indices.push_back(f.mIndices[2]);
  }

  return std::unique_ptr<gl_depth_sim::Mesh>(new gl_depth_sim::Mesh(vertices, indices));
}


std::unique_ptr<gl_depth_sim::Mesh> gl_depth_sim::loadMesh(const std::string& path)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

  if (!scene)
  {
    std::cerr << "Failed to load mesh: " << path << "\n";
    return {};
  }

  return process(scene);

}
