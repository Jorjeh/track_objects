#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;

IplImage* GetThresholdedImageHSV(IplImage* img)
{
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    cvCvtColor(img, imgHSV, CV_BGR2HSV);
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
    cvInRangeS(imgHSV, cvScalar(20, 100, 100), cvScalar(30, 255, 255), imgThreshed);
    cvReleaseImage(&imgHSV);
    return imgThreshed;
}

int main()
{
    CvCapture* capture = 0;
    capture = cvCaptureFromCAM(0);
    if(!capture)
    {
        printf("Não é possível iniciar...\n");
        return -1;
    }
    cvNamedWindow("Entrada");
    IplImage* imgScribble = NULL;

    while(true)
    {
        IplImage* frame = 0;
        frame = cvQueryFrame(capture);

        if(!frame)
        break;

        if(imgScribble == NULL)
        {
            imgScribble = cvCreateImage(cvGetSize(frame), 8, 3);
        }

        IplImage* imgCorThresh = GetThresholdedImageHSV(frame);
        CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
        cvMoments(imgCorThresh, moments, 1);

        double moment10 = cvGetSpatialMoment(moments, 1, 0);
        double moment01 = cvGetSpatialMoment(moments, 0, 1);
        double area = cvGetCentralMoment(moments, 0, 0);

        static int posX = 0;
        static int posY = 0;
        int lastX = posX;
        int lastY = posY;

        posX = moment10/area;
        posY = moment01/area;
        printf("position (%d,%d)\n", posX, posY);
        if(lastX>0 && lastY>0 && posX>0 && posY>0)
        {
            cvLine(imgScribble, cvPoint(posX, posY), cvPoint(lastX, lastY), cvScalar(0,255,0), 5);
        }
 
        cvAdd(frame, imgScribble, frame);
        cvShowImage("Video acionado", frame);

        int c = cvWaitKey(10);
        if(c!=-1)
        {
            break;
        }
        cvReleaseImage(&imgCorThresh);
        delete moments;
    }
    cvReleaseCapture(&capture);
    return 0;
}

