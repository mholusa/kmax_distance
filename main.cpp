/*============================================================================*/
/* main.cpp                                                                   */
/*                                                        mrl vsb-tuo 2014-18 */
/*============================================================================*/

#include "kmax_distance.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

/*============================================================================*/

int loadSeeds( Image<unsigned char>& img, int* seeds, int* numPoints ) 
{
    for ( int i = 0; i < img.size; i++ ) {
        if ( img.vals[i] > 0 ) seeds[(*numPoints)++] = i;
        if(*numPoints == MAX_NUM_OF_SEEDS) { fprintf(stderr, "ERROR: too many seed points\n"); return 1; }
    }
    return 0;
}

cv::Mat createDistanceMat( Image<double>& dstImg )
{
    cv::Mat dstMat = cv::Mat::zeros(dstImg.height, dstImg.width, CV_8U);    
    double min = dstImg.vals[0], max=dstImg.vals[0];
    for(int i = 1; i < dstImg.size; i++)
    {
        if(dstImg.vals[i] < min) min = dstImg.vals[i];
        if(dstImg.vals[i] > max) max = dstImg.vals[i];
    }

    if(max > MAX_GEO_DIST - GEO_MAX_EPSX ) { fprintf(stderr, "ERROR: can not create distance image (dstPoint is defined)\n"); return dstMat; }
    for(int x = 0; x < dstMat.cols; x++)
    {
        for(int y = 0; y < dstMat.rows; y++)
        {
            double val = (dstImg.vals[y*dstMat.cols+x] - min) / (max - min);
            dstMat.at<uchar>(y,x) = (uchar)(255*val);
        }
    }

    return dstMat;
}


void drawPath( cv::Mat& img, const std::vector<int> path, cv::Scalar color )
{
	if( path.size() == 0 ) return;
    for(unsigned int i = 0; i < path.size()-1; i++) {
        cv::line(img, cv::Point(path[i] % img.cols, path[i] / img.cols), cv::Point(path[i+1] % img.cols, path[i+1] / img.cols), color, 1);
    }
}


int main( int argc, char** argv )
{
    const char* imgFileName = "img.png";
    
    cv::Mat inputImageGrey = cv::imread(imgFileName, 0);
    if(inputImageGrey.empty()) { printf("Image not found\n"); return -1; }
    
    KMaxDistance kmax;
    
    Image<unsigned char> img = Image<unsigned char>(inputImageGrey.cols, inputImageGrey.rows, inputImageGrey.data);
    Image<double> distImg = Image<double>(inputImageGrey.cols, inputImageGrey.rows);
    
    cv::Point srcPoint = cv::Point(50, 50);
    cv::Point dstPoint = cv::Point(99, 99);
    const int numSeedPixs = 1;
    int seedPixs[numSeedPixs];
    seedPixs[0] = srcPoint.y * img.width + srcPoint.x;
    int dstPix = dstPoint.y * img.width + dstPoint.x;
    
    std::vector<int> backPoints;
    std::vector<double> maxVector;
    int k = 5;
    
    double dist = kmax.compute(img, k, seedPixs, numSeedPixs, -dstPix, &distImg, &backPoints, &maxVector);
    printf("distance: %.3f\n", dist);
    printf("vector: ( "); 
    for(int i = 0; i < k; i++) printf("%.3f ", maxVector[i]);
    printf(")\n");
    
    cv::Mat pathImg = inputImageGrey.clone();
    drawPath(pathImg, backPoints, cv::Scalar(0,0,255));
    cv::Mat out = createDistanceMat(distImg);
    cv::imwrite("pathImg.png", pathImg);
    cv::imwrite("distImg.png", out);
    return 0;

}
/*============================================================================*/

