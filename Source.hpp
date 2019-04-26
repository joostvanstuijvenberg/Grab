/*
* Joost van Stuijvenberg
* Avans Hogeschool Breda
*
* CC BY-SA 4.0, see: https://creativecommons.org/licenses/by-sa/4.0/
* sources & updates: https://github.com/joostvanstuijvenberg/DemoCV
*/

#ifndef SOURCE_H
#define SOURCE_H

#include "opencv2/opencv.hpp"

#define SIZE_FACTOR_MIN			0.2
#define SIZE_FACTOR_MAX			2.0
#define SIZE_FACTOR_STEP		0.1
#define SIZE_FACTOR_NORMAL		1.0

#define MEDIA_DEFAULT_WIDTH		640
#define MEDIA_DEFAULT_HEIGHT	480

// ---------------------------------------------------------------------------------------------- *
// Abstract superclass Source                                                                     *
// ---------------------------------------------------------------------------------------------- *

class Source
{
public:
	void increaseSize();
	void decreaseSize();
	void normalSize();
	void toggleFlipHorizontal();
	void toggleFlipVertical();
	virtual cv::Mat getImage() = 0;
protected:
	cv::Mat image;
	cv::Mat noData;
	void postProcess(cv::Mat& image);
private:
	double sizeFactor = SIZE_FACTOR_NORMAL;
	bool flipH = false;
	bool flipV = false;
};

// ---------------------------------------------------------------------------------------------- *
// Concrete subclass FileSource to support image files                                            *
// ---------------------------------------------------------------------------------------------- *

class FileSource : public Source
{
public:
	FileSource(std::string filename);
	cv::Mat getImage() override;
private:
	std::string filename;
};

// ---------------------------------------------------------------------------------------------- *
// Concrete subclass CameraSource to support video                                                *
// ---------------------------------------------------------------------------------------------- *

class CameraSource : public Source
{
public:
	CameraSource(int cameraNumber);
	cv::Mat getImage() override;
private:
	cv::VideoCapture camera;
};

// ---------------------------------------------------------------------------------------------- *
// Concrete subclass MovieSource to support movie files                                           *
// ---------------------------------------------------------------------------------------------- *

class MovieSource : public Source
{
public:
	MovieSource(std::string filename);
	cv::Mat getImage() override;
private:
	cv::VideoCapture movie;
};

#endif