//////////////////////////////////////////////////////////////////////////
// Creates C++ MEX-file for OpenCV routine descodeUDPimg. 
// This routine decodes an uint8 input buffer to output image,
// using opencv functions
//
// Copyright 2016 Mohamed Abdelkader
//////////////////////////////////////////////////////////////////////////

#include <iostream>               // For cout and cerr
#include <cstdlib>                // For atoi()
#include <opencv2/opencv.hpp>
#include "mex.h"
#include "opencv_matlab.hpp"
/*
#define _DO_NOT_EXPORT
#if defined(_DO_NOT_EXPORT)
#define DllExport  
#else
#define DllExport __declspec(dllexport)
#endif
*/
unsigned char *InputBuffer;
int buffLen;
#include <iostream>               // For cout and cerr
#include <cstdlib>                // For atoi()
using namespace cv;
using namespace std;
///////////////////////////////////////////////////////////////////////////
// Check inputs
///////////////////////////////////////////////////////////////////////////
void checkInputs(int nrhs, const mxArray *prhs[])
{ 
    // Check number of inputs
    // Expecting 2 inputs: uint8 buffer, and size of buffer
    if (nrhs != 2)
    {
        mexErrMsgTxt("Incorrect number of inputs. Function expects 2 inputs.");
    }
    
    // Check buffer data type
    if (!mxIsUint8(prhs[0]))
    {
        mexErrMsgTxt("Buffer must be UINT8.");
    }
    
    if (prhs[1] == 0)
    {
        mexErrMsgTxt("Buffer length should  be positive.");
    }
}


///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
	// Check inputs to mex function
    checkInputs(nrhs, prhs);
    
    // process image data
    InputBuffer = (unsigned char *)mxGetData(prhs[0]);
    //cout << (int)InputBuffer[7]<<endl;
    buffLen= (int)mxGetScalar(prhs[1]);
    Mat rawData = Mat(1, buffLen, CV_8UC1, InputBuffer);
    //Mat frame;
    //Mat frame = imdecode(rawData, 0);
    Mat frame = imdecode(rawData, 0);
    //cout << "channels " << frame.channels();
    //imdecode(Mat(1, buffLen, CV_8U, 90InputBuffer), IMREAD_COLOR, &frame);
    
    if (frame.size().width == 0) {
        cout << "decode failure!" << endl;
        plhs[0] = mxCreateDoubleScalar(0);
        //plhs[1] = mxCreateDoubleScalar(1);
        
        return;
    }
    
       // Convert opencv to Matlab and set as output
    // @note: output is uint8
    int dims[3];
    int nDims;
    if (frame.channels()>1){
        nDims =frame.channels();
    }else
        nDims =2;
    
    if (nDims>2)
    {
        dims[0]=frame.rows; dims[1]=frame.cols; dims[2]=frame.channels();
    }
    else{
        dims[0]=frame.rows; dims[1]=frame.cols;
    }
    plhs[0] = mxCreateNumericArray(nDims, dims, mxUINT8_CLASS, mxREAL);
    om::copyMatrixToMatlab<unsigned char>(frame, (unsigned char*)mxGetPr(plhs[0]));
    
    // Put the data back into the output MATLAB array
   //plhs[0] = ocvMxArrayFromImage_single(frame);
    //plhs[0] = mxCreateDoubleScalar(0);
    
    return;
}