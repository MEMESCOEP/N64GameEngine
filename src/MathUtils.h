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
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


/* FUNCTIONS */
// ----- Vector math -----
void RotateVectorAxisX(T3DVec3 *VectorToRotate, float Degrees);
void RotateVectorAxisY(T3DVec3 *VectorToRotate, float Degrees);
void RotateVectorAxisZ(T3DVec3 *VectorToRotate, float Degrees);
void RotateVector3ByDegrees(T3DVec3 *Vector3ToRotate, T3DVec3 RotationDegrees);
T3DVec3 Vec3UnitCirclePointFromAngle(float HorizontalDegrees, float VerticalDegrees, T3DVec3 CenterPoint);
T3DVec3 GetCameraForwardVector(T3DVec3 CameraPosition, T3DVec3 CameraTarget);

// ----- Matrix math -----
T3DMat4 CreateSRTMatrix(float Position[3], float Rotation[3], float Scale[3]);
void CreateFixedPointMatrix(T3DMat4FP *FPMatrixToUpdate, T3DMat4 MatrixToCreateFrom);
void UpdateTransformMatrix(struct ModelTransform *Transform);
#endif