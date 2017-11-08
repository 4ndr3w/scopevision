#include <iostream>
#include <opencv2/opencv.hpp>
#include "SerialPort.h"
#include <sys/time.h>

using namespace std;
using namespace cv;

struct ArduinoData {
    char s;
    uchar x;
    uchar y;
};

int main() {
    VideoCapture video("/home/andrew/badapple.avi");

    double period = 1.0/(double)video.get(CV_CAP_PROP_FPS);
    int msDelay = period*1000.0;

    SerialPort<ArduinoData, ArduinoData> serial("/dev/ttyUSB0", B115200);
    cout << "ready" << endl;
    ArduinoData point = {0,0, 0};
    serial.sendMessage(point);
    Mat m;
    Mat edges;


    fstream file("badapple.scope", fstream::out | fstream::trunc);
    
    while ( video.isOpened() ) {
        if ( !video.read(m) )
            break;

        resize(m, m, Size(256,256));
        blur(m,m, Size(4,4));
        Canny(m, edges, 100, 300, 3); 

        int skip = 0;
        for ( int x = 1; x < 256; x++ ) {
            for ( int y = 1; y < 256; y++ ) {
                if ( x == '\n' || y == '\n' )
                    continue;
                uchar val = edges.at<uchar>(x,y);
                if ( val == 255 ) {
                    if ( skip % 10 == 0 ) {
                        file << (uchar)(256-x);
                        file << (uchar)y;
                    }
                    skip++;
                }
            }
        }
        file << '\n';
    }
    file.close();
}
