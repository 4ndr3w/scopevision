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

ArduinoData* sendBuf;
int sendLen = 0;
int bufSize = 500;
void addPointToBuf(ArduinoData d) {
    if ( sendLen == bufSize-1 ) {
        bufSize *= 2;
        sendBuf = (ArduinoData*)realloc(sendBuf, bufSize * sizeof(ArduinoData));
    }
    sendBuf[sendLen++] = d;
}

int compare(const void *a, const void *b) {
    ArduinoData* p1 = (ArduinoData*) a;
    ArduinoData* p2 = (ArduinoData*) b;
    int d1 = p1->x * p1->x + p1->y * p1->y;
    int d2 = p2->x * p2->x + p2->y * p2->y;
    return d1-d2;
}

int main() {
    mpv_handle *mpvctx = mpv_create();
    mpv_initialize(mpvctx);
//    mpv_set_option_string(mpvctx, "video", "no");
    sendBuf = (ArduinoData*)malloc(bufSize*sizeof(ArduinoData));
    double period = 1.0/30.0;
    int usDelay = period*1000000.0;

    SerialPort<ArduinoData, ArduinoData> serial("/dev/ttyUSB0", B115200);
    cout << "ready" << endl;
    ArduinoData point = {0,0, 0};
    serial.sendMessage(point);

    struct timeval timeStart;
    struct timeval timeEnd;
    fstream file("badapple.scope", fstream::in);

    const char* cmd[] = { "loadfile", "/home/andrew/badapple.avi", NULL };
    mpv_command(mpvctx, cmd); 
    while ( !file.eof() ) {
	gettimeofday(&timeStart, NULL);
        sendLen = 0;
        while ( true ) {
            file.get(point.x);
            if ( point.x == '\n' )
                break;
            file.get(point.y);
            point.y = 255 - point.y;
            addPointToBuf(point);
        }
//        qsort(sendBuf, sendLen, sizeof(ArduinoData), compare);
        write(serial.fd(), sendBuf, sendLen*sizeof(ArduinoData));
        gettimeofday(&timeEnd, NULL);
        long elapsed = (timeEnd.tv_sec-timeStart.tv_sec)*1000000+ timeEnd.tv_usec-timeStart.tv_usec;
        int realDelay = usDelay-elapsed;
        if ( realDelay > 1 )
            usleep(realDelay);
    }
    cout << "exit" << endl;

}
