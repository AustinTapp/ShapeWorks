#include <Libs/Mesh/MeshWarper.h>

#include <vtkCellLocator.h>
#include <vtkTriangleFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkLine.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkKdTreePointLocator.h>

#include <igl/biharmonic_coordinates.h>
#include <igl/point_mesh_squared_distance.h>
#include <igl/remove_unreferenced.h>

// tbb
#include <tbb/mutex.h>

using namespace shapeworks;

// for concurrent access
static tbb::mutex mutex;

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshWarper::build_mesh(const Eigen::MatrixXd& particles)
{
  if (!this->warp_available_) {
    return nullptr;
  }

  if (!this->check_warp_ready()) {
    return nullptr;
  }

  auto points = this->remove_bad_particles(particles);

  Mesh output = MeshWarper::warp_mesh(points);

  vtkSmartPointer<vtkPolyData> poly_data = output.getVTKMesh();

  for (int i = 0; i < poly_data->GetNumberOfPoints(); i++) {
    double* p = poly_data->GetPoint(i);
    if (std::isnan(p[0]) || std::isnan(p[1]) || std::isnan(p[2])) {
      this->warp_available_ = false; // failed
      std::cerr << "Reconstruction Failed\n";
      return nullptr;
    }
  }
  return poly_data;
}

//---------------------------------------------------------------------------
void MeshWarper::set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh,
                                    const Eigen::MatrixXd& reference_particles)
{
  if (this->incoming_reference_mesh_ == reference_mesh) {
    if (this->reference_particles_.size() == reference_particles.size()) {
      if (this->reference_particles_ == reference_particles) {
        // we can skip, nothing has changed
        return;
      }
    }
  }

  this->incoming_reference_mesh_ = reference_mesh;
  this->reference_particles_ = reference_particles;

  // mark that the warp needs to be generated
  this->needs_warp_ = true;

  this->warp_available_ = true;
}

//---------------------------------------------------------------------------
bool MeshWarper::get_warp_available()
{
  return this->warp_available_;
}

//---------------------------------------------------------------------------
bool MeshWarper::check_warp_ready()
{
  tbb::mutex::scoped_lock lock(mutex);

  if (!this->needs_warp_) {
    // warp already done
    return true;
  }

  return this->generate_warp();
}

