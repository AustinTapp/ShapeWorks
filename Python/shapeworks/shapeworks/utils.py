import numpy as np
import os
import vtk

# helper function to determine the best grid size (rows and columns) given the number of samples in a dataset.
def postive_factors(num_samples):
    factors = []

    for whole_number in range(1, num_samples + 1):
        if num_samples % whole_number == 0:
            factors.append(whole_number)

    return factors

def num_subplots(num_samples):
    factors = postive_factors(num_samples)
    cols    = min(int(np.ceil(np.sqrt(num_samples))),max(factors))
    rows    = int(np.ceil(num_samples/cols))

    return rows, cols

# a helper function that saves a list of shapeworks images in a directory
# this could be used to save final and intermediate results (if needed)
def save_images(outDir,        # path to the directory where we want to save the images
                swImageList,   # list of shapeworks images to be saved
                swImageNames,  # list of image names to be used as filenames
                extension        = 'nrrd',
                compressed       = False, # use false to load in paraview
                verbose          = True):

    if (len(swImageList) != len(swImageNames)):
        print('swImageNames list is not consistent with number of images in swImageList')
        return

    # create the output directory in case it does not exist
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    filenames = []
    for curImg, curName in zip(swImageList, swImageNames):
        filename = outDir + curName + '.' + extension
        if verbose:
            print('Writing: ' + filename)
        if not os.path.exists(os.path.dirname(filename)):
            os.makedirs(os.path.dirname(filename))
        curImg.write(filename, compressed=compressed)
        filenames.append(filename)
    return filenames

# a helper function that saves a list of shapeworks meshes in a directory
# this could be used to save final and intermediate results (if needed)
def save_meshes(outDir,        # path to the directory where we want to save the images
                swMeshList,   # list of shapeworks images to be saved
                swMeshNames,  # list of image names to be used as filenames
                extension        = 'ply',
                compressed       = False, # use false to load in paraview
                verbose          = True):

    if (len(swMeshList) != len(swMeshNames)):
        print('swMeshNames list is not consistent with number of images in swMeshList')
        return

    # create the output directory in case it does not exist
    if not os.path.exists(outDir):
        os.makedirs(outDir)

    filenames = []
    for curMesh, curName in zip(swMeshList, swMeshNames):
        filename = outDir + curName + '.' + extension
        if verbose:
            print('Writing: ' + filename)
        if not os.path.exists(os.path.dirname(filename)):
            os.makedirs(os.path.dirname(filename))
        curMesh.write(filename)
        filenames.append(filename)
    return filenames

# a helper function to get list of files with specific extensions from a given file list
def get_file_with_ext(file_list,extension):

    extList =[]
    for file in file_list:
        ext = file.split(".")[-1]
        if(ext==extension):
            extList.append(file)
    extList = sorted(extList)
    return extList

def find_reference_image_index(inDataList):
    x = y = z = 0
    for i in range(len(inDataList)):
        img = inDataList[i]
        tmp = img.toArray()
        dim = tmp.shape
        if dim[0] > x:
            x = dim[0]
        if dim[1] > y:
            y = dim[1]
        if dim[2] > z:
            z = dim[2]

    COM = np.zeros((x, y, z))
    for i in range(len(inDataList)):
        img = inDataList[i]
        tmp = img.toArray()
        COM += np.pad(tmp, (((x - tmp.shape[0]) // 2, (x - tmp.shape[0]) - (x - tmp.shape[0]) // 2),
                            ((y - tmp.shape[1]) // 2, (y - tmp.shape[1]) - (y - tmp.shape[1]) // 2),
                            ((z - tmp.shape[2]) // 2, (z - tmp.shape[2]) - (z - tmp.shape[2]) // 2)))
    COM /= len(inDataList)
    dist = np.inf
    idx = 0
    for i in range(len(inDataList)):
        img = inDataList[i]
        tmp = img.toArray()
        tmp_dist = np.linalg.norm(
            COM - np.pad(tmp, (((x - tmp.shape[0]) // 2, (x - tmp.shape[0]) - (x - tmp.shape[0]) // 2),
                               ((y - tmp.shape[1]) // 2, (y - tmp.shape[1]) - (y - tmp.shape[1]) // 2),
                               ((z - tmp.shape[2]) // 2, (z - tmp.shape[2]) - (z - tmp.shape[2]) // 2))))
        if tmp_dist < dist:
            idx = i
            dist = tmp_dist
    return idx

def find_reference_mesh_index(mesh_list):
    num_mesh = len(mesh_list)
    median_mesh_index = None
    min_sum = np.inf
    dists = np.zeros((num_mesh, num_mesh))
    for mesh_index in range(num_mesh):
        if (mesh_index + 1 < num_mesh):
            for comp_index in range(mesh_index + 1, num_mesh):
                dist = mesh_list[mesh_index].distance(mesh_list[comp_index]).getFieldMean("distance")
                dists[mesh_index, comp_index] = dist
                dists[comp_index, mesh_index] = dist
    dists = np.sum(dists, axis=0)
    median_index = np.argmax(dists)
    return median_index

def save_contour_as_vtp(points, lines, filename):
    """
    Generates a .vtp file for the given contour to use in ShapeWorks optimizer

    points:   Nx3 np.ndarray of points in the contour
    lines:    Mx2 np.ndarray of lines in the contour
    filename: output .vtp filename
    """
    vtk_pts = vtk.vtkPoints()
    n = points.shape[0]
    for j in range(n):
        x, y, z = points[j]
        vtk_pts.InsertNextPoint((x,y,z))

    vtk_lines = vtk.vtkCellArray()
    m = lines.shape[0]
    for j in range(m):
        vtk_line = vtk.vtkLine()
        vtk_line.GetPointIds().SetId(0, lines[j][0])
        vtk_line.GetPointIds().SetId(1, lines[j][1])
        vtk_lines.InsertNextCell(vtk_line)

    polydata = vtk.vtkPolyData()
    polydata.SetPoints(vtk_pts)
    polydata.SetLines(vtk_lines)

    writer = vtk.vtkXMLPolyDataWriter()
    writer.SetFileName(filename)
    writer.SetInputData(polydata)
    writer.Write()

def compute_line_indices(n, is_closed=True):
    """
    Given a number of points, return indices for lines(as np.ndarray) between successive pairs of points.

    n:         number of points
    is_closed: whether or not the last vertex is to to be connected to the first vertex
    """
    lines = np.zeros((n if is_closed else n-1, 2), dtype=int)
    for i in range(lines.shape[0]):
        lines[i] = [i, (i+1)%n]

    return lines