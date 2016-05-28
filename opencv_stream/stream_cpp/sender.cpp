/*
 *   C++ UDP socket client for live image upstreaming
 *   Modified from http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/UDPEchoClient.cpp
 *   Copyright (C) 2015
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PracticalSocket.h"      // For UDPSocket and SocketException
#include <iostream>               // For cout and cerr
#include <cstdlib>                // For atoi()

#define BUF_LEN 65540
#define LCLPORT=12345

using namespace std;

#include "opencv2/opencv.hpp"
using namespace cv;
#include "config.h"


int main(int argc, char * argv[]) {
    if ((argc < 3) || (argc > 3)) { // Test for correct number of arguments
        cerr << "Usage: " << argv[0] << " <Server> <Server Port>\n";
        exit(1);
    }

    string servAddress = argv[1]; // First arg: server address
    unsigned short servPort = Socket::resolveService(argv[2], "udp");
    bool confirmed=false;
    char buffer[BUF_LEN]; // Buffer
    int recvMsgSize;
    bool s1_ack=false;
    clock_t start_t;
    clock_t end_t;
    bool ack=false;

    try {
        UDPSocket sock(12345);
        int jpegqual =  ENCODE_QUALITY; // Compression Parameter

        Mat frame, send;
        vector < uchar > encoded;
        VideoCapture cap(0); // Grab the camera
        //namedWindow("send", CV_WINDOW_AUTOSIZE);
        if (!cap.isOpened()) {
            cerr << "OpenCV Failed to open camera";
            exit(1);
        }

        clock_t last_cycle = clock();
        while (1) {
            cap >> frame;
            if(frame.size().width==0)continue;//simple integrity check; skip erroneous data...
            resize(frame, send, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);
            vector < int > compression_params;
            compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
            compression_params.push_back(jpegqual);

            imencode(".jpg", send, encoded, compression_params);
            //imshow("send", send);
            int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;
            
            
            // send state 1 (=99) + total number of packets to receiver
            int ibuf[3];
            ibuf[0]=9; ibuf[1]=9; ibuf[2] = total_pack;
            sock.sendTo(ibuf, sizeof(int)*3, servAddress, servPort);
            
            //----- wait for confirmation from receiver (that state 1 & number of packets are received)
            start_t=clock();
            do {
                //recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
                recvMsgSize = sock.recv(buffer, BUF_LEN);
                sock.sendTo(ibuf, sizeof(int)*3, servAddress, servPort);
                cout << "total_pcakets..." << total_pack << endl;

            } while (recvMsgSize > sizeof(int) || ( ((int * ) buffer)[0] != total_pack));
            
            // Now state 2
            // send image data to receiver
            for (int i = 0; i < total_pack; i++)
            {
                sock.sendTo( & encoded[i * PACK_SIZE], PACK_SIZE, servAddress, servPort);
                start_t=clock();
                // WAIT FOR CONFIRMATION
                ack=true;
                do {
                    recvMsgSize = sock.recv(buffer, BUF_LEN);
                    //cout <<endl << "receiver packet reception: " << recvMsgSize << endl;
                    
                    // break if it takes long time
                    if (((clock() - start_t)/ (double) CLOCKS_PER_SEC)>2)
                    {
                        ack=false;
                        cout << "going to break packets.." << endl;
                        break;
                    }
                    //sock.sendTo( & encoded[i * PACK_SIZE], PACK_SIZE, servAddress, servPort);
                    
                } while (recvMsgSize > sizeof(int) || ( ((int * ) buffer)[0] != 999));
                if (not ack)
                    break;
            }
            

            if (ack)
            {
                waitKey(FRAME_INTERVAL);

                clock_t next_cycle = clock();
                double duration = (next_cycle - last_cycle) / (double) CLOCKS_PER_SEC;
                cout << endl << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;

                cout << next_cycle - last_cycle;
                last_cycle = next_cycle;
            }
        }
        // Destructor closes the socket

    } catch (SocketException & e) {
        cerr << e.what() << endl;
        exit(1);
    }

    return 0;
}
