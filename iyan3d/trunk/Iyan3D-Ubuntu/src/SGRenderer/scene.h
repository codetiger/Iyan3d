#ifndef SCENE_H_
#define SCENE_H_

#include "common.h"

struct Scene
{
	vector<SGRTMesh*> meshes;
	Camera *cam;
	RTCScene sgScene;
	int imgWidth;
	int imgHeight;
	vector<Tile> tiles;
	double progress;
	unsigned char *pixels;
	double *aoMap;
	double dofNear, dofFar;

	Scene(RTCDevice rtcDevice) {
		srand(time(0));
		dofNear = 0.0;
		dofFar = 5000.0;
		sgScene = rtcDeviceNewScene(rtcDevice, RTC_SCENE_STATIC, RTC_INTERSECT1);

		if(!sgScene) {
			printf("Error: Failed initializing RTCScene\n");
		}
	}

	~Scene() {
		delete cam;
		for (int i = 0; i < meshes.size(); i++)
			if(meshes[i])
				delete meshes[i];
		meshes.clear();
		rtcDeleteScene(sgScene);
	}

	class ThreadWorker : public ThreadPoolWorker {
	private:
		int index;
		Scene *scene;
	public:
		ThreadWorker(int i_, Scene* s_) : index(i_), scene(s_) { }

		void operator()() {
			fprintf(stderr, "\rRendering (%d spp) %5.2f%%", randomSamples, 100. * index / (scene->tiles.size() - 1));
			scene->renderTile(scene->tiles[index]);
		}
	};

	void render() {
		progress = 0;
		pixels = (unsigned char*)malloc(imgWidth * imgHeight * 4 * sizeof(unsigned char));
		aoMap = (double*)malloc(imgWidth * imgHeight * sizeof(double));

		for (int y = 0; y < imgHeight; y += TILE_SIZE) {
			for (int x = 0; x < imgWidth; x += TILE_SIZE) {
				double midx = imgWidth/2.0, midy = imgHeight/2.0;

				Tile t;
				t.x = x;
				t.y = y;
				t.width = TILE_SIZE;
				t.height = TILE_SIZE;
				t.distance = sqrt((x-midx)*(x-midx) + (y-midy)*(y-midy));

				if(t.x + t.width > imgWidth)
					t.width = imgWidth - t.x;

				if(t.y + t.height > imgHeight)
					t.height = imgHeight - t.y;

				tiles.push_back(t);
			}
		}
		sort(tiles.begin(), tiles.end(), sortByDisance);
		ThreadPool pool = ThreadPool(num_cores());
		for (int i = 0; i < tiles.size(); ++i) {
			pool.enqueueWork(new ThreadWorker(i, this));
		}
		pool.waitEnd();
		fprintf(stderr, "\n");
	}

	void SaveToFile(const char* imagePath, ImageFormat imgFormat) {
		double* tmpAOMap = (double*)malloc(imgWidth * imgHeight * sizeof(double));
		memcpy(tmpAOMap, aoMap, imgWidth * imgHeight * sizeof(double));
		gaussionBlur(tmpAOMap, aoMap, imgWidth, imgHeight, 3);
		free(tmpAOMap);

		for (size_t y = 0; y < imgHeight; y++) {
			for (size_t x = 0; x < imgWidth; x++) {
				int pi = ((y * imgWidth) + x) * 4;
				double ao = aoMap[((y * imgWidth) + x)];
				pixels[pi + 0] *= ao;
				pixels[pi + 1] *= ao;
				pixels[pi + 2] *= ao;
			}
		}

		if(imgFormat == ImageFormat_PPM) {
			FILE* file = fopen(imagePath, "wb");
			if (!file) 
				printf("cannot open file\n");

			fprintf(file,"P6\n%i %i\n255\n", imgWidth, imgHeight);

			for (size_t y = 0; y < imgHeight; y++) {
				for (size_t x = 0; x < imgWidth; x++) {
					int pi = ((y * imgWidth) + x) * 4;
					fputc((pixels[pi + 0]), file);
					fputc((pixels[pi + 1]), file);
					fputc((pixels[pi + 2]), file);
				}
			}
			fclose(file);
		} else if(imgFormat == ImageFormat_PNG) {
			unsigned error = lodepng_encode32_file(imagePath, pixels, imgWidth, imgHeight);
			if(error) 
				printf("error %u: %s\n", error, lodepng_error_text(error));
		}
		free(pixels);
	}

