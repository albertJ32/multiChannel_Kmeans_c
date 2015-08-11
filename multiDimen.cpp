///////////////////////////////////////
// Opencv BoW Texture classification //
///////////////////////////////////////

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
//#include "features2d.hpp" // For feature2d (typedef DescriptorExtractor)
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <string>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm> // Maybe fix DescriptorExtractor doesn't have a member 'create'

#define DICTIONARY_BUILD 1

using std::vector;
using namespace cv;
using namespace std;

typedef vector<Mat> m;
typedef vector<m> mV;

void getNovelImgs(const char *inPath, map<string, vector<Mat> >& novelImgs){
  DIR *pdir = NULL;
  cout << "inpath : " << inPath << endl;
  pdir = opendir(inPath);
  // Check that dir was initialised correctly
  if(pdir == NULL){
    cout << "ERROR! novelImgs: unable to open directory, exiting." << endl;
    exit(1);
  }
  struct dirent *pent = NULL;

  // Continue as long as there are still values in the dir list
  while (pent = readdir(pdir)){
    if(pdir==NULL){
      cout << "ERROR! dir was not initialised correctly, exiting." << endl;
      exit(3);
    }

    // Extract and save img filename without extension
    stringstream ss;
    ss << pent->d_name;
    string fileNme =  ss.str();

    // If not file then continue iteration
    string dot[] = {".", ".."};
    if(fileNme.compare(dot[0]) == 0 || fileNme.compare(dot[1]) == 0){
      continue;
    }
    string cls;
    int lastIdx = fileNme.find_last_of(".");
    int classmk = fileNme.find_last_of("_");
    if(classmk>0){
      cls = fileNme.substr(0, classmk);
    } else{
      cls = fileNme.substr(0, lastIdx);
    }

    ss.str(""); // Clear stringstream

    // Read in image
    ss << inPath;
    ss << pent->d_name;
    string a = ss.str();
    Mat tmp = imread(a, CV_BGR2GRAY);

    if(tmp.data){
      // if(!novelImgs.count(cls))
      //   novelImgs[cls][0].create(tmp.rows, tmp.cols, tmp.type()); // if key not yet created initialise
      novelImgs[cls].push_back(tmp);
      cout << "pushing back: " << cls << endl;
    }else{
      cout << "unable to read image.." << endl;
    }
  }
  closedir(pdir);
  cout << "finished Reading Successfully.." << endl;
}

void listDir(const char *inPath, vector<m >& dirFiles){
  DIR *pdir = NULL;
  cout << "inpath : " << inPath << endl;
  pdir = opendir(inPath);
  // Check that dir was initialised correctly
  if(pdir == NULL){
    cout << "ERROR! unable to open directory, exiting." << endl;
    exit(1);
  }
  struct dirent *pent = NULL;

  // Continue as long as there are still values in the dir list

  m local;
  while (pent = readdir(pdir)){
    if(pdir==NULL){
      cout << "ERROR! dir was not initialised correctly, exiting." << endl;
      exit(3);
    }
    stringstream ss;
    ss << inPath;
    ss << pent->d_name;
    string a = ss.str();
    Mat tmp = imread(a, CV_BGR2GRAY);
    if(tmp.data){
      local.push_back(tmp);
    }else{
      cout << "unable to read image.." << endl;
    }
  }
  dirFiles.push_back(local);
  closedir(pdir);
  cout << "finished Reading Successfully.." << endl;
}


void importImgs(mV &modelImg, vector<string> classes){
  string basePath = "../../../TEST_IMAGES/kth-tips/";

  int count = 0;
  for(int i=0;i<classes.size();i++){
    stringstream ss;
    ss << basePath;
    ss << classes[count];
    ss << "/train/";
    string b = ss.str();
    const char* a = b.c_str();
    cout << "this is the path.. " << a << endl;

    cout << "number: " << i << endl;
    listDir(a, modelImg);

    count ++;
  }
  cout << "This is the size of bread: " << modelImg[0].size() << ", cotton: " << modelImg[1].size() << ", cork: " << modelImg[2].size() << endl;
}

