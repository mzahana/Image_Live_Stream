/*
 *   C++ UDP socket server for live image upstreaming
 *   Modified from http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/UDPEchoServer.cpp
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

#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()

#define BUF_LEN 65540 // Larger than maximum UDP packet size

#include "opencv2/opencv.hpp"
using namespace cv;
#include "config.h"

int main(int argc, char * argv[]) {

    if (argc != 2) { // Test for correct number of parameters
        cerr << "Usage: " << argv[0] << " <Server Port>" << endl;
        exit(1);
    }

    unsigned short servPort = atoi(argv[1]); // First arg:  local port
    int test_value1, test_value2;
    bool s1_ack=false;
    int total_pack;

    namedWindow("recv", CV_WINDOW_AUTOSIZE);
    try {
        UDPSocket sock(servPort);

        char buffer[BUF_LEN]; // Buffer for echo string
        int recvMsgSize; // Size of received message
        string sourceAddress; // Address of datagram source
        unsigned short sourcePort; // Port of datagram source

        clock_t last_cycle = clock();
        clock_t start_t;
        int ibuf[0];
        char temp_buf[sizeof(int)];
        int counter=0;
        bool wrong_packets=false;

        while (1) {
            recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            
            cout << "received ... " <<recvMsgSize << endl;
            memcpy(temp_buf,&buffer[0],sizeof(int));
            test_value1=((int *) temp_buf)[0];
            memcpy(temp_buf,&buffer[4],sizeof(int));
            test_value2=((int *) temp_buf)[0];
            
            memcpy(temp_buf,&buffer[8],sizeof(int));
            
            cout << "value 1... "<< test_value1 << endl;
            cout << "value 2... "<< test_value2 << endl;
            cout << " number of packets..." << ((int *) temp_buf)[0] << endl;
            //cout << "value 2... "<< test_value2 << endl;
            
            //if(0){
            if (test_value1==9 && test_value2==9) {
                cout << "got state 1.." << endl;
                s1_ack=true;
                memcpy(temp_buf,&buffer[8],sizeof(int));
                total_pack= ((int * ) temp_buf)[0];
                
                //----- confirm back to sender (that we receiverd number of packet)
                ibuf[0]=total_pack;
                sock.sendTo(ibuf, sizeof(int), sourceAddress, sourcePort);
                
                // start receiving packets
                cout << "expecting length of packs:" << total_pack << endl;
                char * longbuf = new char[PACK_SIZE * total_pack];
                start_t=clock();
                counter=0;
                wrong_packets=false;
                while (counter<total_pack) {
                    if ( ((clock()-start_t)/(double)CLOCKS_PER_SEC)>2 ) {
                        wrong_packets=true;
                        break;
                    }
                    recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
                    if (recvMsgSize != PACK_SIZE) {
                        cerr << "Received unexpected size pack:" << recvMsgSize << endl;
                        continue;
                    }
                    memcpy( & longbuf[counter * PACK_SIZE], buffer, PACK_SIZE);
                    counter++;
                    //----- confirm back to sender (that we receiverd one image packet)
                    ibuf[0]=999;
                    sock.sendTo(ibuf, sizeof(int), sourceAddress, sourcePort);
                    
                }
                
                if (not wrong_packets)
                {
                    // process image data
                    Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
                    Mat frame = imdecode(rawData, CV_LOAD_IMAGE_COLOR);
                    if (frame.size().width == 0) {
                        cerr << "decode failure!" << endl;
                        continue;
                    }
                    imshow("recv", frame);
                    free(longbuf);
                    
                    waitKey(1);
                    clock_t next_cycle = clock();
                    double duration = (next_cycle - last_cycle) / (double) CLOCKS_PER_SEC;
                    cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;
                    
                    cout << next_cycle - last_cycle;
                    last_cycle = next_cycle;
                }
            }// done checking state 1
            
        } // end of while(1)
        
    } catch (SocketException & e) {
        cerr << e.what() << endl;
        exit(1);
    }

    return 0;
}