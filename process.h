#ifndef NEW_H
#define NEW_H
#include <opencv2/opencv.hpp>
#include <egg.h>
#include <time.h>
using namespace std;
using namespace cv;
class Egg;
class Process
{
  public:
  int rXY = 20;
  int lineC;
  int eggsNum = 0;
  int eggsInProcess = 0;
  int eggsInLast = 0;
  int col = 50;
  int beginEgg;
  int endEgg;
  int num = 0;
  int pointDistance = 10;
  int pointGroupNumber = 0;
  int eggSize = 0;
  vector<int> startPosition;
  vector<int*> sizes;
  Process();
  Process(Mat m, int i, vector<Point2d>*);
  Mat mat;
  Mat newMat;
  Mat newMat2;
  Mat matOld;
  vector<vector<pair<int,int>>> allPoints;
  vector<int> camParams;
  void GetFrame(Mat, int);
  Mat ProcessOld(int);
  Mat ProcessNew(int, int);
  void FilterSize();
  void FindNextPoint(vector<vector<pair<int, int> >>:: iterator, vector<pair<int, int> >::iterator, bool);
  void FindEgg();
  void GroupPoints();
  void DrawPoints();
  void CountEgg();
  void DrawAllPoints();
  vector<vector<pair<int, int> > > contourPoints;
  vector<vector<pair<int, int> > > groupPoints;
  vector<vector<int> > contourSize;
  //vector<Egg> eggs;
  void AddSize();
  Egg* egg;
  vector<Egg> eggs;
  vector<Point2d>* eggsPos;
};
#endif // NEW_H