	void renderTile(Tile t) {
 		for (int y = t.y; y < t.y + t.height; y++)
			for (int x = t.x; x < t.x + t.width; x++)
				renderPixel(x, y);
	}
	
	void renderPixel(int x, int y) {
		Vec3fa color = Vec3fa(0.0f);
		double distance = 0;

		double ao = 1.0;
		if(samplesAO > 0) {
			Vec3fa dir = cam->getRayDirection(x/(double)imgWidth, y/(double)imgHeight);
			ao = getAmbientOcclusion(cam->position, dir, distance);
			aoMap[(((imgHeight - y - 1) * imgWidth) + x)] = ao;
		}

		if(randomSamples > 0) {
			for (int i = 0; i < randomSamples; ++i) {
				double r1 = 2 * GetRandomValue();
				double r2 = 2 * GetRandomValue();
				double dx = r1 < 1 ? sqrt(r1)-1 : 1-sqrt(2-r1);
				double dy = r2 < 1 ? sqrt(r2)-1 : 1-sqrt(2-r2);
				Vec3fa dir = cam->getRayDirection((x + dx)/(double)imgWidth, (y + dy)/(double)imgHeight);

				double focalDist = dofNear + (dofFar - dofNear) / 2.0;
				if(distance > dofNear && distance < dofFar) {
					color = color + getRadiance(cam->position, dir, 0);
				} else {
					Vec3fa focalPoint = cam->position + dir * focalDist;
					double blurMagnitude = (distance > focalDist ? distance - dofFar : dofNear - distance)/ focalDist;
					Vec3fa camPosition = cam->position + Vec3fa(GetRandomValue(), GetRandomValue(), GetRandomValue()) * blurMagnitude;
					color = color + getRadiance(camPosition, focalPoint - camPosition, 0);
				}
			}
			color = color / (double)randomSamples;
		} else {
			Vec3fa dir = cam->getRayDirection(x/(double)imgWidth, y/(double)imgHeight);
			color = color + getRadiance(cam->position, dir, 0);
		}

		int pi = (((imgHeight - y - 1) * imgWidth) + x) * 4;
		pixels[pi + 0] = 255.0f * clip(color.x, 0.0f, 1.0f);
		pixels[pi + 1] = 255.0f * clip(color.y, 0.0f, 1.0f);
		pixels[pi + 2] = 255.0f * clip(color.z, 0.0f, 1.0f);
		pixels[pi + 3] = 255;

		progress = (y * imgWidth + x) / (1.0f * imgWidth * imgHeight);
	}

	double getAmbientOcclusion(Vec3fa point, Vec3fa dir, double &distance) {
		RTCRay ray = getIntersection(sgScene, point, dir, 5000.0, 0xFFFF0000);
		distance = ray.tfar;

		if (ray.geomID != RTC_INVALID_GEOMETRY_ID && (int)ray.geomID < (int)meshes.size()) {
			Vec3fa n = meshes[ray.geomID]->getInterpolatedNormal(ray.primID, ray.u, ray.v);
			n = n.normalize();
			Vec3fa hitPoint = getHitPoint(ray);
			if(!meshes[ray.geomID]->material.hasLighting)
				return 1.0;

			int hitCount = 0;
			for (int i = 0; i < samplesAO; ++i) {
				Vec3fa nd = sampleAroundNormal(n);
				RTCRay ray = getOcclusion(sgScene, hitPoint, nd, 7.5f);
				if(ray.geomID == 0)
					hitCount++;
			}

			double ambience = 1.0f - (hitCount/(double)samplesAO);
			return (minAOBrightness + ambience * (1.0f - minAOBrightness));
		}

		return 0;
	}

