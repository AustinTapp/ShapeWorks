import os
import sys
from shapeworks import *

def cropTest1():
  img = Image(os.environ["DATA"] + "/seg.ellipsoid_1.nrrd")

  region = img.logicalBoundingBox().pad(-16)

  region.min[0] = 7
  region.max[0] = 42

  img.crop(img.logicalToPhysical(region))

  compareImg = Image(os.environ["DATA"] + "/crop_baseline.nrrd")

  return img.compare(compareImg)

try:
  if not cropTest1():
    print("cropTest1 failed")
    sys.exit(1)
except RuntimeError:
  print("cropTest1 failed (exception)")
  sys.exit(1)

def cropTest2():
  img1 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_1.nrrd")
  img2 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_3.nrrd")
  img3 = Image(os.environ["DATA"] + "/many/seg.ellipsoid_4.nrrd")

  region = ImageUtils.boundingBox([img1, img2, img3])
  img1.crop(region)

  compareImg = Image(os.environ["DATA"] + "/cropped2.nrrd")

  return img1.compare(compareImg)

try:
  if not cropTest2():
    print("cropTest2 failed")
    sys.exit(1)
except RuntimeError:
  print("cropTest2 failed (exception)")
  sys.exit(1)
