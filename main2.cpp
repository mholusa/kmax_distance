/*============================================================================*/
/* main2.cpp                                                                  */
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


cv::Mat createDistanceMat( const double* dstImg, const int width, const int height )
{
    cv::Mat dstMat = cv::Mat::zeros(height, width, CV_8U);    
    double min = dstImg[0], max=dstImg[0];
    for(int i = 1; i < width*height; i++)
    {
        if(dstImg[i] < min) min = dstImg[i];
        if(dstImg[i] > max) max = dstImg[i];
    }
    
    if(max > MAX_GEO_DIST) { fprintf(stderr, "ERROR: can not create distance image (dstPoint is defined)\n"); return dstMat; }
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            double val = (dstImg[y*width+x] - min) / (max - min);
            dstMat.at<uchar>(y,x) = (uchar)(255*val);
        }
    }

    return dstMat;
}

void drawPath( cv::Mat& img, int* path, cv::Scalar color )
{
    int i = 0;
    while(path[i+1] > 0) {
        cv::line(img, cv::Point(path[i] % img.cols, path[i] / img.cols), cv::Point(path[i+1] % img.cols, path[i+1] / img.cols), color, 1);
        i++;
    }
}

int main( int argc, char** argv )
{
    const char* imgFileName = "img.png";
    
    cv::Mat inputImageGrey = cv::imread(imgFileName, 0);
    if(inputImageGrey.empty()) { printf("Image not found\n"); return -1; }
    
    int imageWidth = inputImageGrey.cols;
    int imageHeight = inputImageGrey.rows;
    int numImagePixs = imageWidth * imageHeight;
    
    double brSigma = 1.0/3.0;
    double sxy = 0.001;
    
    KMaxDistance kmax;
    int k = 5;
    
    double *image = (double*)malloc( numImagePixs*sizeof( image[0] ));
    double *xCosts = (double*)malloc( numImagePixs*sizeof( xCosts[0] ));
    double *yCosts = (double*)malloc( numImagePixs*sizeof( yCosts[0] ));
    double* distImg = (double*)malloc( numImagePixs*sizeof( image[0] ));
    int* backPtrs = (int*)malloc( numImagePixs*sizeof( backPtrs[0] ));
    double* maxVector = (double*)malloc( k*sizeof( maxVector[0] ));
    
    for(int i = 0; i < numImagePixs; i++) image[i] = (double)inputImageGrey.data[i]/255.0;
    
    kmax.edgeCosts( image, imageWidth, imageHeight, brSigma, sxy, xCosts, yCosts );
    
    cv::Point srcPoint = cv::Point(50, 50);
    cv::Point dstPoint = cv::Point(99, 99);
    const int numSeedPixs = 1;
    int seedPixs[numSeedPixs];
    seedPixs[0] = srcPoint.y * imageWidth + srcPoint.x;
    int dstPix = dstPoint.y * imageWidth + dstPoint.x;
    
    double dist = kmax.compute(xCosts, yCosts, imageWidth, imageHeight, k, seedPixs, numSeedPixs, -dstPix, distImg, backPtrs, maxVector);
    printf("distance: %.3f\n", dist);
    printf("vector: ( "); 
    for(int i = 0; i < k; i++) printf("%.3f ", maxVector[i]);
    printf(")\n");
    
    cv::Mat pathImg = inputImageGrey.clone();
    drawPath(pathImg, backPtrs, cv::Scalar(0,0,255));
    cv::Mat out = createDistanceMat(distImg, imageWidth, imageHeight);
    cv::imwrite("pathImg.png", pathImg);
    cv::imwrite("distImg.png", out);
    
    free(image); free(xCosts); free(yCosts); free(distImg); free(backPtrs); free(maxVector);
    
    return 0;

}
/*============================================================================*/

