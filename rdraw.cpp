#include <iostream>
#include <opencv2/opencv.hpp>
#include "SerialPort.h"

using namespace std;
using namespace cv;

struct ArduinoData {
    char s;
    char x;
    char y;
};

int main() {
    SerialPort<ArduinoData, ArduinoData> serial("/dev/ttyUSB0", B115200);
    Mat m = cv::imread("/home/andrew/Desktop/pumpkin.png");
    resize(m, m, Size(256,256));
    blur(m,m, Size(3,3));
    Mat edges;
    Canny(m, edges, 100, 300, 3); 
    imshow("edges", edges);
    waitKey(0);

    ArduinoData point = {0,0,0};
    serial.sendMessage(point);

    sleep(1);
    while ( 1 ) {
        int skip = 0;
        for ( int x = 0; x < 256; x++ ) {
            for ( int y = 0; y < 256; y++ ) {
                if ( x == 0 && y == 0 )
                    continue;
                uchar val = edges.at<uchar>(x,y);
                if ( val == 255 ) {
                    if ( skip % 2 == 0 ) {
                        point.x = 255-x;
                        point.y = y;
                        serial.sendMessage(point);
                        cout << "{" << (255-x) << "," << (255-y) << "}," <<  endl;
                    }
                    skip++;
                }
            }
        }
        return 0;
    }

}
