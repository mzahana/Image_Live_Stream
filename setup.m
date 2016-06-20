%% setup environment
% if ~ismac
%     error('Sorry, only supported on Mac OS for now')
% end
cd matlab_udp_class
setup_udp_toolbox
cd ../opencv_stream
cd stream_matlab
setup_mex
cd ../..

% add folders to matlab path
current_folder=pwd;
addpath(genpath(current_folder))

disp('Setup is Done.')