
#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "common.h"

using namespace std;

struct SGRTTexture
{
	unsigned char *pngData;
	unsigned int width, height;
	bool hasLoadedData;

	SGRTTexture(const char* path) {
		hasLoadedData = false;
		vector<unsigned char> image;
		unsigned error = lodepng::decode(image, width, height, path);
		if(!error) {
			hasLoadedData = true;
			pngData = new unsigned char[width * height * 4];
			copy(image.begin(), image.end(), pngData);
		}
	}

	Vec3fa getColorAt(double u, double v, bool isSmoothTexture) {
		if(!hasLoadedData) {
			return Vec3fa(1.0, 1.0, 1.0);
		}
		while(u < 0.0 || u > 1.0) {
			if(u < 0.0)
				u = -u;
			if(u > 1.0)
				u = 1.0 - u;
		}

		while(v < 0.0 || v > 1.0) {
			if(v < 0.0)
				v = -v;
			if(v > 1.0)
				v = 1.0 - v;
		}

		u = u * width - 0.5;
		v = v * height - 0.5;

		int x = floor(u);
		int y = floor(v);

		if(!isSmoothTexture)
			return readImageColor(x, y);
		else {
			double u_ratio = u - x;
			double v_ratio = v - y;

			double u_opposite = 1 - u_ratio;
			double v_opposite = 1 - v_ratio;

			Vec3fa result = (readImageColor(x, y)   * u_opposite  + readImageColor(x+1, y)   * u_ratio) * v_opposite +
							(readImageColor(x, y+1) * u_opposite  + readImageColor(x+1, y+1) * u_ratio) * v_ratio;
			return result;
		}
	}

	Vec3fa readImageColor(int x, int y) {
		int cSize = 4;
		int index = y * width * cSize + x * cSize;
		double r = 0.0, g = 0.0, b = 0.0;
		if(index > 0 && index + 3 < width * height * 4)
        {
			r = (double)pngData[index + 0] / 255.0;
			g = (double)pngData[index + 1] / 255.0;
			b = (double)pngData[index + 2] / 255.0;
        }
		return Vec3fa(r, g, b);
	}

	double getAlphaAt(double u, double v) {
		if(!hasLoadedData) {
			return 0.0;
		}
		while(u < 0.0 || u > 1.0) {
			if(u < 0.0)
				u = -u;
			if(u > 1.0)
				u = 1.0 - u;
		}

		while(v < 0.0 || v > 1.0) {
			if(v < 0.0)
				v = -v;
			if(v > 1.0)
				v = 1.0 - v;
		}

		u = u * width - 0.5;
		v = v * height - 0.5;

		return readImageAlpha(u, v);
	}

	double readImageAlpha(int x, int y) {
		int cSize = 4;
		int index = y * width * cSize + x * cSize;
		double a = 0.0;
		if(index > 0 && index + 3 < width * height * 4)
			a = (double)pngData[index + 3] / 255.0;
		return a;
	}
};

#endif
