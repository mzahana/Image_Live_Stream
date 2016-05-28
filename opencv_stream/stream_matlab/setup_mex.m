%%
clear all
clc
%%
if ismac
    OCVRoot = '/usr/local/include';
    IPath = ['-I',OCVRoot];
    LPath = '/usr/local/lib';
    lib1=fullfile(LPath,'libopencv_highgui.dylib');
    lib2=fullfile(LPath,'libopencv_core.dylib');
    lib3=fullfile(LPath,'libopencv_imgcodecs.dylib');
    lib4=fullfile(LPath,'libopencv_ml.dylib');
    lib5=fullfile(LPath,'libopencv_photo.dylib');
    lib6=fullfile(LPath,'libopencv_calib3d.dylib');
    lib7=fullfile(LPath,'libopencv_features2d.dylib');


    mex('decodeUDPimg.cpp', IPath, lib1, lib2,lib3,lib4,lib5,lib6,lib7);
end

if ispc
end