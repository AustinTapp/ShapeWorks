# -*- coding: utf-8 -*-
"""
====================================================================
Full Example Pipeline for Statistical Shape Modeling with ShapeWorks
====================================================================

In this example, we provide unpre-processed left atrial dataset,
which consists of the LGE scan and its segmentation across some number of subjects.
Since the data set is not pre-processed, it requires all re-processing steps;
resampling, padding, alignment, etc. To run the ShapeWorks particle-based optimization,
the processed segmentation files are converted to the signed distance transform.
This example is set to serve as a test case for users who want to process the raw(gray-sclae)
images as well as their binary segmentation images.

In this example, the raw and segmentation data are sharing the same functions for pre-processing,
such as the same transformation matrices for the center of mass and rigid alignment or same bounding box for cropping.

First import the necessary modules
"""

from zipfile import ZipFile
import os
import sys
import csv
import argparse
import glob

from GroomUtils import *
from OptimizeUtils import *
from AnalyzeUtils import *
import CommonUtils

def Run_Pipeline(args):
    """
    Unzip the data for this tutorial.

    The data is inside the leftatrium.zip, run the following function to unzip the
    data and create necessary supporting files. The files will be Extracted in a
    newly created Directory TestEllipsoids.
    This data is LGE segmentation of left atrium.
    For a detailed explanation of grooming steps see: https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/Workflow/Groom.md
    """

    """
    Extract the zipfile into proper directory and create necessary supporting
    files
    """

    print("\nStep 1. Extract Data\n")
    if int(args.interactive) != 0:
        input("Press Enter to continue")

    datasetName = "vent_centered_sub"
    filename = datasetName
    # Check if the data is in the right place
    if not os.path.exists(filename):
        print("Can't find " + filename + " in the current directory.")
        import DatasetUtils
        DatasetUtils.downloadDataset(datasetName)

    parentDir = "Test_vent_centered_sub/"
    if not os.path.exists(parentDir):
        os.makedirs(parentDir)
    
    fileList_seg = sorted(glob.glob(datasetName + "/segmentations/*.nrrd"))

    if args.tiny_test:
        fileList_img = fileList_img[:2]
        fileList_seg = fileList_seg[:2]
        args.use_single_scale = True
        
    if args.use_subsample:
        sample_idx = sampledata(fileList_seg, int(args.use_subsample))
        fileList_seg= [fileList_seg[i] for i in sample_idx]
        fileList_img = [fileList_img[i] for i in sample_idx]
    else:            
        
        if args.groom_images:
            
            """
            ## GROOM : Data Pre-processing
            For the unprepped data the first few steps are
            -- Isotropic resampling
            -- Padding
            -- Center of Mass Alignment
            -- Rigid Alignment
            -- Largest Bounding Box and Cropping

            For detailed explainations of parameters for each tool, go to
            'https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/ImagePrepTools.pdf'
            'https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/AlgnmentTools.pdf'
            """

            print("\nStep 2. Groom - Data Pre-processing\n")
            if args.interactive:
                input("Press Enter to continue")

            # create the output directory
            prepDir = parentDir+'PrepOutput/'
            if not os.path.exists(prepDir):
                os.makedirs(prepDir)

            """
            Apply isotropic resampling

            For detailed explainations of parameters for resampling volumes, go to
            '/Documentation/PDFs/ImagePrepTools.pdf'
            """
            print(fileList_seg)
            
            resampledFiles = applyIsotropicResampling(prepDir + "resampled", fileList_seg, isBinary=True)
            
            print(resampledFiles)

            """Apply Centering"""
            centeredFiles = center(prepDir + "centered", resampledFiles)

            """
            Apply padding
            For detailed explainations of parameters for padding volumes, go to
            '/Documentation/PDFs/ImagePrepTools.pdf'
            """
            paddedFiles = applyPadding(prepDir + "padded", centeredFiles, 10)

            

            """
            Apply rigid alignment

            For detailed explainations of parameters for rigid alignment of volumes, go to
            '/Documentation/PDFs/AlgnmentTools.pdf'

            Rigid alignment needs a reference file to align all the input files, FindMedianImage function defines the median file as the reference.
            """
    #            medianFile = FindReferenceImage(comFiles)
    #            rigidFiles = applyRigidAlignment(parentDir + "aligned", comFiles, None, medianFile)

            """
            Compute largest bounding box and apply cropping

            For detailed explainations of parameters for finding the largest bounding box and cropping, go to
            '/Documentation/PDFs/ImagePrepTools.pdf'
            """
            croppedFiles = applyCropping(prepDir + "cropped", paddedFiles, paddedFiles)

            print("\nStep 3. Groom - Convert to distance transforms\n")
            if args.interactive:
                input("Press Enter to continue")

            """
            We convert the scans to distance transforms, this step is common for both the
            prepped as well as unprepped data, just provide correct filenames.
            """
            dtFiles = applyDistanceTransforms(prepDir, croppedFiles)
        else:
            dtFiles = sorted(glob.glob( parentDir + "PrepOutput/distance_transforms/*.nrrd" ))
            if len(dtFiles)<2:
                raise ValueError(" no distance transforms. Run with --groom_images flag")
                
            

    """
    ## OPTIMIZE : Particle Based Optimization

    Now that we have the distance transform representation of data we create
    the parameter files for the shapeworks particle optimization routine.
    For more details on the plethora of parameters for shapeworks please refer to
    '/Documentation/PDFs/ParameterDescription.pdf'

    We provide two different mode of operations for the ShapeWorks particle opimization:
    1- Single Scale model takes fixed number of particles and performs the optimization.
    For more detail about the optimization steps and parameters please refer to
    '/Documentation/PDFs/ScriptUsage.pdf'

    2- Multi scale model optimizes for different number of particles in hierarchical manner.
    For more detail about the optimization steps and parameters please refer to
    '/Documentation/PDFs/ScriptUsage.pdf'

    First we need to create a dictionary for all the parameters required by these
    optimization routines
    """

    print("\nStep 4. Optimize - Particle Based Optimization\n")
    if args.interactive:
        input("Press Enter to continue")

    pointDir = parentDir+'PointFiles/'
    if not os.path.exists(pointDir):
        os.makedirs(pointDir)

    parameterDictionary = {
        "number_of_particles": 1024,
        "use_normals": [1],
        "normal_weight": [100.0],
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 5000,
        "optimization_iterations": 5000,
        "starting_regularization": 500000,
        "ending_regularization": 10,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "relative_weighting": 20,
        "domain_type" : 'image',
        "initial_relative_weighting": 0.1,
        "procrustes_interval": 0,
        "procrustes_scaling": 1,
        "save_init_splits": 0,
        "debug_projection": 0,
        "verbosity": 3
    }

    if not args.use_single_scale:
        parameterDictionary["use_shape_statistics_after"] = 128
        
    if args.tiny_test:
        parameterDictionary["number_of_particles"] = 32
        parameterDictionary["optimization_iterations"] = 25
        parameterDictionary["iterations_per_split"] = 25
            
    """
    Now we execute the particle optimization function.
    """
    [localPointFiles, worldPointFiles] = runShapeWorksOptimize(pointDir, dtFiles, parameterDictionary)

    if args.tiny_test:
        print("Done with tiny test")
        exit()

    """
    ## ANALYZE : Shape Analysis and Visualization

    Shapeworks yields relatively sparse correspondence models that may be inadequate to reconstruct
    thin structures and high curvature regions of the underlying anatomical surfaces.
    However, for many applications, we require a denser correspondence model, for example,
    to construct better surface meshes, make more detailed measurements, or conduct biomechanical
    or other simulations on mesh surfaces. One option for denser modeling is
    to increase the number of particles per shape sample. However, this approach necessarily
    increases the computational overhead, especially when modeling large clinical cohorts.

    Here we adopt a template-deformation approach to establish an inter-sample dense surface correspondence,
    given a sparse set of optimized particles. To avoid introducing bias due to the template choice, we developed
    an unbiased framework for template mesh construction. The dense template mesh is then constructed
    by triangulating the isosurface of the mean distance transform. This unbiased strategy will preserve
    the topology of the desired anatomy  by taking into account the shape population of interest.
    In order to recover a sample-specific surface mesh, a warping function is constructed using the
    sample-level particle system and the mean/template particle system as control points.
    This warping function is then used to deform the template dense mesh to the sample space.
    """

    print("\nStep 5. Analysis - Launch ShapeWorksStudio.\n")
    if args.interactive :
        input("Press Enter to continue")

    command = "ShapeWorksStudio "+ pointDir+ "analyze.xml"
    print(command)
    os.system(command)
