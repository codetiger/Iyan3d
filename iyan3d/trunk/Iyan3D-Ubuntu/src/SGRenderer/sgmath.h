
#include <math.h>
#include <vector>


struct Vec3fa
{
	double x, y, z;

	Vec3fa()
	{
        x = y = z = 0.0f;
	}

	Vec3fa(double a, double b, double c)
	{
		x = a; y = b; z = c;
	}

	Vec3fa(double a)
	{
		x = y = z = a;
	}

	Vec3fa normalize() 
	{ 
		Vec3fa a = Vec3fa(x, y, z);
		const double d = a.dot(a);

		if (d != 0.0f)
			a = a / sqrt(d);

		x = a.x;
		y = a.y;
		z = a.z;
        
		return Vec3fa(x, y, z);
	}

    double dot(const Vec3fa &b) const 
    {
        return x*b.x + y*b.y + z*b.z;
    }

    double distance(const Vec3fa &b) const 
    {
        return sqrt((x-b.x)*(x-b.x) + (y-b.y)*(y-b.y) + (z-b.z)*(z-b.z));
    }

    Vec3fa cross(const Vec3fa &b) const 
    {
        return Vec3fa(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
    }

    Vec3fa operator+(const Vec3fa &b) const 
    {
        return Vec3fa(x+b.x, y+b.y, z+b.z);
    }
    
    Vec3fa operator-(const Vec3fa &b) const 
    {
        return Vec3fa(x-b.x, y-b.y, z-b.z);
    }
    
    Vec3fa operator*(double b) const 
    {
        return Vec3fa(x*b, y*b, z*b);
    }

    Vec3fa operator/(double b) const 
    {
        return Vec3fa(x/b, y/b, z/b);
    }
    
    Vec3fa operator*(const Vec3fa &b) const 
    {
        return Vec3fa(x*b.x, y*b.y, z*b.z);
    }

    double& operator [] ( const size_t index )
    { 
    	return (&x)[index];
    }

    Vec3fa neg() const
    {
    	return (Vec3fa(x,y,z)) * (-1.0f);
    }

    double& Get(int i)
    {
        if(i == 0) return x;
        if(i == 1) return y;
        if(i == 2) return z;
    }

    Vec3fa getDirection()
    {
        double d2r = M_PI / 180.0f;
        Vec3fa dir;
        dir.x = cos(x * d2r) * sin(y * d2r);
        dir.y = sin(x * d2r) * sin(y * d2r);
        dir.z =                cos(y * d2r);
        return dir;
    }
};

Vec3fa face_forward(const Vec3fa& N, const Vec3fa& I, const Vec3fa& _Ng) 
{
	const Vec3fa Ng = _Ng;
	return I.dot(Ng) < 0.0f ? N : N.neg();
}

double getAngle(Vec3fa a, Vec3fa b) {
    double dotValue = a.dot(b);
    double modA = sqrtf(a.dot(a));
    double modB = sqrtf(b.dot(b));
    return (dotValue/(modA * modB));
}
