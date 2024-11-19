/* LIBRARIES */
#include <t3d/t3d.h>


/* FUNCTIONS */
// Rotate a vector3 by x degrees
T3DVec3 RotateVector3ByDegrees(T3DVec3 Vector3ToRotate, float Degrees)
{
    T3DVec3 ResultingVector = {{0.0f, 0.0f, 0.0f}};
    float Radians = T3D_DEG_TO_RAD(Degrees);

    ResultingVector.v[0] = Vector3ToRotate.v[0] * cosf(Radians) - Vector3ToRotate.v[2] * sinf(Radians);
    ResultingVector.v[1] = Vector3ToRotate.v[1];
    ResultingVector.v[2] = Vector3ToRotate.v[0] * sinf(Radians) + Vector3ToRotate.v[2] * cosf(Radians);
    
    return ResultingVector;
}

// Get a point on a unit circle using a position and an angle
T3DVec3 Vec3UnitCirclePointFromAngle(T3DVec3 *VectorToRotate, T3DVec3 Position, float Degrees)
{
    T3DVec3 ResultingVector = {{0.0f, VectorToRotate->v[1], 0.0f}};
    float Radians = T3D_DEG_TO_RAD(Degrees);

    ResultingVector.v[0] = cosf(Radians) * Position.v[0];  // X = cos(theta)
    ResultingVector.v[2] = sinf(Radians) * Position.v[2];  // Z = sin(theta) (this would normally be Y, but in this case it's 3D space and therefore needs to be Z)
    
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