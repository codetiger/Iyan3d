#ifndef MESH_H_
#define MESH_H_

#include "common.h"

class SGRTMesh
{
public:
	unsigned int id;
	SGRMaterial material;
	
	SGRTTexture* texture;
	Vec3fa* uvs;
	Vec3fa* normals;

	Vec3fa center;
	int numberOfTriangles;

	Vec3fa minPoint, maxPoint;

	SGRTMesh(RTCScene rtcScene, ifstream &data) {
		center = Vec3fa(0.0f);
		minPoint = Vec3fa(999.0f);
		maxPoint = Vec3fa(-999.0f);

		material.emission = readFloat(data);
		material.emissionColor = readVec3fa(data);
		material.shadowDarkness = readFloat(data);

		if (material.emission == 0) {
			material.emissionColor = Vec3fa();
		}

		material.diffuse = readVec3fa(data).normalize();
		material.hasTexture = readBool(data);

		const char *texFile = readString(data);
		if(material.hasTexture) {
			texture = new SGRTTexture(texFile);
		}

		material.reflection = readFloat(data);
		material.refraction = readFloat(data);
		material.transparency = readFloat(data);
		material.hasLighting = readBool(data);

		numberOfTriangles = readInt(data);

		id = rtcNewTriangleMesh(rtcScene, RTC_GEOMETRY_STATIC, numberOfTriangles, numberOfTriangles * 3);
		uvs = (Vec3fa*) malloc(numberOfTriangles*3*sizeof(Vec3fa));
		normals = (Vec3fa*) malloc(numberOfTriangles*3*sizeof(Vec3fa));

		if(material.emission > 0.0)
			rtcSetMask(rtcScene, id, 0x0000FFFF);
		else
			rtcSetMask(rtcScene, id, 0xFFFF0000);

		Vertex* vs = (Vertex*) rtcMapBuffer(rtcScene, id, RTC_VERTEX_BUFFER); 
		if(!vs) {
			printf("Error: Vertex Array Initializing failed %d\n", numberOfTriangles);
		}
		for (unsigned int i = 0; i < numberOfTriangles; i++) {
			for(int j = 0; j < 3; j++) {
				Vec3fa v = readVec3fa(data);
				v.x = -v.x;
				vs[(i*3)+j].x = v.x;
				vs[(i*3)+j].y = v.y;
				vs[(i*3)+j].z = v.z;
				center = center + v;
				updateBoundingBox(v);
			}

			for(int j = 0; j < 3; j++) {
				Vec3fa n = readVec3fa(data);
				n.x = -n.x;
				normals [(i*3)+j].x = n.x;
				normals [(i*3)+j].y = n.y;
				normals [(i*3)+j].z = n.z;
			}

			for(int j = 0; j < 3; j++) {
				uvs[(i*3)+j].x = readFloat(data);
				uvs[(i*3)+j].y = readFloat(data);
				uvs[(i*3)+j].z = 0.0f;
			}
		}

		rtcUnmapBuffer(rtcScene, id, RTC_VERTEX_BUFFER);

		Triangle *triangles = (Triangle*) rtcMapBuffer(rtcScene, id, RTC_INDEX_BUFFER);
		for (unsigned int i = 0; i < numberOfTriangles; i++) {
			triangles[i].v0 = i*3 + 0;
			triangles[i].v1 = i*3 + 1;
			triangles[i].v2 = i*3 + 2;
		}
		rtcUnmapBuffer(rtcScene, id, RTC_INDEX_BUFFER);

		center = center / (double)(numberOfTriangles * 3.0f);
	}

	void updateBoundingBox(Vec3fa v) {
		if(minPoint.x > v.x)
			minPoint.x = v.x;
		if(minPoint.y > v.y)
			minPoint.y = v.y;
		if(minPoint.z > v.z)
			minPoint.z = v.z;

		if(maxPoint.x < v.x)
			maxPoint.x = v.x;
		if(maxPoint.y < v.y)
			maxPoint.y = v.y;
		if(maxPoint.z < v.z)
			maxPoint.z = v.z;
	}

	Vec3fa getRandomPointInMesh() {
		Vec3fa v;
		v.x = center.x + (GetRandomValue()-0.5) * (maxPoint.x - minPoint.x)/2.0;
		v.y = center.y + (GetRandomValue()-0.5) * (maxPoint.y - minPoint.y)/2.0;
		v.z = center.z + (GetRandomValue()-0.5) * (maxPoint.z - minPoint.z)/2.0;

		return v;
	}

	double getRadius() {
		return fabs(maxPoint.distance(minPoint)) / 2.0f;
	}

	Vec3fa getUV(int index) {
		Vec3fa uv;
		uv.x = uv.y = uv.z = 0;

		if(index >= 0 && index < numberOfTriangles * 3)
			uv = uvs[index];

		return uv;
	}

	Vec3fa getNormal(int index) {
		Vec3fa n;
		n.x = n.y = n.z = 0.0;

		if(index >= 0 && index < numberOfTriangles * 3)
			n = normals[index];

		return n;
	}

	Vec3fa getEmissionColor() {
		return material.emissionColor;//* 200.0f;
	}

	Vec3fa getColor(double u, double v) {
		Vec3fa color = Vec3fa(0.0f);
		if(material.hasTexture)
			color = texture->getColorAt(u, v);
		else if(material.emission > 0.0)
			color = material.emissionColor;
		else
			color = material.diffuse;

		return color;
	}

	Vec3fa getInterpolatedUV(int index, double u, double v) {
		Vec3fa uva = getUV(index * 3 + 0);
		Vec3fa uvb = getUV(index * 3 + 1);
		Vec3fa uvc = getUV(index * 3 + 2);

		Vec3fa uv = uva + (uvb - uva) * u + (uvc - uva) * v;
		return uv;
	}

	Vec3fa getInterpolatedNormal(int index, double u, double v) {
		Vec3fa na = getNormal(index * 3 + 0);
		Vec3fa nb = getNormal(index * 3 + 1);
		Vec3fa nc = getNormal(index * 3 + 2);

		return (na + (nb - na) * u + (nc - na) * v);
	}

	~SGRTMesh() {
		if(uvs)
			free(uvs);
		if(normals)
			free(normals);
		if(material.hasTexture)
			delete texture;
	}
};

#endif
