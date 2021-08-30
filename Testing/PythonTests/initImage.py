import os
import sys
import numpy as np
from shapeworks import *

# load an image
def initImageTest1():
  img = Image(os.environ["DATA"] + "/femurImage.nrrd")

  return img

utils.test(initImageTest1)

# pass invalid path to make sure an exception is thrown
def initImageTest2():
  img = Image(os.environ["DATA"] + "/not_a_valid_path.nrrd")

  return img

utils.expectException(initImageTest2, ValueError)

# load from a numpy array of the correct type
def initImageTest3():
  arr = np.ones([25,15,5], dtype=np.float32)
  img = Image(arr)

  return img

utils.test(initImageTest3)

# load from a numpy array of an unsupported type
def initImageTest4():
  arr = np.ones([25,15,5], dtype=np.float64)
  img = Image(arr)

  return img

utils.expectException(initImageTest4, ValueError)

# load from a numpy array and ensure the data is correct
def initImageTest5():
  arr0 = np.zeros([10,15,5], dtype=np.float32)
  arr1 = np.ones([25,15,5], dtype=np.float32)
  arr2 = np.zeros([10,15,5], dtype=np.float32)
  arr = np.concatenate((arr0, arr1, arr2))
  img = Image(arr)

  compareImg = Image(os.environ["DATA"] + "/imgFromArray.nrrd")
  return img == compareImg

utils.test(initImageTest5)

# copy constructor
def initImageTest6():
  img = Image(os.environ["DATA"] + "/femurImage.nrrd")
  img_copy = Image(img)
  return img == img_copy

utils.test(initImageTest6)

# assignment operator
def initImageTest7():
  img = Image(os.environ["DATA"] + "/femurImage.nrrd")
  img_copy = img
  return img == img_copy

utils.test(initImageTest7)
