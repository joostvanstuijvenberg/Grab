/*
 * Joost van Stuijvenberg
 * Avans Hogeschool Breda
 *
 * CC BY-SA 4.0, see: https://creativecommons.org/licenses/by-sa/4.0/
 * sources & updates: https://github.com/joostvanstuijvenberg/OpenCV
 *
 * You are free to:
 *    Share � copy and redistribute the material in any medium or format
 *    Adapt � remix, transform, and build upon the material for any purpose, even commercially.
 *
 * The licensor cannot revoke these freedoms as long as you follow the license terms.
 *
 * Under the following terms:
 *    Attribution � You must give appropriate credit, provide a link to the license, and indicate
 *                  if changes were made. You may do so in any reasonable manner, but not in any
 * 	                way that suggests the licensor endorses you or your use.
 *    ShareAlike  � If you remix, transform, or build upon the material, you must distribute your
 *                  contributions under the same license as the original.
 *
 * No additional restrictions � You may not apply legal terms or technological measures that
 * legally restrict others from doing anything the license permits.
 *
 * Notices:
 *    You do not have to comply with the license for elements of the material in the public domain
 *    or where your use is permitted by an applicable exception or limitation. No warranties are
 *    given. The license may not give you all of the permissions necessary for your intended use.
 *    For example, other rights such as publicity, privacy, or moral rights may limit how you use
 *    the material.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <iomanip>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define GRAB_VERSION			"1.1.0"
#define DEFAULT_CAMERA			"0"

std::string dateTimeFileName(std::string extension);
void onMouseClick(int event, int x, int y, int flags, void* userdata);

/*
 * ---------------------------------------------------------------------------------------------- *
 * main()                                                                                         *
 * ---------------------------------------------------------------------------------------------- *
 */
int main(int argc, char** argv)
{
	std::cout << "-------------------------------------------------------------------------------" << std::endl;
	std::cout << "Grab " << GRAB_VERSION << " - Image and video capture utility" << std::endl;
	std::cout << "Joost van Stuijvenberg" << std::endl;
	std::cout << "Avans Hogeschool Breda" << std::endl;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;

	// First optional parameter: camera number (expecting a valid integer).
	std::string camera = argc > 1 ? argv[1] : DEFAULT_CAMERA;

	// See if we can access the camera using the given camera number. A path to a movie file is
	// currently not supported.
	cv::VideoCapture capture(std::stoi(camera));
	if (!capture.isOpened())
	{
		std::cerr << "Could not access the camera. Press Enter to quit." << std::endl;
		std::cin.get();
		return -1;
	}

	std::cout << "Clicking in the camera window will display the selected pixel properties. Press" << std::endl;
	std::cout << "<SPACE> to save a snapshot, <RETURN> to start or stop recording, <ESC> to exit," << std::endl;
	std::cout << "<h> to flip horizontally and <v> to flip vertically. Make sure to press keys" << std::endl;
	std::cout << "while the image window has focus." << std::endl;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;
	std::cout << "Using camera . . . . : " + camera << std::endl;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;

	cv::Mat image, copy;
	cv::namedWindow("Source", CV_WINDOW_AUTOSIZE);
	cv::Size size = cv::Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	//int x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (size.width / 2);
	//int y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (size.height / 2);
	//cv::moveWindow("Source", x, y);
	cv::setMouseCallback("Source", onMouseClick, &image);
	cv::VideoWriter writer;
	std::string fileName;
	bool recording = false, flipH = false, flipV = false;
	char key = 0;
	while (key != 27)
	{
		// Capture an image and flip it horizontally and/or vertically, as requested. If we are
		// recording, write the captured (and possibly flipped) image to the video file.
		capture >> image;
		if (flipH)
			cv::flip(image, image, 1);
		if (flipV)
			cv::flip(image, image, 0);
		if (recording)
			writer << image;

		// Show the image. Display a red dot in the upper left corner if we are recording.
		// Note that this doesn't affect the original image (we show a copy).
		copy = image.clone();
		if (recording)
			cv::circle(copy, cv::Point(20, 20), 10, cv::Scalar(0, 0, 255), -1);
		cv::imshow("Source", copy);
		key = cv::waitKey(40);
		
		// Handle the keys.
		switch (key) {

		// h-key: flip the image horizontally.
		case 'h':
			flipH = !flipH;
			if (flipH)
				std::cout << "Flipping horizontally." << std::endl;
			else
				std::cout << "No longer flipping horizontally." << std::endl;
			break;

		// v-key: flip the image vertically.
		case 'v':
			flipV = !flipV;
			if (flipV)
				std::cout << "Flipping vertically." << std::endl;
			else
				std::cout << "No longer flipping vertically." << std::endl;
			break;

		// Space bar: make a snapshot and store it in the specified output path.
		case 32:
			fileName = dateTimeFileName("bmp");
			cv::imwrite(fileName, image);
			std::cout << "Saved a snapshot as " << fileName << '.' << std::endl;
			break;

		// Return: start or stop recording.
		case 13:
			if (!recording)
			{
				recording = true;
				fileName = dateTimeFileName("avi");
				writer.open(fileName, 0, 25, size);
				if (!writer.isOpened())
				{
					std::cerr << "Could not open the video file for writing. Press Enter to quit." << std::endl;
					std::cin.get();
					return -1;
				}
				std::cout << "Started recording in " << fileName << '.' << std::endl;
			}
			else
			{
				recording = false;
				writer.release();
				std::cout << "Stopped recording in " << fileName << '.' << std::endl;
			}
			break;
		}
	}
}

/*
 * ---------------------------------------------------------------------------------------------- *
 * dateTimeFileName()                                                                             *
 * ---------------------------------------------------------------------------------------------- *
 */
std::string dateTimeFileName(std::string extension)
{
	std::string result;
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	std::ostringstream oss;

	oss << std::put_time(&tm, "%Y%m%d%H%M%S");
	auto dateTime = oss.str();
	result = dateTime + '.' + extension;
	return result;
}

/*
 * ---------------------------------------------------------------------------------------------- *
 * onMouseClick()                                                                                 *
 * ---------------------------------------------------------------------------------------------- *
 */
void onMouseClick(int event, int x, int y, int flags, void* userdata)
{
	std::ostringstream oss;

	if (event == CV_EVENT_LBUTTONDOWN)
	{
		cv::Mat image(*(cv::Mat *)userdata);
		cv::Mat color, gray;
		std::vector<cv::Mat> bgr, hsv;

		cv::split(image, bgr);
		int b = bgr[0].at<uchar>(y, x), g = bgr[1].at<uchar>(y, x), r = bgr[2].at<uchar>(y, x);

		cv::cvtColor(image, color, CV_BGR2HSV);
		cv::split(color, hsv);
		int h = hsv[0].at<uchar>(y, x), s = hsv[1].at<uchar>(y, x), v = hsv[2].at<uchar>(y, x);

		cv::cvtColor(image, gray, CV_BGR2GRAY);
		int gr = gray.at<uchar>(y, x);

		oss << "XY=(" << x << ',' << y << ')';
		oss << ", BGR=(" << b << ',' << g << ',' << r << ')';
		oss << ", HSV=(" << h << ',' << s << ',' << v << ')';
		oss << ", gray=" << gr << '.';
		std::cout << oss.str() << std::endl;
	}
}
