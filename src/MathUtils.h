/* N64 GAME ENGINE */
// Math utilities header
// Written by MEMESCOEP
// November of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


// Define MATHUTILS_H if it hasn't been already
#ifndef MATHUTILS_H
#define MATHUTILS_H


/* LIBRARIES */
#include <t3d/t3d.h>
#include "N64GameEngine.h"


/* DEFINITIONS */
#define SIGN(Number) (((Number) < 0.0f) ? -1.0f : ((Number) > 0.0f)) // The sign of a number (-1, 0, 1)
#define ABS(Number) (((Number) < 0.0f) ? ((Number) * -1.0f) : ((Number))) // Returns the positive version of a number (EX: -1 turns into 1)
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y)) // The larger of two values
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y)) // The smaller of two values


/* FUNCTIONS */
// ----- Vector math -----
void RotateVectorAxisX(T3DVec3* VectorToRotate, float Degrees);
void RotateVectorAxisY(T3DVec3* VectorToRotate, float Degrees);
void RotateVectorAxisZ(T3DVec3* VectorToRotate, float Degrees);
void RotateVector3ByDegrees(T3DVec3* Vector3ToRotate, T3DVec3 RotationDegrees);
void MultiplyAxesByFloat(T3DVec3* VectorToUpdate, T3DVec3 AxesToUpdate, float MultiplyValue);
void AddFloatToAxes(T3DVec3* VectorToUpdate, T3DVec3 AxesToUpdate, float ValueToAdd);
void OneifyVectorZeroes(T3DVec3* VectorToUpdate);
T3DVec3 Vec3UnitCirclePointFromAngle(float HorizontalDegrees, float VerticalDegrees, T3DVec3 CenterPoint);
T3DVec3 GetForwardVector(T3DVec3 Position1, T3DVec3 Position2);
T3DVec3 GetRightVector(T3DVec3 ForwardVector);
T3DVec3 GetUpVector(T3DVec3 ForwardVector, T3DVec3 RightVector);

// ----- Matrix math -----
T3DMat4 CreateSRTMatrix(T3DVec3 Position, T3DVec3 Rotation, T3DVec3 Scale);
void UpdateTransformMatrix(struct ModelTransform* Transform);

// ----- Range math -----
float ZeroBelowMinimum(float Number, float Minimum);
float ZeroAboveMaximum(float Number, float Maximum);
float UnsignedKeepInRange(float Number, float Minimum, float Maximum);

// ----- Array math -----
void ScaleFloat3(float FloatToScale[3], float ScaleValue);

// ----- Lerping -----
uint8_t LerpUint8(int A, int B, float Time);
float LerpFloat(float A, float B, float Time);
void Lerp1DUint8Array(uint8_t ArrayToLerp[], uint8_t InitialArray[], uint8_t TargetArray[], int ArraySize, float Time);
int LerpInt(int A, int B, float Time);
#endif