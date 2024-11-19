#ifndef MATHUTILS_H
#define MATHUTILS_H
#include <t3d/t3d.h>


/* DEFINITIONS */
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


/* FUNCTIONS */
T3DVec3 RotateVector3ByDegrees(T3DVec3 Vector3ToRotate, float Degrees);
T3DVec3 Vec3UnitCirclePointFromAngle(T3DVec3 *VectorToRotate, T3DVec3 Position, float Degrees);
T3DVec3 GetCameraForwardVector(T3DVec3 CameraPosition, T3DVec3 CameraTarget);
#endif