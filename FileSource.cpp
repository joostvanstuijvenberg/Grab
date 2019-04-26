/*
* Joost van Stuijvenberg
* Avans Hogeschool Breda
*
* CC BY-SA 4.0, see: https://creativecommons.org/licenses/by-sa/4.0/
* sources & updates: https://github.com/joostvanstuijvenberg/DemoCV
*/

#include "Source.hpp"
#include "opencv2/opencv.hpp"

// ---------------------------------------------------------------------------------------------- *
// Concrete subclass FileSource to support image files                                            *
// ---------------------------------------------------------------------------------------------- *

FileSource::FileSource(std::string filename)
{
	this->filename = filename;
}

cv::Mat FileSource::getImage()
{
	image = cv::imread(filename);
	postProcess(image);
	return image;
}

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

inline CameraSource::CameraSource(int cameraNumber)
{
	camera = cv::VideoCapture(cameraNumber);
	noData = cv::imread("Test.bmp");
	int w = std::max((int)camera.get(CV_CAP_PROP_FRAME_WIDTH), MEDIA_DEFAULT_WIDTH);
	int h = std::max((int)camera.get(CV_CAP_PROP_FRAME_HEIGHT), MEDIA_DEFAULT_HEIGHT);
	cv::resize(noData, noData, cv::Size(w, h));
}

inline cv::Mat CameraSource::getImage()
{
	// Attempt to read an image from the camera. If this fails, the camera may not be available.
	camera >> image;
	if (!camera.isOpened() || !camera.grab() || image.empty())
		image = noData;
	postProcess(image);
	return image;
}

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

inline MovieSource::MovieSource(std::string filename)
{
	movie = cv::VideoCapture(filename);
	//movie.set(CV_CAP_PROP_FOURCC, CV_FOURCC('D', 'I', 'V', '4'));
	//movie.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
	noData = cv::imread("Test.bmp");
	int w = std::max((int)movie.get(CV_CAP_PROP_FRAME_WIDTH), MEDIA_DEFAULT_WIDTH);
	int h = std::max((int)movie.get(CV_CAP_PROP_FRAME_HEIGHT), MEDIA_DEFAULT_HEIGHT);
	cv::resize(noData, noData, cv::Size(w, h));
}

inline cv::Mat MovieSource::getImage()
{
	// Attempt to read an image from the movie file. If this fails, the movie is possibly at its end.
	// Reset the movie position and try again.
	movie >> image;
	if (!movie.isOpened() || !movie.grab() || image.empty())
	{
		movie.set(CV_CAP_PROP_POS_FRAMES, 0);
		movie.set(CV_CAP_PROP_POS_MSEC, 0);
		movie >> image;
	}

	// If the problem persists, return the no-data image.
	if (!movie.isOpened() || !movie.grab() || image.empty())
		return noData;

	// If we reach here, postprocess the image as normal.
	postProcess(image);
	return image;
}
