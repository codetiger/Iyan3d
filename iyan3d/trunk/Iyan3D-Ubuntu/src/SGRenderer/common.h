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
int antiAliasingSamples = 0;
int randomSamples = 0;

enum ImageFormat {
    ImageFormat_PPM,
    ImageFormat_PNG
};

struct Vertex   { float x, y, z, a; };
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

RTCRay getOcclusion(RTCScene scene, Vec3fa o, Vec3fa d, double depth = 5000.0f) {
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
    ray.mask = -1;
    ray.time = 0;

    rtcOccluded(scene, ray);
    return ray;
}

Vec3fa getHitPoint(RTCRay ray) {
    return Vec3fa(ray.org[0], ray.org[1], ray.org[2]) + Vec3fa(ray.dir[0], ray.dir[1], ray.dir[2]) * ray.tfar;
}

Vec3fa sampleAroundNormal(Vec3fa n) {
    double r1 = 2 * M_PI * GetRandomValue();
    double r2 = GetRandomValue();
    double r2s = sqrt(r2);

    Vec3fa w = n;
    Vec3fa u = ((fabs(w.x) > .1 ? Vec3fa(0, 1, 0) : Vec3fa(1, 0, 0)).cross(w)).normalize();
    Vec3fa v = w.cross(u);
    return (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalize();
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

#include "texture.h"
#include "camera.h"
#include "material.h"
#include "Mesh.h"
#include "scene.h"

#include "videoencoder.h"

#endif
