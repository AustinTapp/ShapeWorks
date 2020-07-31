#pragma once

#include "itkParticleSystem.h"
#include "itkParticleGradientDescentPositionOptimizer.h"
#include "itkParticleEntropyGradientFunction.h"
#include "itkParticleImplicitSurfaceDomain.h"
#include "itkParticleContainerArrayAttribute.h"
#include "itkParticleCurvatureEntropyGradientFunction.h"
#include "itkParticleMeanCurvatureAttribute.h"
#include "itkParticleSurfaceNeighborhood.h"
#include "itkParticleOmegaGradientFunction.h"
#include "DomainType.h"
#include "MeshWrapper.h"
#include "MeshDomain.h"

#include "itkParticleModifiedCotangentEntropyGradientFunction.h"
#include "itkParticleConstrainedModifiedCotangentEntropyGradientFunction.h"
#include "vnl/vnl_matrix_fixed.h"

#include "TriMesh.h"

namespace shapeworks {

/** \class MaximumEntropySurfaceSampler
 *
 *
 *
 */
class MaximumEntropySurfaceSampler : public itk::DataObject {

public:

  using TImage = itk::Image<float, 3>;

  /** Standard class typedefs. */
  typedef MaximumEntropySurfaceSampler Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MaximumEntropySurfaceSampler, InPlaceImageFilter);

  /** Expose the image dimension. */
  itkStaticConstMacro(Dimension, unsigned int, TImage::ImageDimension);

  typedef itk::ParticleMeanCurvatureAttribute<typename TImage::PixelType, Dimension> MeanCurvatureCacheType;

  /** Convenient typedef for storing cutting plane information */
  struct CuttingPlaneType {
    vnl_vector_fixed<double, Dimension> a;
    vnl_vector_fixed<double, Dimension> b;
    vnl_vector_fixed<double, Dimension> c;
  };

  /** Convenient typedef for storing sphere information */
  struct SphereType {
    vnl_vector_fixed<double, Dimension> center;
    double radius;
  };

  /** Tranform Type*/
  typedef vnl_matrix_fixed<double, Dimension + 1, Dimension + 1> TransformType;

  /** Type of the input/output image. */
  typedef TImage ImageType;
  typedef itk::ParticleGradientDescentPositionOptimizer<typename ImageType::PixelType, Dimension> OptimizerType;

  /** Returns the particle system used in the surface sampling. */
  itkGetObjectMacro(ParticleSystem, itk::ParticleSystem<Dimension>);

  itkGetConstObjectMacro(ParticleSystem, itk::ParticleSystem<Dimension>);

  /** Returns a pointer to the gradient function used. */
  itk::ParticleEntropyGradientFunction<typename ImageType::PixelType, Dimension>
  * GetGradientFunction()
  {
    return m_GradientFunction;
  }

  itk::ParticleCurvatureEntropyGradientFunction<typename ImageType::PixelType, Dimension>
  * GetCurvatureGradientFunction()
  {
    return m_CurvatureGradientFunction;
  }

  itk::ParticleModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>
  * GetModifiedCotangentGradientFunction()
  {
    return m_ModifiedCotangentGradientFunction;
  }

  itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>
  * GetConstrainedModifiedCotangentGradientFunction()
  {
    return m_ConstrainedModifiedCotangentGradientFunction;
  }

  itk::ParticleOmegaGradientFunction<typename ImageType::PixelType, Dimension>
  * GetOmegaGradientFunction()
  {
    return m_OmegaGradientFunction;
  }

  /** Return a pointer to the optimizer object. */
  itkGetObjectMacro(Optimizer, OptimizerType);

  itkGetConstObjectMacro(Optimizer, OptimizerType);

  /**Optionally provide a filename for an initial point set.*/
  void SetPointsFile(unsigned int i, const std::string& s)
  {
    if (m_PointsFiles.size() < i + 1) {
      m_PointsFiles.resize(i + 1);
    }
    m_PointsFiles[i] = s;
  }

  void SetPointsFile(const std::string& s)
  {
    this->SetPointsFile(0, s);
  }

  /**Optionally provide a filename for a mesh with geodesic distances.*/
  void SetMeshFile(unsigned int i, const std::string& s)
  {
    if (m_MeshFiles.size() < i + 1) {
      m_MeshFiles.resize(i + 1);
    }
    m_MeshFiles[i] = s;
  }

  void SetMeshFile(const std::string& s)
  {
    this->SetMeshFile(0, s);
  }

  void SetMeshFiles(const std::vector<std::string>& s)
  {
    m_MeshFiles = s;
  }

  void AddImage(const typename TImage::Pointer image, double narrow_band);

  void AddMesh(shapeworks::MeshWrapper* mesh);

  void SetFidsFiles(const std::vector<std::string>& s)
  {
    m_FidsFiles = s;
  }

  void SetFeaFiles(const std::vector<std::string>& s)
  {
    m_FeaMeshFiles = s;
  }

  void SetFeaGradFiles(const std::vector<std::string>& s)
  {
    m_FeaGradFiles = s;
  }

  void SetDomainsPerShape(int i)
  {
    m_DomainsPerShape = i;
  }

  void SetAttributesPerDomain(const std::vector<int>& i)
  {
    m_AttributesPerDomain = i;
  }

