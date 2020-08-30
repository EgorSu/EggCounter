#ifndef EGG_H
#define EGG_H
#include <process.h>
#include "qelapsedtimer.h"
using namespace std;
using namespace cv;
class Egg
{
  public:
  const int thresh = 10;
  Egg();
  Egg(vector<pair<int,int>>, int);
  Egg(vector<vector<pair<int, int> > >::iterator, vector<vector<pair<int, int> >>::iterator, int);
  vector<int> sizes;
  vector<pair<int,int> > points;
  Point2d center;
  vector<int> feature;
  double FindDistance(Egg);
  Mat PlotGraph(vector<int>, bool);
  void AddSizeNumber(pair<int,int>);
  void FilterSize();
};

#endif // EGG_H
