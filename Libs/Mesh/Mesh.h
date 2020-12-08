#pragma once

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <string>

namespace shapeworks {

class Mesh
{
public:
  using MeshType = vtkSmartPointer<vtkPolyData>; // TODO: we need to support multiple mesh types, such as vtkPolyData and vtkPLYData; probably use vtk mesh base class (if one exists)

  Mesh(const std::string& pathname) : mesh(read(pathname)) {}
  Mesh(vtkSmartPointer<vtkPolyData>&& rhs) : mesh(std::move(rhs)) {}

  bool write(const std::string &pathname);

  Mesh& coverage(const Mesh& other_mesh); // TODO: everything should be like this and return reference to self

  bool compare_points_equal(const Mesh& other_mesh);
  bool compare_scalars_equal(const Mesh& other_mesh);

  // query functions //

  vtkIdType numVertices() const { return mesh->GetNumberOfVerts(); }

private:
  Mesh() {}
  MeshType read(const std::string& pathname);

  MeshType mesh;
};

/// stream insertion operators for Mesh
std::ostream& operator<<(std::ostream &os, const Mesh& mesh);

} // shapeworks
