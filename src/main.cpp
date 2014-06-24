/*******************************************************************************
 * Filename      : main.cpp
 * Header File(s): 
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
/************************************
* Included Headers 
************************************/
#include "main.hpp"
#include "ZFileDecompressor.hpp"
#include <vector>
#include <iostream>
//#include <iomanip>
//#include <chrono>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

/************************************
* Namespaces 
************************************/
using namespace std;
//using namespace chrono;
//using namespace cv;

/************************************
* Local Variables 
************************************/

/************************************
* Local Functions 
************************************/

/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
int main ( int argc, char* argv[] ) 
{
  ZFileDecompressor z("../6500KARX20070808_102612.Z");
  vector<unsigned char> header = z.read(24);
  string tmp = "", tmp1 = "";
  unsigned int idx = 0;
  for ( unsigned int i = 0; i < 9; i++, idx++ )
  {
    tmp += header[idx];
  }
  for ( unsigned int i = 0; i < 3; i++, idx++ )
  {
    tmp1 += header[idx];
  }  
  int tmp2 = header[14]<<8|header[15];
  int tmp3 = header[16]<<24|header[17]<<16|header[18]<<8|header[19];

  cout << tmp << " " << tmp1 << " " << tmp2 << " " << tmp3 << endl;

  return 0;
}

/*
void tmp ( void )
{
  Mat image(800,800, CV_8UC3);
  for ( int i = 0; i < image.rows; i++ )
  {
    for ( int j = 0; j < image.cols; j++ )
    {
      image.at<Vec3b>(i,j)[0] = rand() % 255;
      image.at<Vec3b>(i,j)[1] = rand() % 255;
      image.at<Vec3b>(i,j)[2] = rand() % 255;
    } 
  }
  imshow("window", image);
  waitKey(0);  
}
void tmp1 ( void )
{
  ifstream file;
  typedef high_resolution_clock clock;
  streampos size;
  unsigned char * memblock;

  auto start = clock::now();
  file.open ("../6500KARX20070808_102612.Z", ios::in | ios::binary | ios::ate);
  auto end = clock::now();
  cout << "took " << fixed << duration_cast<nanoseconds>(end-start).count()  
       << " nanoseconds to open the file" << endl;

  if (file.is_open())
  {
    size = file.tellg();
    auto start = clock::now();
    memblock = new unsigned char[size];
    file.seekg (0, ios::beg);
    file.read ((char*)memblock, size);
    auto end = clock::now();
    file.close();

  Mat image(800,800, CV_8UC3);
  for ( int i = 0; i < image.rows; i++ )
  {
    for ( int j = 0; j < image.cols; j++ )
    {
      image.at<Vec3b>(i,j)[0] = memblock[i*800+j];
      image.at<Vec3b>(i,j)[1] = memblock[i*800+j];
      image.at<Vec3b>(i,j)[2] = memblock[i*800+j];
    } 
  }    

    cout << "the entire file content is in memory " << size  << " bytes" << endl;
    cout << "and took " << fixed << duration_cast<nanoseconds>(end-start).count()  
         << " nanoseconds to load the file into ram" << endl; 

    delete[] memblock;
  }
  else 
  {
    cout << "Unable to open file";  
  }
}
*/