	Vec3fa getRadiance(Vec3fa point, Vec3fa dir, int depth, int E = 1) {
		RTCRay ray = getIntersection(sgScene, point, dir, 5000.0f, 0xFFFF0000);
		Vec3fa color = Vec3fa(0.0f);

		if (ray.geomID != RTC_INVALID_GEOMETRY_ID && (int)ray.geomID < (int)meshes.size()) {
			Vec3fa uv = meshes[ray.geomID]->getInterpolatedUV(ray.primID, ray.u, ray.v);
			Vec3fa faceColor = meshes[ray.geomID]->getColor(uv.x, uv.y);
			color = faceColor;

			Vec3fa backColor = Vec3fa(0.0);
			double alpha = meshes[ray.geomID]->getAlpha(uv.x, uv.y);
			if(alpha < 1.0) {
				backColor = getRadiance(getHitPoint(ray), dir, depth);
				faceColor = faceColor * alpha + backColor * (1.0 - alpha);
			}

			if(++depth > MAX_RAY_DEPTH)
				return meshes[ray.geomID]->material.diffuse;

			if(!meshes[ray.geomID]->material.hasLighting) {
				if(alpha < 1.0) {
					return (meshes[ray.geomID]->material.diffuse * alpha + backColor) * E;
				} else {
					return faceColor;
				}
			}

			double refraction = meshes[ray.geomID]->material.refraction;
			double reflection = meshes[ray.geomID]->material.reflection;

			double p = (faceColor.x > faceColor.y && faceColor.x > faceColor.z) ? faceColor.x : (faceColor.y > faceColor.z ? faceColor.y : faceColor.z);
			if(reflection > 0.0 || refraction > 0.0)
				p = 1.0;

			if(!p && GetRandomValue() < p)
				faceColor = faceColor * (1.0 / p);

			Vec3fa n = meshes[ray.geomID]->getInterpolatedNormal(ray.primID, ray.u, ray.v);
			n = n.normalize();

			if(meshes[ray.geomID]->material.emission == 0.0 && reflection != 1.0 && refraction != 1.0) {
				Vec3fa nl = n.dot(dir) < 0.0 ? n : n * -1.0;
				Vec3fa nd = sampleAroundNormal(nl);

				Vec3fa recursiveRadiance = Vec3fa(0.0f);
				if(depth <= 3 || reflection > 0 || refraction > 0)
					recursiveRadiance = getRadiance(point, nd, depth+1, 0);

				Vec3fa lightsContrib = Vec3fa(0.0f);
				lightsContrib = getLightContribution(ray, nl);

				color = meshes[ray.geomID]->getEmissionColor() * E + faceColor * (lightsContrib + recursiveRadiance);
			}

			if(reflection > 0.0) {
				color = meshes[ray.geomID]->getEmissionColor() + (color * (1.0 - reflection) + getReflection(getHitPoint(ray), n, dir, meshes[ray.geomID]->material.reflectionSharpness, depth) * reflection);
			}

			if(refraction > 0.0) {
				color = meshes[ray.geomID]->getEmissionColor() + (color * (1.0 - refraction) + getRefraction(getHitPoint(ray), n, dir, meshes[ray.geomID]->material.reflectionSharpness, depth) * refraction);
			}
		}
		return color;
	}

