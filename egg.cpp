#include "egg.h"

Egg::Egg(){}
Egg::Egg(vector<pair<int, int> > p, int middle)
{
  points.assign(p.begin(), p.end());
  center.x = (double)(p.at(p.size()/2).first + p.at(p.size()/2).second)/2;
  center.y = middle + p.size();
}
Egg::Egg(vector<vector<pair<int, int> > >::iterator first, vector<vector<pair<int, int> > >::iterator last, int middle)
{
  for(auto it = first; it != last; ++it)
    points.push_back(it->front());
  center.x = (double)(points.front().first + points.back().second)/2;
  center.y = middle;
}
double Egg::FindDistance(Egg egg)
{
  double dx = center.x - egg.center.x;
  double dy = center.y - egg.center.y;
  return sqrt(dx * dx + dy * dy);
}
Mat Egg::PlotGraph(vector<int> points, bool zero)
{
  int bottom = 100;
  if(zero) bottom = 50;
  Mat graph = Mat::zeros(100, points.size(), CV_8UC1);
  for(vector<int>::iterator i = points.begin(); i != points.end() - 1; i++)
    line(graph, Point(distance(points.begin(), i), bottom - *i), Point(distance(points.begin(), i + 1), bottom - *(i + 1)), 255);
  return graph;
}
