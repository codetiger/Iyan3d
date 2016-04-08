#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <xmmintrin.h>
#include <pmmintrin.h>
#include <sys/stat.h>
#include <sstream>
#include <algorithm>
#include <locale>

#include <curl/curl.h>
#include <zip.h>

#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>

#include "server.h"
#include "sgmath.h"
#include "lodepng.h"
#include "Mat4.h"
#include "tile.h"
#include "threadpool.h"

using namespace std;

int TILE_SIZE = 64;

bool isRenderMachine = false;
int taskFetchFrequency = 2;

int MAX_RAY_DEPTH = 6;
int samplesAO = 0;
double minAOBrightness = 0.5f;
int randomSamples = 0;

long debug_ray_intersections = 0;
long debug_ray_occlusions = 0;

bool runInDeveloperMode = false;

enum ImageFormat {
    ImageFormat_PPM,
    ImageFormat_PNG
};

#define ALIGN(...) __attribute__((aligned(__VA_ARGS__)))

ALIGN(16) struct Vertex   { float x, y, z, a; };
struct Triangle { unsigned int v0, v1, v2; };
struct UV { float u, v; };
struct Normal { float x, y, z; };

struct TaskDetails {
    int taskId;
    int width;
    int height;
    int frame;
    int startFrame;
    int endFrame;
    bool isRenderTask;
};

float readFloat(ifstream &dfile) {
    float dval = 0;
    dfile.read((char*)&dval, sizeof(float));
    return dval;
}

Vec3fa readVec3fa(ifstream &dfile) {
    Vec3fa v;
    v.x = (float)readFloat(dfile);
    v.y = (float)readFloat(dfile);
    v.z = (float)readFloat(dfile);
    return v;
}

int readInt(ifstream &dfile) {
    int dval = 0;
    dfile.read((char*)&dval, sizeof(dval));
    return dval;
}

short readShort(ifstream &dfile) {
    short dval = 0;
    dfile.read((char*)&dval, sizeof(dval));
    return dval;
}

bool readBool(ifstream &dfile) {
    bool dval = 0;
    dfile.read((char*)&dval, sizeof(dval));
    return dval;
}

char* readString(ifstream &dfile) {
    int length = readInt(dfile);
    char *dval = new char[length+1];
    dfile.read(dval, length);
    dval[length] = '\0';
    return dval;
}

double clip(double n, double lower, double upper) {
    return std::max(lower, std::min(n, upper));
}

Vec3fa clip(Vec3fa n, double lower, double upper) {
    return Vec3fa(clip(n.x, lower, upper), clip(n.y, lower, upper), clip(n.z, lower, upper));
}