// int main(int argc, char** argv){
  //   if(argc<3){
  //     cout << "not enough inputs entered, exiting." << endl;
  //     exit(1);
  //   }
  //   Mat img1 = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  //   Mat img2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
  //   if(img1.empty() || img2.empty())
  //   {
  //       printf("Can't read one of the images\n");
  //       return -1;
  //   }
  //
  //   // detecting keypoints
  //   SurfFeatureDetector detector(400);
  //   vector<KeyPoint> keypoints1, keypoints2;
  //   detector.detect(img1, keypoints1);
  //   detector.detect(img2, keypoints2);
  //
  //   // computing descriptors
  //   SurfDescriptorExtractor extractor;
  //   Mat descriptors1, descriptors2;
  //   extractor.compute(img1, keypoints1, descriptors1);
  //   extractor.compute(img2, keypoints2, descriptors2);
  //
  //   // matching descriptors
  //   BruteForceMatcher<L2<float> > matcher;
  //   vector<DMatch> matches;
  //   matcher.match(descriptors1, descriptors2, matches);
  // drawing the results
  //  namedWindow("matches", 1);
  //  Mat img_matches;
  //  drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
  //  imshow("matches", img_matches);
  //  waitKey(0);
  //}

Mat reshapeCol(Mat in){
  Mat points(in.rows*in.cols, 1,CV_32F);
  int cnt = 0;
  for(int i =0;i<in.rows;i++){
    for(int j=0;j<in.cols;j++){
      points.at<float>(cnt,0) = in.at<Vec3b>(i,j)[0];
      cnt++;
    }
  }
  return points;
}


