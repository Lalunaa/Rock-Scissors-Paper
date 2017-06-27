#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

const int NUM_SECONDS = 5;

int main(int argc, const char** argv)
{
	double time_counter = 0;  
	clock_t this_time = clock();
	clock_t last_time = this_time;
	int compChoice = (rand() % 3) + 1; 
	int playerChoice;
	int computer = 0;
	int player = 0;
	bool finished = false;
	VideoCapture cam(0); 
	if (!cam.isOpened()) {
		cout << "ERROR not opened " << endl;
		return -1;
	}
	Mat game; 
	Mat flipped_game; 
	Mat game_threshold; 
	Mat game_gray; 
	Mat game_roi;
	Mat rock = imread("rock.JPG", CV_LOAD_IMAGE_UNCHANGED); 
	Mat scissors = imread("scissors.JPG", CV_LOAD_IMAGE_UNCHANGED);
	Mat paper = imread("paper.JPG", CV_LOAD_IMAGE_UNCHANGED);
	Mat playerWin = imread("playerWin.JPG", CV_LOAD_IMAGE_UNCHANGED); //źródło: https://www.colourbox.com/preview/23397043-businessman-holding-winning-trophy.jpg
	Mat computerWin = imread("computerWin.JPG", CV_LOAD_IMAGE_UNCHANGED); //źródło: https://lh3.googleusercontent.com/proxy/wlv1DMMY0YrB76eE4kmFiWYFGUY2jF16pIYGkhUhbYS8Huv6n0rE3doimzwPsRwtbitxKYpzOgIo4KnaWhC3hUz0683hBDr5V9P6mjJSuTMpUz_LsV-p2PQm3rec5e6VyZEgaJM7Vr1MD_EMD24mSA=w530-h354-p-rw
	namedWindow("Game", CV_WINDOW_AUTOSIZE); 
	namedWindow("Computer_choice", CV_WINDOW_AUTOSIZE); 
	char a[40]; 
	int count = 0; 
	while (1) { 
		bool b = cam.read(game);
		if (!b) {
			cout << "ERROR : cannot read" << endl;
			return -1;
		}
		flip(game, flipped_game, 1); 
		Rect rect(50, 100, 270, 270); 
		rectangle(
			flipped_game,
			rect,
			cv::Scalar(255, 255, 255)
		);
		game_roi = flipped_game(rect); 
		cvtColor(game_roi, game_gray, CV_RGB2GRAY); 
		GaussianBlur(game_gray, game_gray, Size(19, 19), 0.0, 0); 
		threshold(game_gray, game_threshold, 0, 255, THRESH_BINARY_INV + THRESH_OTSU); 
		vector<vector<Point> >contours;			//źródło: https://github.com/AmeyCaps/hand-gesture-recognition-opencv/blob/master/main.cpp
		vector<Vec4i>hierarchy;
		findContours(game_threshold, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point());  
		if (contours.size() > 0) {
			size_t indexOfBiggestContour = -1;
			size_t sizeOfBiggestContour = 0;
			for (size_t i = 0; i < contours.size(); i++) {
				if (contours[i].size() > sizeOfBiggestContour) {
					sizeOfBiggestContour = contours[i].size();
					indexOfBiggestContour = i;
				}
			}
			vector<vector<int> >hull(contours.size());		
			vector<vector<Point> >hullPoint(contours.size());
			vector<vector<Vec4i> >defects(contours.size());
			vector<vector<Point> >defectPoint(contours.size());
			vector<vector<Point> >contours_poly(contours.size());
			Point2f rect_point[4];
			vector<RotatedRect>minRect(contours.size());
			vector<Rect> boundRect(contours.size());
			for (size_t i = 0; i < contours.size(); i++) {
				if (contourArea(contours[i]) > 5000) {
					convexHull(contours[i], hull[i], true);
					convexityDefects(contours[i], hull[i], defects[i]);
					if (indexOfBiggestContour == i) {
						minRect[i] = minAreaRect(contours[i]);
						for (size_t k = 0; k < hull[i].size(); k++) {
							int ind = hull[i][k];
							hullPoint[i].push_back(contours[i][ind]);
						}
						count = 0;
						for (size_t k = 0; k < defects[i].size(); k++) { 
							if (defects[i][k][3] > 13 * 256) {
								int p_end = defects[i][k][1];
								int p_far = defects[i][k][2];
								defectPoint[i].push_back(contours[i][p_far]);
								//circle(game_roi, contours[i][p_end], 3, Scalar(0, 255, 0), 2); 
								count++;
							}
						}
						if (count == 0) { 
							strcpy(a, "Wybierasz:  KAMIEN");
							playerChoice = 1;
						}
						else if (count == 1) {
							strcpy(a, "Wybierasz:  KAMIEN");
							playerChoice = 1;
						}
						else if (count == 2) {
							strcpy(a, "Wybierasz:  NOZYCE");
							playerChoice = 2;
						}
						else if (count == 3) {
							strcpy(a, "Wybierasz:  NOZYCE");
							playerChoice = 2;
						}
						else if (count == 4) {
							strcpy(a, "Wybierasz:  PAPIER");
							playerChoice = 3;
						}
						else if (count == 5) {
							strcpy(a, "Wybierasz:  PAPIER");
							playerChoice = 3;
						}
						else {
							strcpy(a, "Wybierasz:  KAMIEN");
							playerChoice = 1;
						}
						putText(flipped_game, a, cvPoint(80, 90), FONT_HERSHEY_PLAIN, 1.2, cvScalar(255, 255, 255), 1, CV_AA); 
					}
				}
			}
			this_time = clock(); 
			time_counter += (double)(this_time - last_time);
			last_time = this_time;
			if (time_counter > (double)(NUM_SECONDS * CLOCKS_PER_SEC))
			{
				time_counter -= (double)(NUM_SECONDS * CLOCKS_PER_SEC);
				compChoice = (rand() % 3) + 1; 
				if (playerChoice == 1) { 
					if (compChoice == 3)
						computer++;
					else if (compChoice == 2)
						player++;
				}
				else if (playerChoice == 2) {
					if (compChoice == 1)
						computer++;
					else if (compChoice == 3)
						player++;
				}
				else if (playerChoice == 3) {
					if (compChoice == 2)
						computer++;
					else if (compChoice == 1)
						player++;
				}
				if (finished == false) { 
					if (compChoice == 1)
						imshow("Computer_choice", rock);
					else if (compChoice == 2)
						imshow("Computer_choice", scissors);
					else if (compChoice == 3)
						imshow("Computer_choice", paper);
				}
			}
			string timer2 = "Runda konczy sie za: "; 
			putText(flipped_game, timer2, cvPoint(350, 50), FONT_HERSHEY_COMPLEX, 0.7, cvScalar(0, 0, 0), 1, CV_AA);
			string timer = to_string(5 - (int)(time_counter / 1000)); 
			putText(flipped_game, timer, cvPoint(450, 140), FONT_HERSHEY_DUPLEX, 3, cvScalar(0, 0, 255), 2, CV_AA);

			string playerScore2 = "Gracz: ";
			putText(flipped_game, playerScore2, cvPoint(450, 350), FONT_HERSHEY_COMPLEX, 0.7, cvScalar(0, 0, 0), 1, CV_AA);
			string playerScore = to_string(player);
			putText(flipped_game, playerScore, cvPoint(530, 350), FONT_HERSHEY_SCRIPT_SIMPLEX, 0.7, cvScalar(255, 255, 255), 2, CV_AA);

			string computerScore2 = "Komputer: ";
			putText(flipped_game, computerScore2, cvPoint(430, 300), FONT_HERSHEY_COMPLEX, 0.7, cvScalar(0, 0, 0), 1, CV_AA);
			string computerScore = to_string(computer);
			putText(flipped_game, computerScore, cvPoint(565, 300), FONT_HERSHEY_SCRIPT_SIMPLEX, 0.7, cvScalar(255, 255, 255), 2, CV_AA);
			
			
			if (finished == false) 
				imshow("Game", flipped_game); 

			if (player >= 5) {			
				finished = true;				
				cvDestroyWindow("Computer_choice");
				cvDestroyWindow("Game");
				namedWindow("Score", CV_WINDOW_AUTOSIZE);
				imshow("Score", playerWin);
			}
			else if (computer >= 5) {
				finished = true;				
				cvDestroyWindow("Computer_choice");
				cvDestroyWindow("Game");
				namedWindow("Score", CV_WINDOW_AUTOSIZE);
				imshow("Score", computerWin);
			}
			if (waitKey(10) == 27) break;
		}
	}
	return 0;
}