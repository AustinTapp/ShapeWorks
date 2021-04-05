#include <vector>
#include <string>

#include "Testing.h"

#include "ParticleSystem.h"
#include "ShapeEvaluation.h"
#include "ParticleShapeStatistics.h"

using namespace shapeworks;

const std::string test_dir = std::string(TEST_DATA_DIR) + "/ellipsoid_particles/";
const std::vector<std::string> filenames = {
  test_dir + "seg.ellipsoid_0.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_1.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_10.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_11.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_12.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_13.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_14.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_15.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_16.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_17.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_18.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_19.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_2.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_20.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
  test_dir + "seg.ellipsoid_21.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles"
};

TEST(ParticlesTests, vnlVector)
{
  ParticleSystem particleSystem(filenames);
  ParticleShapeStatistics stats;
  stats.DoPCA(particleSystem);
  stats.PrincipalComponentProjections();
  Eigen::MatrixXd pcaVec = stats.PCALoadings();
  for(int i=0; i<pcaVec.rows(); i++)
  {
    std::cout << "\n";
    for(int j=0; j<pcaVec.cols(); j++)
    {
      std::cout << pcaVec(i,j) << " ";
    }
  }
}

TEST(ParticlesTests, compactness)
{
  ParticleSystem particleSystem(filenames);
  const double compactness = ShapeEvaluation::ComputeCompactness(particleSystem, 1);
  ASSERT_DOUBLE_EQ(compactness, 0.99178682878009183);
}

TEST(ParticlesTests, generalization)
{
  ParticleSystem particleSystem(filenames);
  const double generalization = ShapeEvaluation::ComputeGeneralization(particleSystem, 1);
  ASSERT_DOUBLE_EQ(generalization, 0.19815116412998687);
}

TEST(ParticlesTests, specificity)
{
  ParticleSystem particleSystem(filenames);
  const double specificity = ShapeEvaluation::ComputeSpecificity(particleSystem, 1);
  ASSERT_NEAR(specificity, 0.262809, 1e-1f);
}