	Vec3fa getLightContribution(RTCRay preRay, Vec3fa normal) {
		Vec3fa point = getHitPoint(preRay);
		Vec3fa lightsContrib = Vec3fa(0.0);

		for (int i = 0; i < meshes.size(); i++) {
			if(meshes[i]->material.emission > 0.0) {
				if(meshes[i]->material.lightType == 0) {
					Vec3fa lightPoint = meshes[i]->getRandomPointInMesh();
					Vec3fa rayDir = (lightPoint - point).normalize();
					double dist = lightPoint.distance(point);
					RTCRay ray = getOcclusion(sgScene, point, rayDir, dist, 0xFFFF0000);

					double distanceEffect = 1.0 - ray.tfar / (meshes[i]->material.emission * 999.0);

					if(ray.geomID != 0)
						lightsContrib = lightsContrib + meshes[i]->getEmissionColor() * rayDir.dot(normal) * distanceEffect;
				} else if(meshes[i]->material.lightType == 1) {
					Vec3fa rayDir = ((meshes[i]->material.lightDirection * 5000.0) - point).normalize();
					RTCRay ray = getOcclusion(sgScene, point, rayDir, 5000, 0xFFFF0000);
					if(ray.geomID != 0)
						lightsContrib = lightsContrib + meshes[i]->getEmissionColor() * rayDir.dot(normal);
				}
			}
		}

		return lightsContrib;
	}

	Vec3fa getReflection(Vec3fa point, Vec3fa normal, Vec3fa dir, double reflectionSharpness, int depth) {
		Vec3fa rayDir = dir - normal * 2.0f * normal.dot(dir);
		rayDir = randomizeDirection(rayDir, (1.0 - reflectionSharpness));
		Vec3fa reflectionColor = Vec3fa(0.0f);

		reflectionColor = getRadiance(point, rayDir, depth);
	    return reflectionColor;
	}

	Vec3fa getRefraction(Vec3fa point, Vec3fa normal, Vec3fa dir, double reflectionSharpnes, int depth) {
		Vec3fa nl = normal.dot(dir) < 0 ? normal : normal * -1.0;
		double into = normal.dot(nl);

		double refractiveIndexAir = 1;
		double refractiveIndexGlass = 1.5;
		double refractiveIndexRatio = into? refractiveIndexAir / refractiveIndexGlass : refractiveIndexGlass / refractiveIndexAir;
		double cosI = dir.dot(nl);
		double cos2t = 1 - refractiveIndexRatio * refractiveIndexRatio * (1 - cosI * cosI);

		if (cos2t < 0)
			return getReflection(point, normal, dir, reflectionSharpnes, depth);

		Vec3fa refractedDirection = dir * refractiveIndexRatio - normal * (into ? 1 : -1) * (cosI * refractiveIndexRatio + sqrt(cos2t));
		refractedDirection = refractedDirection.normalize();

		double a = refractiveIndexGlass - refractiveIndexAir;
		double b = refractiveIndexGlass + refractiveIndexAir;
		double R0 = a * a / (b * b);
		double c = 1 - (into ? -cosI : refractedDirection.dot(normal));
		double Re = R0 + (1 - R0) * c * c * c * c * c;
		double Tr = 1 - Re;
		double P =.25 + .5 * Re;
		double RP = Re / P;
		double TP = Tr / (1 - P);

//		Vec3fa reflectionColor = getReflection(point, normal, dir, reflectionSharpnes, depth);
		Vec3fa refractionColor = getRadiance(point, refractedDirection, depth);

//		if(depth > 2) {
//			if(GetRandomValue() < P)
//				return reflectionColor * RP;
//			else
//				return refractionColor * TP;
//		} else {
//			return reflectionColor * Re + refractionColor * Tr;
//		}

		return refractionColor * Tr;
	}

	bool loadScene(const char* fileName, int width, int height) {
		imgWidth = width;
		imgHeight = height;

		ifstream data(fileName, ios::binary);

		Vec3fa cpos = readVec3fa(data);
		cpos.x = -cpos.x;
		Vec3fa cDir = (readVec3fa(data) - cpos).normalize();
		Vec3fa camRot = readVec3fa(data);

		double fov = readFloat(data);
		// fov = 360;
		cam = new Camera(cpos, camRot, cDir, fov, width, height);
		int nodeCount = readShort(data);

		for (int i = 0; i < nodeCount; i++) {
			SGRTMesh* m = new SGRTMesh(sgScene, data);
			meshes.push_back(m);
		}
		rtcCommit(sgScene);
		data.close();
	}

};

#endif
