#include <process.h>
#include <egg.h>
#include <iostream>
#include <opencv2/opencv.hpp>

double FindDistance(Point2d firstPoint, Point2d secondPoint)
{
  double dx = firstPoint.x - secondPoint.x;
  double dy = firstPoint.y - secondPoint.y;
  return sqrt(dx * dx + dy * dy);
}
int main()
{
  //VideoCapture cap("D:/QtProject/Video/vlc-record-2019-07-16-13h16m37s-rtsp___192.168.11.16_554_Channel10_Sub2-.mp4");
    VideoCapture cap("/dev/sda2/QtProject/Video/vlc-record-2019-07-16-13h09m28s-rtsp___192.168.11.19_554_Channel10_Sub2-.mp4");

  double fps = 25.0;
  Mat frame = Mat::zeros(100, 200, CV_8UC1);

  int thresh1 = 200;
  int egg = 0;
  /*
  circle(frame, Point(50, frame.rows/2), 25, 255, -1);
  circle(frame, Point(85, frame.rows/2 + 30), 25, 255, -1);
  Process proc1(frame);
  proc1.ProcessNew(thresh1, 10);
  imshow("frame", proc1.newMat);
  waitKey();
  */
  int i = 0;
  cap.read(frame);
  Process proc;
  int eggsNum = 0;
  vector<Point2d> eggs;
  vector<Point2d> eggsLast;
  int number = 0;

  //VideoWriter* writer = new VideoWriter("myVideoNew.avi", -1, fps, Size(310, 190), false);
  while(cap.read(frame))
  {
    cvtColor(frame, frame, CV_BGR2GRAY);
    Mat cutFrame(frame, Rect(105, 40, 155, 190));
    Mat f1;
    Mat f2;
    Mat f3;
    i++;
    proc = Process(cutFrame, i, &eggs);
    //proc.GetFrame(cutFrame, i);
    //proc.matOld.copyTo(f1);
    proc.ProcessOld(thresh1);
    //proc.matOld.copyTo(f1);
    proc.newMat.copyTo(f1);
    //proc.ProcessOld(proc.newMat, thresh2).copyTo(f2);
    Mat result = Mat::zeros(cutFrame.rows, 2 * cutFrame.cols, CV_8UC1);
    Mat part11(result, Rect(0,0, cutFrame.cols, cutFrame.rows));
    Mat part12(result, Rect(cutFrame.cols, 0, cutFrame.cols, cutFrame.rows));
    //Mat part13(result, Rect(2*cutFrame.cols, 0, cutFrame.cols, cutFrame.rows));
    //Mat part21(result, Rect(0, cutFrame.rows, cutFrame.cols, cutFrame.rows));
    //Mat part22(result, Rect(cutFrame.cols, cutFrame.rows, cutFrame.cols, cutFrame.rows));
    //Mat part23(result, Rect(2 *cutFrame.cols, cutFrame.rows, cutFrame.cols, cutFrame.rows));
    //cout<<i<<endl;
    cutFrame.copyTo(part11);
    f1.copyTo(part12);
    //f2.copyTo(part22);
    //f3.copyTo(part13);
    //cout<<proc.eggsNum<<endl;
    //egg += proc.eggsNum;
    //if(proc.eggsNum != 0) cout<<egg<<endl;
    //imshow("old", f1);
    //imshow("new", f2);
    vector<double> distance;
    int ch = 0;
    for(int i = 0; i < eggs.size(); i++)
    {
      bool find = false;
      double translateEgg = cutFrame.cols - eggs[i].x;
      for(int j = 0; j < eggsLast.size(); j++)
      {
        double dis = FindDistance(eggs[i],eggsLast[j]);
        if(dis < translateEgg)
        {
          translateEgg = dis;
          number = j;
          find = true;
        }
      }
      distance.push_back(translateEgg);
      if(!find) ch++;
      if(eggsLast.size() > 0 && eggsLast[number].x > cutFrame.cols/2 && eggs[i].x <= cutFrame.cols/2)
        eggsNum++;
    }

    //cout<<eggsNum<<endl;
    cout<<i<<endl;
    waitKey(40);
    imshow("result", result);
    bool pause = false;
    for(int l = 0; l < distance.size(); l++)
    {
      for(int k = 0; k < distance.size(); k++)
      {
        if(distance[l] > 3 * distance[k])
        {
          ch--;
          break;
        }
      }
    }
    eggsLast.assign(eggs.begin(), eggs.end());
    eggs.clear();
    //writer->write(result);
    //waitKey();
  }

  //writer->release();
  return 0;
}
