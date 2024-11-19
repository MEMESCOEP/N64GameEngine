/* LIBRARIES */
#include <t3d/t3d.h>


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
T3DVec3 Vec3UnitCirclePointFromAngle(float HorizontalDegrees, float VerticalDegrees)
{
    T3DVec3 ResultingVector;
    float HRadians = T3D_DEG_TO_RAD(HorizontalDegrees);
    float VRadians = T3D_DEG_TO_RAD(VerticalDegrees);

    // Calculate x, y, z coordinates from spherical coordinates
    ResultingVector.v[0] = sin(VRadians) * cos(HRadians);
    ResultingVector.v[1] = sin(VRadians) * sin(HRadians);
    ResultingVector.v[2] = cos(VRadians);

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