//---------------------------------------------------------------------------
void MeshWarper::add_particle_vertices()
{

  for (int i = 0; i < this->vertices_.rows(); i++) {
    this->reference_mesh_->BuildLinks();

    auto locator = vtkSmartPointer<vtkCellLocator>::New();
    locator->SetCacheCellBounds(true);
    locator->SetDataSet(this->reference_mesh_);
    locator->BuildLocator();

    double pt[3]{this->vertices_(i, 0), this->vertices_(i, 1), this->vertices_(i, 2)};
    double closest_point[3];//the coordinates of the closest point will be returned here
    double closest_point_dist2; //the squared distance to the closest point will be returned here
    vtkIdType cell_id; //the cell id of the cell containing the closest point will be returned here
    int sub_id; //this is rarely used (in triangle strips only, I believe)
    locator->FindClosestPoint(pt, closest_point, cell_id, sub_id, closest_point_dist2);

    pt[0] = closest_point[0];
    pt[1] = closest_point[1];
    pt[2] = closest_point[2];

    // grab the closest cell
    vtkCell* cell = this->reference_mesh_->GetCell(cell_id);

    double point[3] = {pt[0], pt[1], pt[2]};
    double closest[3];
    double pcoords[3];
    double dist2;
    double weights[3];
    this->reference_mesh_->GetCell(cell_id)->EvaluatePosition(point, closest, sub_id, pcoords,
                                                              dist2, weights);
    bool same_as_vertex = false;

    if (weights[0] > 0.99 || weights[1] > 0.99 || weights[2] > 0.99) {
      same_as_vertex = true;
    }

    if (!same_as_vertex) {
      // now we need to check if we are along an edge already.
      bool on_edge = false;
      int v0_index = 0, v1_index = 0;
      double p0[3], p1[3], p2[3];
      cell->GetPoints()->GetPoint(0, p0);
      cell->GetPoints()->GetPoint(1, p1);
      cell->GetPoints()->GetPoint(2, p2);
      if (weights[2] < 0.01) {
        on_edge = true;
        v0_index = cell->GetPointId(0);
        v1_index = cell->GetPointId(1);
      }
      else if (weights[0] < 0.01) {
        on_edge = true;
        v0_index = cell->GetPointId(1);
        v1_index = cell->GetPointId(2);
      }
      else if (weights[1] < 0.01) {
        on_edge = true;
        v0_index = cell->GetPointId(0);
        v1_index = cell->GetPointId(2);
      }

      if (on_edge) {

        auto neighbors = vtkSmartPointer<vtkIdList>::New();
        this->reference_mesh_->GetCellEdgeNeighbors(cell_id, v0_index, v1_index, neighbors);

        // add the new vertex
        int new_vertex = this->reference_mesh_->GetPoints()->InsertNextPoint(pt);

        if (neighbors->GetNumberOfIds() == 1) {  // could be 0 for the boundary of an open mesh
          // split the neighbor cell into two triangles as well
          this->split_cell_on_edge(neighbors->GetId(0), new_vertex, v0_index, v1_index);
        }

        // split the current cell into two triangles
        this->split_cell_on_edge(cell_id, new_vertex, v0_index, v1_index);

        this->reference_mesh_->RemoveDeletedCells();

      }
      else {
        vtkSmartPointer<vtkIdList> list = vtkSmartPointer<vtkIdList>::New();

        int new_vertex = this->reference_mesh_->GetPoints()->InsertNextPoint(pt);
        list->SetNumberOfIds(3);
        list->SetId(0, cell->GetPointId(1));
        list->SetId(1, new_vertex);
        list->SetId(2, cell->GetPointId(0));
        this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);

        list->SetId(0, cell->GetPointId(2));
        list->SetId(1, new_vertex);
        list->SetId(2, cell->GetPointId(1));
        this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);

        list->SetId(0, cell->GetPointId(0));
        list->SetId(1, new_vertex);
        list->SetId(2, cell->GetPointId(2));
        this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);

        this->reference_mesh_->DeleteCell(cell_id);
        this->reference_mesh_->RemoveDeletedCells();

      }
    }
  }
}

//---------------------------------------------------------------------------
void MeshWarper::find_good_particles()
{
  // Create the tree
  auto tree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  tree->SetDataSet(this->reference_mesh_);
  tree->BuildLocator();

  std::vector<int> ids;
  for (int i = 0; i < this->vertices_.rows(); i++) {
    double p[3]{this->vertices_(i, 0), this->vertices_(i, 1), this->vertices_(i, 2)};
    int id = tree->FindClosestPoint(p);
    ids.push_back(id);
  }

  std::set<int> set;  // initially store in set to avoid duplicates
  for (int i = 0; i < this->vertices_.rows(); i++) {
    for (int j = i + 1; j < this->vertices_.rows(); j++) {
      if (ids[i] == ids[j]) {
        set.insert(i);
        set.insert(j);
      }
    }
  }

  this->good_particles_.clear();
  for (int i = 0; i < this->vertices_.rows(); i++) {
    if (set.find(i) == set.end()) {
      this->good_particles_.push_back(i);
    }
  }
}

//---------------------------------------------------------------------------
Eigen::MatrixXd MeshWarper::remove_bad_particles(const Eigen::MatrixXd& particles)
{
  Eigen::MatrixXd new_particles(this->good_particles_.size(), 3);
  for (int i = 0; i < this->good_particles_.size(); i++) {
    int id = this->good_particles_[i];
    new_particles(i, 0) = particles(id, 0);
    new_particles(i, 1) = particles(id, 1);
    new_particles(i, 2) = particles(id, 2);
  }

  return new_particles;
}

