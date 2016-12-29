// CT_TP1.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>

int main()
{
	cv::Mat imgMaze;
	imgMaze = cv::imread("img/maze1.png");
	cv::Mat copyImgMaze(imgMaze.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	imgMaze.copyTo(copyImgMaze);
	cv::imshow("Original image", copyImgMaze);

	cv::Mat img;
	cv::cvtColor(imgMaze, img, CV_BGR2GRAY);

	//Seuillage sur image grayscale
	double thresh = 127;
	double maxValue = 255;

	cv::threshold(img, img, 50, maxValue, cv::THRESH_BINARY_INV);

	cv::Mat kernel = cv::Mat::ones(2, 2, CV_8UC1);
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	if (contours.size() != 2)
		std::cout << "There are more than 2 walls";

	cv::Mat draw = cv::Mat::zeros(imgMaze.size(), CV_32FC1);
	cv::drawContours(draw, contours, 0, cv::Scalar(255), -1);
	cv::imshow("Draw contours", draw);

	cv::Mat dilated;
	kernel = cv::Mat::ones(21, 21, CV_8UC1);
	cv::dilate(draw, dilated, kernel, cv::Point(-1, -1), 2, IPL_BORDER_CONSTANT);
	cv::imshow("Draw contours dilate", dilated);

	cv::Mat eroded;
	cv::erode(dilated, eroded, kernel, cv::Point(-1, -1), 2, IPL_BORDER_CONSTANT);
	cv::imshow("Draw contours eroded", eroded);

	cv::Mat diff(imgMaze.size(), CV_8UC1, cv::Scalar(255, 255, 255));
	cv::absdiff(dilated, eroded, diff);
	diff.convertTo(diff, CV_8UC1);

	cv::Mat bgr(imgMaze.size(), CV_8UC3, cv::Scalar(255,255,255));
	//set pixels masked by blackWhite to blue
	bgr.setTo(cv::Scalar(255, 0, 0), diff);
	bgr.convertTo(bgr, CV_8UC1);
	cv::imshow("Path", bgr);

	//cv::Mat result(imgMaze.size(), CV_8UC3, cv::Scalar(255, 255, 255));
	//cv::add(copyImgMaze, diff, result);
	std::vector<cv::Mat> array_to_merge;
	array_to_merge.push_back(copyImgMaze);
	array_to_merge.push_back(bgr);

	cv::Mat color(imgMaze.size(), CV_8UC3);
	cv::merge(array_to_merge, color);
	//cv::imshow("Merged image", array_to_merge);

	cv::waitKey();
	return 1;
}
