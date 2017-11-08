#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    Mat m = cv::imread("/home/andrew/Desktop/pittlogo.png");
    resize(m, m, Size(256,256));
    blur(m,m, Size(3,3));
    Mat edges;
    Canny(m, edges, 100, 300, 3); 
    imshow("edges", edges);

    int n = 0;
    int sm = 0;
    for ( int x = 1; x < 256; x++ ) {
        for ( int y = 1; y < 256; y++ ) {
            uchar val = edges.at<uchar>(x,y);
            if ( val == 255 ) {
                if ( sm % 5 == 0 ) {
                    cout << (255-x) << "," << (255-y) << endl;
                    n++;
                }
                sm++;
            }
        }
    }

    cout << n << endl;
    waitKey(0);
}
