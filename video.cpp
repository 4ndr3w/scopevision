#include <iostream>
#include <opencv2/opencv.hpp>
#include "SerialPort.h"
#include <sys/time.h>
#include <mpv/client.h>

using namespace std;
using namespace cv;

struct ArduinoData {
    char s;
    char x;
    char y;
};

int main() {
    mpv_handle *mpvctx = mpv_create();
    mpv_initialize(mpvctx);
    mpv_set_option_string(mpvctx, "no-vid", NULL);
    const char* path = "/home/andrew/badapple.avi";
    VideoCapture video(path);

    double period = 1.0/(double)video.get(CV_CAP_PROP_FPS);
    int msDelay = period*1000.0;

    SerialPort<ArduinoData, ArduinoData> serial("/dev/ttyUSB0", B115200);
    cout << "ready" << endl;
    ArduinoData point = {0,0, 0};
    serial.sendMessage(point);
    Mat m;
    Mat edges;

    const char* cmd[] = { "loadfile", path, NULL };
    mpv_command(mpvctx, cmd); 

    struct timeval timeStart;
    struct timeval timeEnd;

    while ( video.isOpened() ) {
        video.read(m);
	gettimeofday(&timeStart, NULL);
        resize(m, m, Size(256,256));
        blur(m,m, Size(4,4));
        Canny(m, edges, 100, 300, 3); 
        imshow("edges", edges);

        int skip = 0;
        for ( int x = 1; x < 256; x++ ) {
            for ( int y = 1; y < 256; y++ ) {
                uchar val = edges.at<uchar>(x,y);
                if ( val == 255 ) {
                    if ( skip % 10 == 0 ) {
                        point.x = 256-x;
                        point.y = y;
                        serial.sendMessage(point);
//                        cout << (255-x) << "," << (255-y) << endl;
                    }
                    skip++;
                }
            }
        }
        gettimeofday(&timeEnd, NULL);
        long elapsed = (timeEnd.tv_sec-timeStart.tv_sec)*1000000+ timeEnd.tv_usec-timeStart.tv_usec;
        elapsed /= 1000;
        int realDelay = msDelay-elapsed;
        cout << elapsed << endl;
        if ( realDelay < 1 )
            realDelay = 1;
        waitKey(realDelay);
    }
    cout << "exit" << endl;

}
