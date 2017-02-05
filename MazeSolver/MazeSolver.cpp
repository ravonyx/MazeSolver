#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>

#include <Windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

int video();

void listFolders(std::vector<std::string> &imgNames)
{
	WIN32_FIND_DATA file;
	HANDLE hFind = NULL;
	LARGE_INTEGER filesize;
	TCHAR* filePath = L"img";

	TCHAR path[2048];
	DWORD dwError = 0;
	StringCchCopy(path, MAX_PATH, filePath);
	StringCchCat(path, MAX_PATH, TEXT("\\*"));

	if ((hFind = FindFirstFile(path, &file)) == INVALID_HANDLE_VALUE)
	{
		printf("Path not found: [%s]\n", filePath);
		return;
	}
	do
	{
		if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
		}
		else
		{
			filesize.LowPart = file.nFileSizeLow;
			filesize.HighPart = file.nFileSizeHigh;

			std::wstring ws(file.cFileName);
			std::string str(ws.begin(), ws.end());
			imgNames.push_back(str);
		}
	} while (FindNextFile(hFind, &file) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
		printf("No more files");

	FindClose(hFind);
}

std::string loadMazeImage(std::vector<std::string> &imgNames)
{
	for (int i = 0; i < imgNames.size(); i++)
	{
		std::cout << "(" << i << ") " << imgNames[i] << std::endl;
	}
	std::cout << "Enter nb:";
	int choose;
	std::cin >> choose;

	std::string pathImg = "Img/";
	if (choose < 0)
	{
		std::cout << "Wrong number given !" << std::endl;
		exit(0);
	}
	else if(choose > imgNames.size())
	{
		video();
	}
	else
	{
		std::string name = imgNames[choose];
		pathImg = "Img/";
		pathImg += name;
		std::cout << pathImg << std::endl;
	}
	return pathImg;
}

int video() {
	cv::VideoCapture cap;
	cv::Mat nextInput;

	//si videoname n’est pas null, ouvrir la video dans cap, sinon ouvrir la camera 0
	cap = cv::VideoCapture(0); //Camera frontale par défault de ma machine

	if (!cap.isOpened())
		cap = cv::VideoCapture(0);

	if (!cap.isOpened())  // check if we succeeded
		return -1;

	//recuperer une image depuis cap et la stocker dans nextInput
	cap >> nextInput;
	//tant que nextinput n’est pas vide
	while (!nextInput.empty())
	{
		// - > faire les traitements sur l’image (prochaines étapes)


		// - > appeler la fonction de dessin
		//draw();
		cv::Mat img = nextInput.clone();
		cv::Mat gray;
		cv::cvtColor(img, gray, CV_BGR2GRAY);
		cv::cvtColor(img, nextInput, CV_BGR2GRAY);

		// - > recuperer une nouvelle image et la stocker dans nextInput
		cap >> nextInput;
		// - > attendre 10ms que l’utilisateur tape une touche, et quitter si il le fait
		cv::imshow("input", gray);
		if (cv::waitKey(10) >= 0) break;
	}

	return 0;
	
}

int main()
{

	//video();
	std::vector<std::string> imgNames;
	listFolders(imgNames);
	std::string pathImg = loadMazeImage(imgNames);

	cv::Mat imgMaze;
	imgMaze = cv::imread(pathImg);
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
	else
	{
		cv::Mat draw = cv::Mat::zeros(imgMaze.size(), CV_32FC1);
		cv::drawContours(draw, contours, 0, cv::Scalar(255), -1);
		cv::imshow("Draw contours", draw);

		cv::Mat dilated;
		kernel = cv::Mat::ones(10, 10, CV_8UC1);
		cv::dilate(draw, dilated, kernel, cv::Point(-1, -1), 2, IPL_BORDER_CONSTANT);
		cv::imshow("Draw contours dilate", dilated);

		cv::Mat eroded;
		cv::erode(dilated, eroded, kernel, cv::Point(-1, -1), 2, IPL_BORDER_CONSTANT);
		cv::imshow("Draw contours eroded", eroded);

		cv::Mat diff(imgMaze.size(), CV_8UC1, cv::Scalar(255, 255, 255));
		cv::absdiff(dilated, eroded, diff);
		diff.convertTo(diff, CV_8UC1);

		//set pixels masked by blackWhite to blue
		cv::Mat path(imgMaze.size(), CV_8UC3, cv::Scalar(255, 255, 255));
		path.setTo(cv::Scalar(255, 0, 0), diff);
		path.convertTo(path, CV_8UC1);
		cv::imshow("Path", path);

		cv::Mat finalImg;
		copyImgMaze.copyTo(finalImg, path);
		imshow("Maze Resolved", finalImg);
	}

	

	cv::waitKey();
	return 1;
}