int main( int argc, char** argv ){
  cout << "\n\n.......Loading Model Images...... \n" ;

  mV modelImg;
  vector<string> classes = {"bread", "cotton", "cork", "wood"};
  importImgs(modelImg, classes);

  //////////////////////////////
  // Create Texton vocabulary //
  //////////////////////////////
  #if DICTIONARY_BUILD == 1
  cout << "\n\n.......Generating Texton Dictionary...... \n" ;
  int dictSize = 28;
  int attempts = 1;
  int flags = KMEANS_PP_CENTERS;
  TermCriteria tc(TermCriteria::MAX_ITER + TermCriteria::EPS, 10, 0.001);

  BOWKMeansTrainer bowTrainer(dictSize, tc, attempts, flags);

  vector<Mat> txtons;

  listDir("../../../TEST_IMAGES/kth-tips/textons/",txtons);
  SiftDescriptorExtractor detector;

  for(int i=0;i<txtons.size();i++){
    Mat descriptorsTxt;
    vector<KeyPoint> keypointsTxt;
    detector.detect(txtons[i], keypointsTxt);
    detector.compute(txtons[i], keypointsTxt, descriptorsTxt);
    if(!descriptorsTxt.empty())
      bowTrainer.add(descriptorsTxt);
    cout << "loop number: " << i << endl;
  }
  cout << "bowTrainer.discriptorCount: " << bowTrainer.descripotorsCount() << endl;

  Mat dictionary = bowTrainer.cluster();
  cout << "This is the dictionary size: " << dictionary.size() << endl;

  cout << "Saving Dictionary.." << endl;
  FileStorage fs("dictionary.xml",FileStorage::WRITE);
  fs << "vocabulary" << dictionary;
  fs.release();


  #else
  ///////////////////////////////////////////////////////////
  // Get histogram responses using vocabulary from Classes //
  ///////////////////////////////////////////////////////////

  // Load TextonDictionary
    Mat dictionary;
    FileStorage fs("dictionary.xml",FileStorage::READ);
    fs["vocabulary"] >> dictionary;
    fs.release();

  vector<KeyPoint> keypoints;
  Mat response_hist;
  vector<string> class_names;
  Mat img;
  string filepath;
  map<string,Mat> classes_training_data;


  Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);
  Ptr<FeatureDetector> detector(new SiftFeatureDetector());
  Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor);

  BOWImgDescriptorExtractor bowDE(extractor, matcher);
  bowDE.setVocabulary(dictionary);

  cout << "\n\n.......Generating Models...... \n" ;

  for(int i = 0; i < modelImg.size(); i++){
    for(int j = 0; j < modelImg[i].size(); j++){
      Mat bob = imread("../../bread.png", CV_BGR2GRAY);;
      detector->detect(bob, keypoints);

      bowDE.compute(modelImg[i][j], keypoints, response_hist);

      if(!classes_training_data.count(classes[i])){
        cout << "Creating new class.." << endl;
        classes_training_data[classes[i]].create(0,response_hist.cols,response_hist.type());
        class_names.push_back(classes[i]);
      }
      classes_training_data[classes[i]].push_back(response_hist);
    }
  }



  //////////////////////////////////////////
  // Create 1 against all SVM classifiers //
  //////////////////////////////////////////

  cout << "\n\n.......Generating Classifiers...... \n" ;

  map<string,CvSVM> classifiers;

  // Iterate through class models
  for(int i=0;i<classes.size();i++){
    string class_ = classes[i];
    cout << "Currently training class: " << class_ << endl;

    Mat samples(0, response_hist.cols, response_hist.type());
    Mat labels(0, 1, CV_32FC1);

    // Copy class samples and labels
    cout << "adding " << classes_training_data[class_].rows << " positive" << endl;
    samples.push_back(classes_training_data[class_]);

    // Set the label to 1 for positive match
    Mat class_label = Mat::ones(classes_training_data[class_].rows, 1, CV_32FC1);
    labels.push_back(class_label);

    for(map<string,Mat>::iterator it1 = classes_training_data.begin(); it1 != classes_training_data.end(); ++it1) {
      string not_class_ = (*it1).first;
      if(not_class_.compare(class_)==0)
        continue; //skip if not_class == currentclass

      samples.push_back(classes_training_data[not_class_]);

      // Set the label as zero for negative match
      class_label = Mat::zeros(classes_training_data[not_class_].rows, 1, CV_32FC1);
      labels.push_back(class_label);
    }

    cout << "Train.." << endl;
    Mat samples_32f; samples.convertTo(samples_32f, CV_32F);

    if(samples.rows == 0)
      continue; //phantom class?!

    // Train and store classifiers in Map
    classifiers[class_].train(samples_32f, labels);
  }



    //////////////////////////////
    // Test Against Novel Image //
    //////////////////////////////

  cout << "\n\n.......Testing Against Novel Images...... \n" ;

  map<string, map<string, int> > confusionMatrix;

 // Mat novelImage1 = imread(argv[1], CV_BGR2GRAY);
 //
 //  if(!novelImage1.data){
 //    cout << "novelImage unable to be loaded.\nExiting." << endl;
 //    exit(0);
 //  }
  map<string, vector<Mat> > novelImgs;

  getNovelImgs("../../../TEST_IMAGES/kth-tips/NovelTest/", novelImgs);

  double y,n, total;
  // for(int i=0;i<modelImg.size();i++){
  //   for(int j=0;j<modelImg[i].size();j++){
  for(map<string, vector<Mat> >::iterator it = novelImgs.begin(); it != novelImgs.end(); ++it){
   cout << "\nThe class is: " << it->first << endl;
    for(int i=0;i<it->second.size();i++){
      if(it->second[i].rows == 0){
        cout << "ERROR: novelImage map contains blank Mat. Exiting." << endl;
        exit(1);
      };

      Mat responseNovel_hist;
      detector->detect(it->second[i], keypoints);
      bowDE.compute(it->second[i], keypoints, responseNovel_hist);

      float minf = FLT_MAX;
      string min_class;
      for(map<string,CvSVM>::iterator it = classifiers.begin(); it != classifiers.end(); ++it ){
        float res = (*it).second.predict(responseNovel_hist, true);
        if(res<minf){
          minf = res;
          min_class = (*it).first;
        }
      }
      if(min_class == it->first){
        cout << "YES, Predicted: " << min_class << " Actual: " << it->first << endl;
        y++;
      }else {
        cout << "NO, Predicted: " << min_class << " Actual: " << it->first << endl;
        n++;
      }
      total++;
    }
  }

  cout << "\nThe total ratio was:\nCorrect: " << y << "\nIncorrect: " << n << "\n\nPercent correct: " << (y/total)*100 << "\%\n\n";

// //      Add 1 to the class with the closest match
//     confusionMatrix[min_class][classes[i]]++;
  //   }
  // }

  #endif

  return 0;
}
