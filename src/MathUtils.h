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
#define ABS(Number) (((Number) < 0.0f) ? ((Number) * -1.0f) : ((Number))) // Returns the positive version of a number (EX: -1 -> 1)
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y)) // The larger of the two values
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y)) // The smaller of the two values


/* FUNCTIONS */
// ----- Vector math -----
void RotateVectorAxisX(T3DVec3 *VectorToRotate, float Degrees);
void RotateVectorAxisY(T3DVec3 *VectorToRotate, float Degrees);
void RotateVectorAxisZ(T3DVec3 *VectorToRotate, float Degrees);
void RotateVector3ByDegrees(T3DVec3 *Vector3ToRotate, T3DVec3 RotationDegrees);
void MultiplyAxesByFloat(T3DVec3 *VectorToUpdate, T3DVec3 AxesToUpdate, float MultiplyValue);
void AddFloatToAxes(T3DVec3 *VectorToUpdate, T3DVec3 AxesToUpdate, float ValueToAdd);
void OneifyVectorZeroes(T3DVec3 *VectorToUpdate);
T3DVec3 Vec3UnitCirclePointFromAngle(float HorizontalDegrees, float VerticalDegrees, T3DVec3 CenterPoint);
T3DVec3 GetForwardVector(T3DVec3 Position1, T3DVec3 Position2);
T3DVec3 GetRightVector(T3DVec3 ForwardVector);
T3DVec3 GetUpVector(T3DVec3 ForwardVector, T3DVec3 RightVector);

// ----- Matrix math -----
T3DMat4 CreateSRTMatrix(float Position[3], float Rotation[3], float Scale[3]);
void CreateFixedPointMatrix(T3DMat4FP *FPMatrixToUpdate, T3DMat4 MatrixToCreateFrom);
void UpdateTransformMatrix(struct ModelTransform *Transform);

// ----- Range math -----
float ZeroBelowMinimum(float Number, float Minimum);
float ZeroAboveMaximum(float Number, float Maximum);
float UnsignedKeepInRange(float Number, float Minimum, float Maximum);
#endif