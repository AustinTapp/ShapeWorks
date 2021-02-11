import os
import sys
import shapeworks as sw

def recenterTest1():
  img = sw.Image(os.environ["DATA"] + "/1x2x2.nrrd")
  img.recenter()

  compareImg = sw.Image(os.environ["DATA"] + "/recenter.nrrd")

  return img.compare(compareImg)

val = recenterTest1()

if val is False:
  sys.exit(1)