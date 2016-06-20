%%
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
    p=getenv('OPENCV_DIR');
    if isempty(p)
        error('OPENCV_DIR is not set.');
    end
    OCVRoot = [p,'\..\..\include'];
    IPath = ['-I',OCVRoot];
    LPath = [p,'\lib'];
    lib1=fullfile(LPath,'opencv_highgui2413.lib');
    lib2=fullfile(LPath,'opencv_core2413.lib');
    lib3=fullfile(LPath,'opencv_imgcodecs2413.lib');
    lib4=fullfile(LPath,'opencv_ml2413.lib');
    lib5=fullfile(LPath,'opencv_photo2413.lib');
    lib6=fullfile(LPath,'opencv_calib3d2413.lib');
    lib7=fullfile(LPath,'opencv_features2d2413.lib');

    mex('decodeUDPimg.cpp', IPath, lib1, lib2,lib4,lib5,lib6,lib7);
    %mex('decodeUDPimg.cpp', IPath, lib1, lib2);
end