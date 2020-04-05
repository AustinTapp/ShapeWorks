#include <gtest/gtest.h>

#include <Libs/Image/Image.h>

#include "TestConfiguration.h"

using namespace shapeworks;

TEST(ImageTests, isoresample_binary_isotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "binary-isotropic-input.nrrd");
  image.antialias();
  image.isoresample();
  image.threshold();
  image.recenter();
  Image ground_truth(test_location + "binary-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresample_binary_anisotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "binary-anisotropic-input.nrrd");
  image.antialias();
  image.isoresample();
  image.threshold();
  image.recenter();
  Image ground_truth(test_location + "binary-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresample_smooth_isotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "smooth-isotropic-input.nrrd");
  image.isoresample();
  image.recenter();
  Image ground_truth(test_location + "smooth-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresample_smooth_anisotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "smooth-anisotropic-input.nrrd");
  image.isoresample();
  image.recenter();
  Image ground_truth(test_location + "smooth-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresample_image_anisotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "image-anisotropic-input.nrrd");
  image.isoresample();
  image.recenter();
  Image ground_truth(test_location + "image-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, pad_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/pad/");

  Image image(test_location + "1x2x2.nrrd");
  image.pad(30, 0.0);
 
  Image ground_truth(test_location + "pad_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, pad_identity_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/pad/");

  Image image(test_location + "1x2x2.nrrd");
  image.pad(0, 0.0);
  Image ground_truth(test_location + "1x2x2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, extractlabel_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/extract-label/");

  Image image(test_location + "1x2x2.nrrd");
  image.extractLabel(1.0);
  Image ground_truth(test_location + "extract-label_baseline.nrrd");
}

TEST(ImageTests, closeholes_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/close-holes/");

  Image image(test_location + "1x2x2.nrrd");
  image.closeHoles();
  Image ground_truth(test_location + "close-holes_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, threshold_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/threshold/");

  Image image(test_location + "1x2x2.nrrd");
  image.threshold();
  Image ground_truth(test_location + "threshold_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, computedt_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compute-dt/");

  Image image(test_location + "1x2x2.nrrd");
  image.computeDT();
  Image ground_truth(test_location + "compute-dt_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, dicom_read_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/dicom/");

  Image image(test_location + "dcm_files");
  Image ground_truth(test_location + "dicom.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, curvature_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/curvature/");

  Image image(test_location + "1x2x2.nrrd");
  image.applyCurvatureFilter();
  Image ground_truth(test_location + "curvature_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, gradient_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/gradient/");

  Image image(test_location + "1x2x2.nrrd");
  image.applyGradientFilter();
  Image ground_truth(test_location + "gradient_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, sigmoid_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/sigmoid/");

  Image image(test_location + "1x2x2.nrrd");
  image.applySigmoidFilter();
  Image ground_truth(test_location + "sigmoid_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, setlevel_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/set-level/");
  std::string other_location = std::string(TEST_DATA_DIR) + std::string("/curvature/curvature_baseline.nrrd");

  Image image(test_location + "1x2x2.nrrd");
  image.applyTPLevelSetFilter(other_location);
  Image ground_truth(test_location + "set-level_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, blur_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/blur/");

  Image image(test_location + "1x2x2.nrrd");
  image.gaussianBlur(2.0);
  Image ground_truth(test_location + "blur_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, crop_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/crop/");
  std::string images_location = std::string(TEST_DATA_DIR) + std::string("/images/");
  std::vector<std::string> images = {
    images_location + "seg.ellipsoid_1.nrrd",
    images_location + "seg.ellipsoid_2.nrrd",
    images_location + "seg.ellipsoid_3.nrrd",
    images_location + "seg.ellipsoid_4.nrrd",
    images_location + "seg.ellipsoid_5.nrrd",
    images_location + "seg.ellipsoid_6.nrrd",
    images_location + "seg.ellipsoid_7.nrrd",
    images_location + "seg.ellipsoid_8.nrrd",
    images_location + "seg.ellipsoid_9.nrrd",
  };

  Image image(test_location + "seg.ellipsoid_1.nrrd");
  Image::Region region;
  region = image.binaryBoundingBox(images);
  // image.read(test_location + "seg.ellipsoid_1.nrrd");
  image.crop(region);
  Image ground_truth(test_location + "crop_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

//TEST(ImageTests, blah_test) {

// ...

// }
