%% test script of the ImgStream class

%% create object
clear classes
clc
obj=ImgStream;
%% connect
obj.showImage=1;
obj.Connect();
%% image data
% image date is stored in 
% obj.img
% you can use that variable to manipulate the received image.
%%
obj.Disconnect();
%% close/clean
obj.Disconnect();
obj.delete();