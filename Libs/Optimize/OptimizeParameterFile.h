#pragma once

#include <string>

class Optimize;
class TiXmlHandle;

/**
 * \class OptimizeParameterFile
 * \ingroup Group-Optimize
 *
 * Implementation of Optimize library parameter file
 *
 * \par Overview of OptimizeParameterFile
 *
 * The OptimizeParameterFile class reads in a parameter file and sets the
 * appropriate values for an Optimize object.
 *
 */
class OptimizeParameterFile
{
public:

  OptimizeParameterFile();

  bool set_parameters(std::string filename, Optimize* optimize);

private:

  bool set_io_parameters(TiXmlHandle* docHandle, Optimize* optimize);

  bool set_optimization_parameters(TiXmlHandle* docHandle, Optimize* optimize);

  bool set_debug_parameters(TiXmlHandle* docHandle, Optimize* optimize);

  bool read_inputs(TiXmlHandle* docHandle, Optimize* optimize);

  bool read_mesh_inputs(TiXmlHandle* docHandle, Optimize* optimize);

  bool read_constraints(TiXmlHandle* doc_handle, Optimize* optimize);

  bool read_distribution_cutting_plane(TiXmlHandle* doc_handle, Optimize* optimize);

  bool read_cutting_planes(TiXmlHandle* doc_handle, Optimize* optimize);

  bool read_cutting_spheres(TiXmlHandle* doc_handle, Optimize* optimize);

private:
  int verbosity_level_ = 0;
};
