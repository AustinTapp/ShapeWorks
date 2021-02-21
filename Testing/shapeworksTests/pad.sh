#!/bin/bash

shapeworks readimage --name $DATA/1x2x2.nrrd pad compareimage --name $DATA/pad1.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd pad --padding -1 compareimage --name $DATA/pad2.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd pad --value 1.0 compareimage --name $DATA/pad3.nrrd
if [[ $? != 0 ]]; then exit -1; fi
shapeworks readimage --name $DATA/1x2x2.nrrd pad --padx 100 --pady 25 --padz 1 --value 10.0 compareimage --name $DATA/pad4.nrrd
