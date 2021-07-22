import os
import sys
from shapeworks import *


def copyTest1():
    img1 = Image(os.environ["DATA"] + "/femur.nrrd")
    img2 = img1.copy()
    img1.recenter()
    compareImg1 = Image(os.environ["DATA"] + "/femurRecentered.nrrd")
    compareImg2 = Image(os.environ["DATA"] + "/femur.nrrd")

    return img1 == compareImg1 and img2 == compareImg2


val = copyTest1()

if val is False:
    print("copyTest1 failed")
    sys.exit(1)


def copyTest2():
    mesh1 = Mesh(os.environ["DATA"] + "/femur.vtk")
    mesh2 = mesh1.copy()
    mesh1.translate([1, 10, -1])
    compareMesh1 = Mesh(os.environ["DATA"] + "/femurTranslated.vtk")
    compareMesh2 = Mesh(os.environ["DATA"] + "/femur.vtk")

    return mesh1 == compareMesh1 and mesh2 == compareMesh2


val = copyTest2()

if val is False:
    print("copyTest2 failed")
    sys.exit(1)
