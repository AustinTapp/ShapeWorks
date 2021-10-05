import os
import sys
from shapeworks import *

success = True

def fieldTest1():
  dist = Mesh(os.environ["DATA"] + "/meshdistance2.vtk")
  a = dist.getFieldValue("distance", 0)
  b = dist.getFieldValue("distance", 1000)
  c = dist.getFieldValue("distance", dist.numPoints()-1)

  return abs(a - 0.375761) < 1e-4 and abs(b - 2.18114) < 1e-4 and abs(c - 6.915) < 1e-4

success &= utils.test(fieldTest1)

def fieldTest2():
  mesh = Mesh(os.environ["DATA"] + "/mesh1.vtk")
  a = mesh.getFieldValue("scalars", 0)
  b = mesh.getFieldValue("scalars", 1000)
  c = mesh.getFieldValue("Normals", 4231)
  d = mesh.getFieldValue("Normals", 5634)

  return a == 1 and b == 1 and abs(c - 0.57735) < 1e-4 and d == 0

success &= utils.test(fieldTest2)

def fieldTest3():
  mesh = Mesh(os.environ["DATA"] + "/mesh1.vtk")
  scalarRange = mesh.getFieldRange("scalars")
  normalsRange = mesh.getFieldRange("Normals")

  return scalarRange[0] == 1 and scalarRange[1] == 1 and normalsRange[0] == -1 and normalsRange[1] == 1

success &= utils.test(fieldTest3)

sys.exit(not success)
