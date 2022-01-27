#pragma once
namespace Math
{
	extern float NormalizeYaw(float value);
	extern void VectorAngles(const Vector&vecForward, Vector&vecAngles);
	extern void AngleVectors(const Vector angles, Vector& forward, Vector& right, Vector& up);
	extern void VectorMA(const Vector & start, float scale, const Vector & direction, Vector & dest);
	extern void NormalizeVector(Vector & vecIn);
	extern void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	extern	void VectorTransform(const float * in1, const matrix3x4_t & in2, float * out);
	extern void VectorTransform2(const float *in1, const matrix3x4_t& in2, float *out);
	extern void VectorSubtract(const Vector & a, const Vector & b, Vector & c);
	extern void NormalizeNum(Vector & vIn, Vector & vOut);
	extern void AngleVectors(const QAngle &angles, Vector* forward);
	extern void AngleVectors(const QAngle & angles, Vector & forward);
	extern void ClampAngles(QAngle& angles);
	extern float RandomFloat(float min, float max);
	extern float RandomFloat2(float min, float max);
    extern void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);
    extern void vector_subtract(const Vector& a, const Vector& b, Vector& c);
	extern void vector_transform(const Vector in1, float in2[3][4], Vector &out);
	extern void VectorTransform(Vector& in1, matrix3x4_t& in2, Vector &out);
}

extern bool enabledtp;