  /** Optionally supply a cutting plane that will be set as a particle
  optimization constraint in the image domains. */
  void SetCuttingPlane(unsigned int i,
                       const vnl_vector_fixed<double, Dimension>& va,
                       const vnl_vector_fixed<double, Dimension>& vb,
                       const vnl_vector_fixed<double, Dimension>& vc);

  /** Transform a cutting plane based on procrustes transformation */
  void TransformCuttingPlanes(unsigned int i);

  /** Optionally add spheres that may be used as constraints to the domain. */
  void AddSphere(unsigned int i, vnl_vector_fixed<double, Dimension>& c, double r);

  /** This method sets the optimization function for the sampling.
    mode 0 = no adaptivity
    mode 1 = isotropic adaptivity
    mode 2 = anisotropic adaptivity
  */
  virtual void SetAdaptivityMode(int mode)
  {
    if (mode == 0) {
      if (m_pairwise_potential_type == 0)
        m_Optimizer->SetGradientFunction(m_CurvatureGradientFunction);
      else if (m_pairwise_potential_type == 1)
        m_Optimizer->SetGradientFunction(m_ModifiedCotangentGradientFunction);
    }
    else if (mode == 1) {
      m_Optimizer->SetGradientFunction(m_GradientFunction);
    }
    else if (mode == 3) {
      if (m_pairwise_potential_type == 0)
        m_Optimizer->SetGradientFunction(m_OmegaGradientFunction);
      else if (m_pairwise_potential_type == 1)
        m_Optimizer->SetGradientFunction(m_ConstrainedModifiedCotangentGradientFunction);
    }

    m_AdaptivityMode = mode;
    this->Modified();

  }

  int GetAdaptivityMode() const
  { return m_AdaptivityMode; }

  void SetTransformFile(const std::string& s)
  { m_TransformFile = s; }

  void SetTransformFile(const char* s)
  { m_TransformFile = std::string(s); }

  void SetPrefixTransformFile(const std::string& s)
  { m_PrefixTransformFile = s; }

  void SetPrefixTransformFile(const char* s)
  { m_PrefixTransformFile = std::string(s); }

  void SetPairwisePotentialType(int pairwise_potential_type)
  { m_pairwise_potential_type = pairwise_potential_type; }

  int GetPairwisePotentialType()
  { return m_pairwise_potential_type; }

  void SetVerbosity(unsigned int val)
  {
    m_verbosity = val;
    m_Optimizer->SetVerbosity(val);
  }

  unsigned int GetVerbosity()
  { return m_verbosity; }

  MeanCurvatureCacheType* GetMeanCurvatureCache()
  { return m_MeanCurvatureCache.GetPointer(); }

  void ReadTransforms();
  void ReadPointsFiles();
  virtual void AllocateDataCaches();
  virtual void AllocateDomainsAndNeighborhoods();
  virtual void InitializeOptimizationFunctions();

/** */
  virtual void Initialize()
  {
    this->m_Initializing = true;
    this->Execute();
    this->m_Initializing = false;
  }

  virtual void ReInitialize();

  virtual void Execute();

protected:
  MaximumEntropySurfaceSampler();

  virtual ~

  MaximumEntropySurfaceSampler()
  {};

  void GenerateData();

  itkSetMacro(Initialized, bool);

  itkGetMacro(Initialized, bool);

  itkSetMacro(Initializing, bool);

  itkGetMacro(Initializing, bool);

  bool m_Initialized;
  int m_AdaptivityMode;
  bool m_Initializing;

  typename OptimizerType::Pointer m_Optimizer;

  typename itk::ParticleEntropyGradientFunction<typename ImageType::PixelType, Dimension>
  ::Pointer m_GradientFunction;
  typename itk::ParticleCurvatureEntropyGradientFunction<typename ImageType::PixelType, Dimension>
  ::Pointer m_CurvatureGradientFunction;

  typename itk::ParticleModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>
  ::Pointer m_ModifiedCotangentGradientFunction;
  typename itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>
  ::Pointer m_ConstrainedModifiedCotangentGradientFunction;

  typename itk::ParticleOmegaGradientFunction<typename ImageType::PixelType, Dimension>
  ::Pointer m_OmegaGradientFunction;

  typename itk::ParticleContainerArrayAttribute<double, Dimension>::Pointer m_Sigma1Cache;
  typename itk::ParticleContainerArrayAttribute<double, Dimension>::Pointer m_Sigma2Cache;

  typename MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;

  typename itk::ParticleSystem<Dimension>::Pointer m_ParticleSystem;

  std::vector<typename itk::ParticleDomain::Pointer> m_DomainList;

  std::vector<typename itk::ParticleSurfaceNeighborhood<ImageType>::Pointer> m_NeighborhoodList;

  int m_pairwise_potential_type;

private:
  MaximumEntropySurfaceSampler(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<std::string> m_PointsFiles;
  std::vector<std::string> m_MeshFiles;
  std::vector<std::string> m_FeaMeshFiles;
  std::vector<std::string> m_FeaGradFiles;
  std::vector<std::string> m_FidsFiles;
  std::vector<int> m_AttributesPerDomain;
  int m_DomainsPerShape;
  double m_Spacing{0};

  std::string m_TransformFile;
  std::string m_PrefixTransformFile;
  std::vector<std::vector<CuttingPlaneType> > m_CuttingPlanes;
  std::vector<std::vector<SphereType> > m_Spheres;

  unsigned int m_verbosity;

};

} // end namespace





