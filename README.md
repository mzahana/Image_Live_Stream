# Image_Live_Stream
This application allows live stream of images captured from camera (e.g. webcam) through network using UDP. It also includes MATLAB routines to allow receiving streamed images in MATLAB. This application **requries** OpenCV to be installed in the **default** system directories.

**NOTE:** This currently  works only on Mac OS. Future updates will include Windows installation guide, Odroid installation guide.

# Install OpenCV

### Mac OS
Please follow the steps in the following video to install OpenCV
https://www.youtube.com/watch?v=U49CVY8yOxw

## Compile sender/receiver c++ apps

# MATLAB setup
Once OpenCV header files and libraries are installed in the `/usr/local/include` and `/usr/local/lib` folders, you can setup the MATLAB files.

Navigate to the `Image_Live_Stream` folder and run `setup.m` file. This will compile the neccessary `mex` files and add the folders to MATLAB path. This setup is a test setup, and assumes that the image sender application is sending to `localhost` which is `127.0.0.1` and local port `10000`. If you would like to change that, navigate to the `opencv_stream/stream_matlab` folder. Open the `receive_image.m` script, and change local port `lport`, `remotehost` and `remoteport` to the desired ones. Note that the c++ sender application uses port `12345`.

Enjoy.

