#include "process.h"
class Egg;
Process::Process() {}
Process::Process(Mat m, int i, vector<Point2d>* eggsCenter)
{
  matOld = Mat(m.rows, m.cols, CV_8UC1);
  m.copyTo(matOld);
  Mat rot_mat = getRotationMatrix2D( Point(matOld.cols/2, matOld.rows/2), 180.0, 1.0 );
  warpAffine(matOld, matOld, rot_mat, matOld.size());
  matOld.copyTo(newMat);
  lineC = matOld.cols/2;
  eggsPos = eggsCenter;
  num = i;
}
void Process::GetFrame(Mat m, int i)
{
  matOld = Mat(m.rows, m.cols, CV_8UC1);
  m.copyTo(matOld);
  Mat rot_mat = getRotationMatrix2D( Point(matOld.cols/2, matOld.rows/2), 180.0, 1.0 );
  warpAffine(matOld, matOld, rot_mat, matOld.size());
  matOld.copyTo(newMat);
  lineC = matOld.cols/2;
  num = i;
}
Mat Process::ProcessOld(int thresh)
{
  Mat mat = Mat::zeros(matOld.rows, matOld.cols, CV_8UC1);
  matOld.copyTo(mat);
  int const imageWidth(mat.size().width);
  int const imageHeigth(mat.size().height);
  bool isX = false;
  int xi, xj;
  contourPoints.clear();
  contourSize.clear();
  allPoints.clear();
  startPosition.clear();
  //allPoints = vector<vector<pair<int, int> > >(imageHeigth);
  contourPoints = vector<vector<pair<int, int> > >(imageHeigth);
  contourSize = vector<vector<int> >(imageHeigth);
  transpose(mat, mat);
  Mat m(mat.rows, mat.cols, CV_8UC1);
  m.setTo(Scalar(0));
  for (int y = 0; y < imageWidth; y++)
  {
      unsigned char *const scanLine(mat.ptr<unsigned char>(y));
      unsigned char *const scanLineM(m.ptr<unsigned char>(y));
      for (int x = 0; x < imageHeigth; x++)
      {
          if (scanLine[x] > thresh)
          {
              if (!isX)
              {
                  isX = true;
                  xi = x;
              }
          }
          else
          {

              if (isX)
              {
                  isX = false;
                  xj = x;
                  if (xj - xi > rXY)
                  {
                      for (int k = xi; k < xj; k++)
                          scanLineM[k] = 255;
                  }
              }
          }
      }
      if (isX)
      {
          isX = false;
          xj = imageHeigth;
          if (xj - xi > rXY)
          {
              for (int k = xi; k < xj; k++)
                  scanLineM[k] = 255;
          }
      }
  }
  transpose(m, m);
  Mat m2(m.size(), CV_8UC1);
  m2.setTo(Scalar(0));
  for (int y = 0; y < imageHeigth; y++)
  {
      unsigned char *const scanLine(m.ptr<unsigned char>(y));
      unsigned char *const scanLineM(m2.ptr<unsigned char>(y));
      for (int x = 0; x < imageWidth; x++)
      {
          if (scanLine[x] > thresh)
          {
              if (!isX)
              {
                  isX = true;
                  xi = x;
              }
          }
          else
          {
              if (isX)
              {
                  isX = false;
                  xj = x;
                  if (xj - xi > rXY)
                  {
                      for (int k = xi; k < xj; k++)
                          scanLineM[k] = 255;
                      contourPoints[y].push_back(pair<int, int>(xi, xj));
                      contourSize[y].push_back(xj - xi);
                  }
              }
          }
      }
      if (isX)
      {
          isX = false;
          xj = imageWidth;
          if (xj - xi > rXY)
          {
              for (int k = xi; k < xj; k++)
                  scanLineM[k] = 255;
              contourPoints[y].push_back(pair<int, int>(xi, xj));
              contourSize[y].push_back(xj - xi);
          }
      }
  }
  m2.copyTo(matOld);
  bool haveSize = true;

  while(haveSize)
  {
    bool isEmty = true;
    for(auto it = contourPoints.begin(); it != contourPoints.end(); ++it)
    {
      if(it->size() != 0)
      {
        isEmty = false;
        AddSize();
        break;
      }
    }
    if(isEmty)
      haveSize = false;
  }
  DrawAllPoints();
  //eggsLast.clear();
  //for(int i = 0; i < eggs.size(); i++)
    //eggsLast.push_back(eggs[i]);
  //eggs.clear();
  return m2;
}
void Process::FindNextPoint(vector<vector<pair<int, int> > >::iterator nextLinePair,
                           vector<pair<int, int> >::iterator startPair, bool direction)
{
  if(nextLinePair->size() > 0 )
  {
    int deltaX = (direction)? 1:-1;
    vector<pair<int, int> >::iterator itP2;
    for(itP2 = nextLinePair->begin(); itP2 != nextLinePair->end(); itP2++)
    {
      if(abs(startPair->first - itP2->first) < pointDistance)
      {
        int *p = &contourSize[distance(contourPoints.begin(), nextLinePair)][distance(nextLinePair->begin(), itP2)];
        if(direction)
        {
          endEgg++;
          //sizes.push_back(p);
        }
        else
        {
          beginEgg--;
          //sizes.insert(sizes.begin(), p);
        }
        FindNextPoint(nextLinePair + deltaX, itP2, direction);
        break;
      }
    }
  }
}
void Process::AddSize()
{
  sizes.clear();
  int min = matOld.cols;
  vector<vector<pair<int,int> > >::iterator startItP;
  for(auto itP = contourPoints.begin(); itP != contourPoints.end(); ++itP)
  {
    if(itP->size() > 0)// && itP->at(0).first <= 28) //!!
    {
      int value = itP->begin()->first;
      if(value < min)
      {
        min = value;
        startItP = itP;
      }
    }
  }
  if(min != matOld.cols)
  {
    //sizes.push_back(&contourSize[distance(contourPoints.begin(), startItP)][0]);
    beginEgg = distance(contourPoints.begin(), startItP);
    endEgg = distance(contourPoints.begin(), startItP);
    FindNextPoint(startItP + 1, startItP->begin(), true);
    FindNextPoint(startItP - 1, startItP->begin(), false);
  }
  eggSize = endEgg - beginEgg;
  if(eggSize > rXY)
      FilterSize();
  else if(eggSize == 0)
  {
    contourPoints.erase(contourPoints.begin() + beginEgg);
    contourSize.erase(contourSize.begin() + beginEgg);
  }
  else
  {
    contourPoints.erase(contourPoints.begin() + beginEgg, contourPoints.begin() + endEgg);
    contourSize.erase(contourSize.begin() + beginEgg, contourSize.begin() + endEgg);
  }
}
void Process::FilterSize()
{
  if(num == 451)
    int a = 0;
  vector<vector<int>>::iterator beginEggIt = contourSize.begin() + beginEgg;
  vector<vector<int>>::iterator endEggIt = contourSize.begin() + endEgg;
  vector<int> dx;
  vector<pair<int,int>> specPoints;
  for(int i = 0; i < eggSize; i++)
    dx.push_back((beginEggIt + i)->front() - (beginEggIt + i + 1)->front());//dx.push_back(*it - *(it - 1));
  //dx.push_back(-endEggIt->front()); //пока такоц костыль для вычисления dddx
  dx.push_back(dx.back());
  dx.insert(dx.begin(), dx.front());
  int nextddx = 0;
  int startPoint = beginEgg;
  int endPoint = endEgg;
  bool isStart = false;
  int a = 0;
  for(vector<int>::iterator it = dx.begin(); it != dx.end() - 1; it++)
  {
    a++;
    int ddx = nextddx;
    nextddx = *(it + 1) - *it;
    if(ddx == 0 || *it == 0)
       continue;
    if(!isStart && *it < 0 && ddx < 0 && nextddx > 0 && /* *it < -pointDistance * 1.8 &&*/
    ddx < -pointDistance/2.0 && nextddx > pointDistance/2.0)
    {
      int dddx0 = *it - 2 * (*(it - 1)) + *(it - 2);
      int dddx1 = *(it + 1) - 2 * (*it) + *(it - 1);
      int dddx2 = *(it + 2) - 2 * (*(it + 1)) + *it; //в точке it + 2
      if ((dddx2 < 0 || *(it + 2) > pointDistance) && dddx1 > 0 && (dddx0 < 0 || *(it - 2) > pointDistance))
      {
        startPoint = distance(dx.begin(), it - 1) + beginEgg;
        isStart = true;
      }
    }
    else if(isStart && *it > 0 && ddx > 0 && nextddx < 0 && /* *it > pointDistance * 1.8 && */
       ddx > pointDistance/2.0 && nextddx < -pointDistance/2.0)
    {
      int dddx0 = *it - 2 * (*(it - 1)) + *(it - 2);
      int dddx1 = *(it + 1) - 2 * (*it) + *(it - 1);
      int dddx2 = *(it + 2) - 2 * (*(it + 1)) + *it;  //в точке it + 2
      if ((dddx2 > 0 || *(it + 1) < -pointDistance) && dddx1 < 0 &&
      (dddx0 > 0 || startPoint ==  distance(dx.begin(), it) + beginEgg - 3))
      {
        endPoint = distance(dx.begin(), it) + beginEgg; // + beginEggIt;
        specPoints.push_back(pair<int,int>(startPoint, endPoint));
        startPoint = beginEgg;
        endPoint = endEgg;
        isStart = false;
      }
    }
  }
  if(isStart)
    specPoints.push_back(pair<int,int>(startPoint, endPoint));
  if(!specPoints.empty())
  {
    waitKey();
    double dy[specPoints.size()] = {0};
    for(auto it = specPoints.begin(); it != specPoints.end(); ++it)
    {
      if(it->first == beginEgg)
      {
        int size = rXY;
        int difference = beginEggIt->front() - size;
        if(difference > rXY)
        {
          beginEggIt->front() = size;
          beginEggIt->insert(beginEggIt->begin() + 1, difference);//it->first->insert(it->first->begin() + 1, difference);
          vector<vector<pair<int, int>>>::iterator iterPoint = contourPoints.begin() + distance(contourSize.begin(), beginEggIt);
          vector<pair<int, int>>::iterator iterPoint2 = iterPoint->begin();
          pair<int,int> newPoint = iterPoint->front();
          iterPoint2->second = iterPoint2->first + size;
          newPoint.first = iterPoint2->second + 1;
          iterPoint->insert(iterPoint->begin() + 1, newPoint);
        }
      }
      if(it->second == endEgg)
      {
        int size = rXY;
        int difference = endEggIt->front() - size;
        if(difference > rXY)
        {
          endEggIt->front() = size;
          endEggIt->insert(endEggIt->begin() + 1, difference);
          vector<vector<pair<int, int>>>::iterator iterPoint = contourPoints.begin() + distance(contourSize.begin(), endEggIt);
          vector<pair<int, int>>::iterator iterPoint2 = iterPoint->begin();
          pair<int,int> newPoint = iterPoint->front();
          iterPoint2->second = iterPoint2->first + size;
          newPoint.first = iterPoint2->second + 1;
          iterPoint->insert(iterPoint->begin() + 1, newPoint);
        }
      }
      double a = contourSize[it->second].front() - contourSize[it->first].front();
      double b = it->second - it->first;
      dy[distance(specPoints.begin(), it)] = a/b;
      for(auto i = contourSize.begin() + it->first + 1; i != contourSize.begin() + it->second; ++i)
      {
        int size = i->front();
        i->front() = (contourSize[it->first].front()) + (int)(dy[distance(specPoints.begin(), it)] * distance(contourSize.begin() + it->first + 1, i)); //!!!!
        int difference = size - i->front();
        i->insert(i->begin() + 1, difference);
        vector<vector<pair<int, int>>>::iterator iterPoint = contourPoints.begin() + distance(contourSize.begin(), i);
        vector<pair<int, int>>::iterator iterPoint2 = iterPoint->begin();
        pair<int,int> newPoint = iterPoint->front();
        iterPoint2->second = iterPoint2->first + size - difference;
        newPoint.first = iterPoint2->second + 1;
        iterPoint->insert(iterPoint->begin() + 1, newPoint);
      }
    }
  }
  Egg egg(contourPoints.begin() + beginEgg, contourPoints.begin() + endEgg, (beginEgg + endEgg)/2);
  eggsPos->push_back(egg.center);
  for(auto it = contourSize.begin() + beginEgg; it != contourSize.begin() + endEgg + 1; ++it)
    it->erase(it->begin());
  allPoints.push_back(vector<pair<int,int>>(0));
  for(auto it = contourPoints.begin() + beginEgg; it != contourPoints.begin() + endEgg + 1; ++it)
  {
    allPoints[pointGroupNumber].push_back(it->front());
    it->erase(it->begin());
  }
  startPosition.push_back(beginEgg);
  pointGroupNumber++;
}
void Process::DrawPoints()
{
  for(auto it = contourPoints.begin() + beginEgg; it != contourPoints.begin() + endEgg; ++it)
  {
    line(newMat, Point(it->begin()->first, distance(contourPoints.begin(), it)), Point(it->begin()->second, distance(contourPoints.begin(), it)), 150, 3);
  }
}
void Process::DrawAllPoints()
{
  for(auto it = allPoints.begin(); it != allPoints.end(); ++it)
  {
    int color = distance(allPoints.begin(), it) * 40;
    if(color > 255) color = 255;
    for(auto it2 = it->begin(); it2 != it->end(); ++it2)
    {
      int i = distance(it->begin(), it2);
      line(newMat, Point(it2->first, i + startPosition[distance(allPoints.begin(), it)]), Point(it2->second, i + startPosition[distance(allPoints.begin(), it)]), color, 1);
    }
  }
  for(vector<Point2d>::iterator it = eggsPos->begin(); it != eggsPos->end(); ++it)
  {
    circle(newMat, Point(it->x, it->y), 2, 255);
  }
}