//---------------------------------------------------------------------------
void MeshWarper::split_cell_on_edge(int cell_id, int new_vertex, int v0, int v1)
{
  vtkCell* cell = this->reference_mesh_->GetCell(cell_id);
  int p0 = cell->GetPointId(0);
  int p1 = cell->GetPointId(1);
  int p2 = cell->GetPointId(2);
  int edge = -1;
  if ((v0 == p0 && v1 == p1) || (v0 == p1 && v1 == p0)) {
    edge = 0;
  }
  else if ((v0 == p1 && v1 == p2) || (v0 == p2 && v1 == p1)) {
    edge = 1;
  }
  else if ((v0 == p0 && v1 == p2) || (v0 == p2 && v1 == p0)) {
    edge = 2;
  }

  vtkSmartPointer<vtkIdList> list = vtkSmartPointer<vtkIdList>::New();
  list->SetNumberOfIds(3);
  if (edge == 0) {
    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(1));
    list->SetId(2, cell->GetPointId(2));
    this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);

    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(2));
    list->SetId(2, cell->GetPointId(0));
    this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);
  }
  else if (edge == 1) {
    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(0));
    list->SetId(2, cell->GetPointId(1));
    this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);

    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(2));
    list->SetId(2, cell->GetPointId(0));
    this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);
  }
  else if (edge == 2) {
    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(0));
    list->SetId(2, cell->GetPointId(1));
    this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);

    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(1));
    list->SetId(2, cell->GetPointId(2));
    this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);
  }

  this->reference_mesh_->DeleteCell(cell_id);

}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshWarper::clean_mesh(vtkSmartPointer<vtkPolyData> mesh)
{
  vtkSmartPointer<vtkTriangleFilter> triangle_filter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangle_filter->SetInputData(mesh);
  triangle_filter->Update();

  vtkSmartPointer<vtkPolyDataConnectivityFilter>
    connectivity = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
  connectivity->SetInputConnection(triangle_filter->GetOutputPort());
  connectivity->SetExtractionModeToLargestRegion();
  connectivity->Update();

  vtkSmartPointer<vtkCleanPolyData> clean = vtkSmartPointer<vtkCleanPolyData>::New();
  clean->ConvertPolysToLinesOff();
  clean->ConvertLinesToPointsOff();
  clean->ConvertStripsToPolysOff();
  clean->PointMergingOn();
  clean->SetInputConnection(connectivity->GetOutputPort());
  clean->Update();

  mesh = clean->GetOutput();
  return mesh;
}

//---------------------------------------------------------------------------
bool MeshWarper::generate_warp()
{
  // clean mesh
  this->reference_mesh_ = MeshWarper::clean_mesh(this->incoming_reference_mesh_);

  // prep points
  this->vertices_ = this->reference_particles_;

  this->add_particle_vertices();

  this->find_good_particles();
  this->vertices_ = this->remove_bad_particles(this->vertices_);

  Eigen::MatrixXd vertices = MeshWarper::distill_vertex_info(this->reference_mesh_);
  this->faces_ = MeshWarper::distill_face_info(this->reference_mesh_);

  // perform warp
  if (!MeshWarper::generate_warp_matrix(vertices, this->faces_,
                                        this->vertices_, this->warp_)) {
    this->warp_available_ = false;
    return false;
  }
  this->needs_warp_ = false;
  return true;
}

//---------------------------------------------------------------------------
Eigen::MatrixXd MeshWarper::distill_vertex_info(vtkSmartPointer<vtkPolyData> poly_data)
{
  vtkSmartPointer<vtkPoints> points = poly_data->GetPoints();
  vtkSmartPointer<vtkDataArray> data_array = points->GetData();

  int num_vertices = points->GetNumberOfPoints();

  Eigen::MatrixXd vertices(num_vertices, 3);

  for (int i = 0; i < num_vertices; i++) {
    vertices(i, 0) = data_array->GetComponent(i, 0);
    vertices(i, 1) = data_array->GetComponent(i, 1);
    vertices(i, 2) = data_array->GetComponent(i, 2);
  }
  return vertices;
}