void mkpath(string s, mode_t mode) {
    char cCurrentPath[FILENAME_MAX];
    getcwd(cCurrentPath, sizeof(cCurrentPath));
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';

    string path = cCurrentPath;
    path = path + "/" + s;
    mkdir(path.c_str(), 0755);
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

int num_cores() {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

double GetRandomValue() {
    return (double)rand() / RAND_MAX;
}

RTCRay getIntersection(RTCScene scene, Vec3fa o, Vec3fa d, double depth = 5000.0f, int mask = 0xFFFFFFFF) {
	debug_ray_intersections++;
    RTCRay ray;
    ray.org[0] = o.x;
    ray.org[1] = o.y;
    ray.org[2] = o.z;

    ray.dir[0] = d.x;
    ray.dir[1] = d.y;
    ray.dir[2] = d.z;

    ray.tnear = 0.001f;
    ray.tfar = depth;
    ray.geomID = RTC_INVALID_GEOMETRY_ID;
    ray.primID = RTC_INVALID_GEOMETRY_ID;
    ray.mask = mask;
    ray.time = 0;

    rtcIntersect(scene, ray);
    return ray;
}

RTCRay getOcclusion(RTCScene scene, Vec3fa o, Vec3fa d, double depth = 5000.0f, int mask = 0xFFFFFFFF) {
	debug_ray_occlusions++;
    RTCRay ray;
    ray.org[0] = o.x;
    ray.org[1] = o.y;
    ray.org[2] = o.z;

    ray.dir[0] = d.x;
    ray.dir[1] = d.y;
    ray.dir[2] = d.z;

    ray.tnear = 0.001f;
    ray.tfar = depth;
    ray.geomID = RTC_INVALID_GEOMETRY_ID;
    ray.primID = RTC_INVALID_GEOMETRY_ID;
    ray.mask = mask;
    ray.time = 0;

    rtcOccluded(scene, ray);
    return ray;
}

Vec3fa getHitPoint(RTCRay ray) {
    return Vec3fa(ray.org[0], ray.org[1], ray.org[2]) + Vec3fa(ray.dir[0], ray.dir[1], ray.dir[2]) * ray.tfar;
}

#define MPI 3.14159265
#define M2PI 6.28318531

float fastSin(float x) {
	if (x < -MPI)
	    x += M2PI;
	else if (x >  MPI)
	    x -= M2PI;

	float sin = 0;
	if (x < 0)
	    sin = 1.27323954 * x + .405284735 * x * x;
	else
	    sin = 1.27323954 * x - 0.405284735 * x * x;
	return sin;
}

float fastCos(float x) {
	x += 1.57079632;
	if (x >  MPI)
	    x -= M2PI;

	float cos = 0;
	if (x < 0)
		cos = 1.27323954 * x + .405284735 * x * x;
	else
		cos = 1.27323954 * x - 0.405284735 * x * x;
	return cos;
}

Vec3fa cosineSampleAroundNormal(double r1, double r2, Vec3fa n) {
	double phi = 2 * M_PI * r1;
	double cosTheta = sqrt(1.0 - r2);
	double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	Vec3fa d = Vec3fa(fastCos(phi) * sinTheta, fastSin(phi) * sinTheta, cosTheta);

    Vec3fa w = n;
    Vec3fa u = ((fabs(w.x) > .1 ? Vec3fa(0, 1, 0) : Vec3fa(1, 0, 0)).cross(w)).normalize();
    Vec3fa v = w.cross(u);
    return (u * d.x + v * d.y + w * d.z).normalize();
}

Vec3fa sampleAroundNormal(Vec3fa n) {
	return cosineSampleAroundNormal(GetRandomValue(), GetRandomValue(), n);
}

Vec3fa randomizeDirection(Vec3fa dir, double magnitude) {
	SGRTMat4 camMatrix;
    Vec3fa angle = Vec3fa(GetRandomValue() - 0.5, GetRandomValue() - 0.5, 0.0f) * magnitude * 60.0;
    camMatrix.setRotationRadians(angle * M_PI / 180.0f);

    Vec3fa d = dir;
    camMatrix.rotateVect(d);
    return d;
}

bool file_exists (const std::string& name) {
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

bool file_copy (const std::string& source, const std::string& dest) {
	std::ifstream  src(source, std::ios::binary);
	if(src) {
		std::ofstream  dst(dest,   std::ios::binary);
		if(dst) {
			dst << src.rdbuf();
		} else {
			return false;
		}
	} else {
		return false;
	}

	return true;
}

string convert2String(int i) {
	string r = to_string(i);
	while(r.size() < 4) {
		r = "0" + r;
	}
	return r;
}

void gaussionBlur (double *sourceData, double *destData, int width, int height, int radius) {
    double rs = ceil(radius * 2.57);
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            double val = 0, wsum = 0;
            for(int iy = i - rs; iy < i + rs + 1; iy++) {
                for(int ix = j - rs; ix < j + rs + 1; ix++) {
                    int x = min(width - 1, max(0, ix));
                    int y = min(height - 1, max(0, iy));
                    int dsq = (ix - j) * (ix - j) + (iy - i) * (iy - i);
                    double wght = exp( -dsq / (2 * radius * radius) ) / (M_PI * 2 * radius * radius);
                    val += sourceData[y*width+x] * wght;
                    wsum += wght;
                }
            }
            destData[i*width+j] = val/wsum;
        }
    }
}

vector<int> boxesForGauss(double sigma, int n) {
    double wIdeal = sqrt(( 12 * sigma * sigma / n) + 1);  // Ideal averaging filter width
    int wl = floor(wIdeal);
    if(wl % 2 == 0)
    	wl--;

    int wu = wl + 2;

    double mIdeal = (12 * sigma * sigma - n * wl * wl - 4 * n * wl - 3 * n) / (-4 * wl - 4);
    int m = round(mIdeal);

    vector<int> sizes;
    for(int i = 0; i < n; i++)
    	sizes.push_back(i < m ? wl : wu);
    return sizes;
}

void boxBlurH_4 (double *sourceData, double *destData, int width, int height, int radius) {
    double iarr = 1.0 / (radius + radius + 1);
    for(int i=0; i<height; i++) {
        int ti = i * width, li = ti, ri = ti+radius;
        double fv = sourceData[ti], lv = sourceData[ti+width-1], val = (radius+1)*fv;
        for(int j=0; j<radius; j++)
        	val += sourceData[ti+j];

        for(int j=0; j<=radius; j++) {
        	val += sourceData[ri++] - fv;
        	destData[ti++] = round(val*iarr);
        }

        for(int j=radius+1; j<width-radius; j++) {
        	val += sourceData[ri++] - sourceData[li++];
        	destData[ti++] = round(val*iarr);
        }

        for(int j=width-radius; j<width  ; j++) {
        	val += lv - sourceData[li++];
        	destData[ti++] = round(val*iarr);
        }
    }
}

void boxBlurT_4 (double *sourceData, double *destData, int width, int height, int radius) {
    double iarr = 1.0 / (radius + radius + 1);
    for(int i = 0; i < width; i++) {
        int ti = i, li = ti, ri = ti + radius * width;
        double fv = sourceData[ti], lv = sourceData[ti+width*(height-1)], val = (radius+1)*fv;

        for(int j=0; j<radius; j++)
        	val += sourceData[ti+j*width];

        for(int j=0; j<=radius; j++) {
        	val += sourceData[ri] - fv;
        	destData[ti] = round(val*iarr);
        	ri+=width; ti+=width;
        }

        for(int j=radius+1; j<height-radius; j++) {
        	val += sourceData[ri] - sourceData[li];
        	destData[ti] = round(val*iarr);
        	li += width;
        	ri += width;
        	ti += width;
        }

        for(int j=height-radius; j<height; j++) {
        	val += lv - sourceData[li];
        	destData[ti] = round(val*iarr);
        	li+=width;
        	ti+=width;
        }
    }
}

void boxBlur_4 (double *sourceData, double *destData, int width, int height, int radius) {
    for(int i = 0; i < width*height; i++)
    	destData[i] = sourceData[i];

    boxBlurH_4(destData, sourceData, width, height, radius);
    boxBlurT_4(sourceData, destData, width, height, radius);
}

void gaussionBlur_fast(double *sourceData, double *destData, int width, int height, int radius) {
	vector<int> bxs = boxesForGauss(radius, 3);
    boxBlur_4 (sourceData, destData, width, height, (bxs[0]-1)/2);
    boxBlur_4 (destData, sourceData, width, height, (bxs[1]-1)/2);
    boxBlur_4 (sourceData, destData, width, height, (bxs[2]-1)/2);
}

#include "texture.h"
#include "camera.h"
#include "material.h"

void intersectFilterFunction(void* userPtr, RTCRay& ray);
void occludeFilterFunction(void* userPtr, RTCRay& ray);

#include "Mesh.h"

void intersectFilterFunction(void* userPtr, RTCRay& ray) {
	SGRTMesh *mesh = (SGRTMesh*)userPtr;
	Vec3fa uv = mesh->getInterpolatedUV(ray.primID, ray.u, ray.v);
	double alpha = mesh->getAlpha(uv.x, uv.y);
	if(alpha == 0.0)
		ray.geomID = RTC_INVALID_GEOMETRY_ID;
}

void occludeFilterFunction(void* userPtr, RTCRay& ray) {
	SGRTMesh *mesh = (SGRTMesh*)userPtr;
	Vec3fa uv = mesh->getInterpolatedUV(ray.primID, ray.u, ray.v);
	double alpha = mesh->getAlpha(uv.x, uv.y);
	if(alpha < 1.0)
		ray.geomID = RTC_INVALID_GEOMETRY_ID;
}

#include "scene.h"

#include "videoencoder.h"

#endif
