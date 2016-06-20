%% test script of the ImgStream class

%% create object
clear classes
close all
clc

% create the object
obj=ImgStream;
%% set connection parameters
obj.set_LocalPort(10000);
obj.set_remoteHost('192.168.1.113')
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
close all