//---------------------------------------------------------------------------
Eigen::MatrixXi MeshWarper::distill_face_info(vtkSmartPointer<vtkPolyData> poly_data)
{
  int num_faces = poly_data->GetNumberOfCells();
  Eigen::MatrixXi faces(num_faces, 3);

  vtkSmartPointer<vtkIdList> cells = vtkSmartPointer<vtkIdList>::New();

  for (int j = 0; j < num_faces; j++) {
    poly_data->GetCellPoints(j, cells);
    faces(j, 0) = cells->GetId(0);
    faces(j, 1) = cells->GetId(1);
    faces(j, 2) = cells->GetId(2);
  }
  return faces;
}

//---------------------------------------------------------------------------
bool MeshWarper::generate_warp_matrix(Eigen::MatrixXd TV, Eigen::MatrixXi TF,
                                      const Eigen::MatrixXd& Vref, Eigen::MatrixXd& W)
{
  Eigen::VectorXi b;
  {
    Eigen::VectorXi J = Eigen::VectorXi::LinSpaced(TV.rows(), 0, TV.rows() - 1);
    Eigen::VectorXd sqrD;
    Eigen::MatrixXd _2;
    // using J which is N by 1 instead of a matrix that represents faces of N by 3
    // so that we will find the closest vertices instead of closest point on the face
    // so far the two meshes are not separated. So what we are really doing here
    // is computing handles from low resolution and use that for the high resolution one
    igl::point_mesh_squared_distance(Vref, TV, J, sqrD, b, _2);
    //assert(sqrD.maxCoeff() < 1e-7 && "Particles must exist on vertices");
  }

  // list of points --> list of singleton lists
  std::vector<std::vector<int> > S;
  igl::matrix_to_list(b, S);

  // Technically k should equal 3 for smooth interpolation in 3d, but 2 is
  // faster and looks OK
  const int k = 2;
  if (!igl::biharmonic_coordinates(TV, TF, S, k, W)) {
    std::cerr << "igl:biharmonic_coordinates failed\n";
    return false;
  }
  // Throw away interior tet-vertices, keep weights and indices of boundary
  Eigen::VectorXi I, J;
  igl::remove_unreferenced(TV.rows(), TF, I, J);
  std::for_each(TF.data(), TF.data() + TF.size(), [&I](int& a) { a = I(a); });
  std::for_each(b.data(), b.data() + b.size(), [&I](int& a) { a = I(a); });
  igl::slice(Eigen::MatrixXd(TV), J, 1, TV);
  igl::slice(Eigen::MatrixXd(W), J, 1, W);
  return true;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshWarper::warp_mesh(const Eigen::MatrixXd& points)
{
  int num_vertices = this->warp_.rows();
  int num_faces = this->faces_.rows();
  Eigen::MatrixXd v_out = this->warp_ * (points.rowwise() + Eigen::RowVector3d(0, 0, 0));
  vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPoints> out_points = vtkSmartPointer<vtkPoints>::New();
  out_points->SetNumberOfPoints(num_vertices);
  for (vtkIdType i = 0; i < num_vertices; i++) {
    out_points->SetPoint(i, v_out(i, 0), v_out(i, 1), v_out(i, 2));
  }
  vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
  for (vtkIdType i = 0; i < num_faces; i++) {
    polys->InsertNextCell(3);
    polys->InsertCellPoint(this->faces_(i, 0));
    polys->InsertCellPoint(this->faces_(i, 1));
    polys->InsertCellPoint(this->faces_(i, 2));
  }
  poly_data->SetPoints(out_points);
  poly_data->SetPolys(polys);

  return poly_data;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------