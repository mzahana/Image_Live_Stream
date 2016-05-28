# Image_Live_Stream
This application allows live stream of images captured from camera (e.g. webcam) through network using UDP. It also includes MATLAB routines to allow receiving streamed images in MATLAB. This application **requries** OpenCV to be installed in the **default** system directories.

**NOTE:** This currently  works only on Mac OS. Future updates will include Windows installation guide, Odroid installation guide.
# Setup on Mac
### Install OpenCV
Please follow the steps in the following video to install OpenCV
https://www.youtube.com/watch?v=U49CVY8yOxw

### Compile sender/receiver c++ apps
The image sender c++ application is  located in the folder `Image_Live_Stream/opencv_stream/stream_cpp`. If you would like to run this on Mac OS, then, first , make sure that you installed OpenCV as mentioned above. Then, you can simply compile the `sender` application by `cmake . & make`. If everything goes fine, you shoul see the executable files `sender` & `receiver` in the same folder. The `receiver` executable can be used to test the image reception.

To run the `sender` application, simply, in a Mac terminal, navigate to the applicaiton folder, and run `./sender 127.0.0.1 10000`. The `127.0.0.1` is the local host IP, you can change it if you are receiving the image stream on a different machine. the `10000` is the port the remote machine is receving on.

To receive image stream in MATLAB, see next section.

### MATLAB setup
Once OpenCV header files and libraries are installed in the `/usr/local/include` and `/usr/local/lib` folders, you can setup the MATLAB files.

NOTE: You need to make sure that you associate your MATLAB with a compiler. On Mac, you can use Xcode.

Navigate to the `Image_Live_Stream` folder and run `setup.m` file. This will compile the neccessary `mex` files and add the folders to MATLAB path. This setup is a test setup, and assumes that the image sender application is sending to `localhost` which is `127.0.0.1` and local port `10000`. If you would like to change that, navigate to the `opencv_stream/stream_matlab` folder. Open the `receive_image.m` script, and change local port `lport`, `remotehost` and `remoteport` to the desired ones. Note that the c++ sender application uses port `12345`.

Enjoy.

# Setup on Windows
**To be done!**

# OpenCV setup on Odroid
**To be done!**

