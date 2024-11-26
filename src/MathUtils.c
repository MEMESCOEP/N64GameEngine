/* N64 GAME ENGINE */
// Math utilities file
// Written by MEMESCOEP
// November of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


/* LIBRARIES */
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include "N64GameEngine.h"


/* FUNCTIONS */
// ----- Vector math -----
// Rotate a vector3 by x degrees around the X axis
void RotateVectorAxisX(T3DVec3 *VectorToRotate, float Degrees)
{
    float Radians = T3D_DEG_TO_RAD(Degrees);

    VectorToRotate->v[1] += VectorToRotate->v[1] * cos(Radians) - VectorToRotate->v[2] * sin(Radians);
    VectorToRotate->v[2] += VectorToRotate->v[1] * sin(Radians) + VectorToRotate->v[2] * cos(Radians);
}

// Rotate a vector3 by x degrees around the Y axis
void RotateVectorAxisY(T3DVec3 *VectorToRotate, float Degrees)
{
    float Radians = T3D_DEG_TO_RAD(Degrees);

    VectorToRotate->v[0] += VectorToRotate->v[0] * cos(Radians) + VectorToRotate->v[2] * sin(Radians);
    VectorToRotate->v[2] += -VectorToRotate->v[0] * sin(Radians) + VectorToRotate->v[2] * cos(Radians);
}

// Rotate a vector3 by x degrees around the Z axis
void RotateVectorAxisZ(T3DVec3 *VectorToRotate, float Degrees)
{
    float Radians = T3D_DEG_TO_RAD(Degrees);

    VectorToRotate->v[0] += VectorToRotate->v[0] * cos(Radians) - VectorToRotate->v[1] * sin(Radians);
    VectorToRotate->v[1] += VectorToRotate->v[0] * sin(Radians) + VectorToRotate->v[1] * cos(Radians);
}

// Rotate a vector3 by x degrees
void RotateVector3ByDegrees(T3DVec3 *Vector3ToRotate, T3DVec3 RotationDegrees)
{
    RotateVectorAxisX(Vector3ToRotate, RotationDegrees.v[0]);
    RotateVectorAxisY(Vector3ToRotate, RotationDegrees.v[1]);
    RotateVectorAxisZ(Vector3ToRotate, RotationDegrees.v[2]);
}

// Get a point on a unit circle using a position and an angle
T3DVec3 Vec3UnitCirclePointFromAngle(float HorizontalDegrees, float VerticalDegrees, T3DVec3 CenterPoint)
{
    T3DVec3 ResultingVector;
    float HRadians = T3D_DEG_TO_RAD(HorizontalDegrees);
    float VRadians = T3D_DEG_TO_RAD(VerticalDegrees);

    // Calculate x, y, z coordinates from spherical coordinates
    ResultingVector.v[0] = CenterPoint.v[0] + cos(VRadians) * sin(HRadians);
    ResultingVector.v[1] = CenterPoint.v[1] + sin(VRadians);
    //ResultingVector.v[2] = CenterPoint.v[2] + cos(HRadians);
    ResultingVector.v[2] = CenterPoint.v[2] + cos(VRadians) * cos(HRadians);

    return ResultingVector;
}

// Get the forward vector of the camera
T3DVec3 GetCameraForwardVector(T3DVec3 CameraPosition, T3DVec3 CameraTarget)
{
    T3DVec3 ResultingVector;

    // t3d_vec3_diff subtracts one vector from another
    t3d_vec3_diff(&ResultingVector, &CameraTarget, &CameraPosition);
    t3d_vec3_norm(&ResultingVector);
    return ResultingVector;
}

// ----- Matrix math -----
T3DMat4 CreateSRTMatrix(float Position[3], float Rotation[3], float Scale[3])
{
    T3DMat4 NewMatrix;

    // Create a matrix so the model can be rendered
    t3d_mat4_from_srt_euler(&NewMatrix,
        Scale,
        Rotation,
        Position
    );

    return NewMatrix;
}

void UpdateTransformMatrix(struct ModelTransform *Transform)
{
    Transform->ModelMatrix = CreateSRTMatrix(Transform->Position, Transform->Rotation, Transform->Scale);
    t3d_mat4_to_fixed(Transform->ModelMatrixFP, &Transform->ModelMatrix);
}