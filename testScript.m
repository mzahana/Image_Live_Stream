%% test script of the ImgStream class

%% create object
clear classes
clc

% create the object
obj=ImgStream;
%% set connection parameters
obj.set_LocalPort(10000);
obj.set_remoteHost('127.0.0.1')
%% connect
%1 if you want to display live images, 0 otherwise
obj.showImage=1;
% connect, and start receiving images
obj.Connect();
%% image data
% image date is stored in 
% obj.img
% you can use that variable to process the received image.
%% close/clean object
obj.Disconnect();
obj.delete();