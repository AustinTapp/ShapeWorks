#include "Commands.h"
#include "Image.h"

namespace Shapeworks {



// boilerplate for a command. Copy this to start a new command
#if 0
///////////////////////////////////////////////////////////////////////////////
// Example
///////////////////////////////////////////////////////////////////////////////
void Example::buildParser()
{
  const std::string prog = "example";
  const std::string desc = "brief description of command";
  parser.prog(prog).description(desc);

  parser.add_option("--optionName").action("store").type("float").set_default(0.01).help("Description of optionName.");
  //additional options... 
  
  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Example::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float optionName = static_cast<float>(options.get("optionName"));
  //read additional options... 

  return sharedData.image.example(optionName, ...);
}
#endif




///////////////////////////////////////////////////////////////////////////////
// Antialias
///////////////////////////////////////////////////////////////////////////////
void Antialias::buildParser()
{
  const std::string prog = "antialias";
  const std::string desc = "antialiases binary volumes";
  parser.prog(prog).description(desc);

  parser.add_option("--maxRMSError").action("store").type("float").set_default(0.01).help("The maximum RMS error allowed.");
  parser.add_option("--numIterations").action("store").type("int").set_default(50).help("Number of iterations.");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Antialias::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  float maxRMSErr = static_cast<float>(options.get("maxRMSError"));
  int numIter = static_cast<int>(options.get("numIterations"));

  return sharedData.image.antialias(maxRMSErr, numIter);
}


///////////////////////////////////////////////////////////////////////////////
// Resamplevolume
///////////////////////////////////////////////////////////////////////////////
void Resamplevolume::buildParser()
{
  const std::string prog = "resamplevolume";
  const std::string desc = "resamples volumes to be isotropic";
  parser.prog(prog).description(desc);

  parser.add_option("--isBinary").action("store").type("bool").set_default(false).help("A flag to treat the input image as a binary image (specialized resampling pipeline) [default disabled].");
  parser.add_option("--recenter").action("store").type("bool").set_default(false).help("A flag to recenter the volume, i.e. change the origin in the image header to the physcial coordinates of the first voxel (lower left corner) [default disabled].");
  parser.add_option("--isoSpacing").action("store").type("float").set_default(1.0f).help("The isotropic spacing in all dimensions.");
  parser.add_option("--sizeX").action("store").type("unsigned").set_default(0).help("Image size in x-direction (optional, if set to 0, the size is autmatically estimated from the input image).");
  parser.add_option("--sizeY").action("store").type("unsigned").set_default(0).help("Image size in y-direction (optional, if set to 0, the size is autmatically estimated from the input image).");
  parser.add_option("--sizeZ").action("store").type("unsigned").set_default(0).help("Image size in z-direction (optional, if set to 0, the size is autmatically estimated from the input image).");

  Command::buildParser();
}

int Resamplevolume::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  bool isBinary = static_cast<bool>(options.get("isBinary"));
  bool recenter = static_cast<bool>(options.get("recenter"));
  float isoSpacing = static_cast<float>(options.get("isoSpacing"));
  unsigned sizeX = static_cast<unsigned>(options.get("sizeX"));
  unsigned sizeY = static_cast<unsigned>(options.get("sizeY"));
  unsigned sizeZ = static_cast<unsigned>(options.get("sizeZ"));

  return sharedData.image.resamplevolume(isBinary, recenter, isoSpacing, Dims({sizeX, sizeY, sizeZ}));
}


///////////////////////////////////////////////////////////////////////////////
// Clip
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Smoothmesh
///////////////////////////////////////////////////////////////////////////////
void Smoothmesh::buildParser()
{
  const std::string prog = "smoothmesh";
  const std::string desc = "smooths meshes";
  parser.prog(prog).description(desc);
  
  // TODO
  // parser.add_option("--maxRMSError").action("store").type("float").set_default(0.01).help("The maximum RMS error allowed.");
  // parser.add_option("--numIterations").action("store").type("int").set_default(50).help("Number of iterations.");

  Command::buildParser();
}

///////////////////////////////////////////////////////////////////////////////
int Smoothmesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  // float maxRMSErr = static_cast<float>(options.get("maxRMSError"));
  // int numIter = static_cast<int>(options.get("numIterations"));

  return sharedData.mesh.smooth(/*maxRMSErr, numIter*/);
}



} // Shapeworks
