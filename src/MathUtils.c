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
#include "MathUtils.h"


/* FUNCTIONS */
// ----- Vector math -----
// Rotate a vector3 by x degrees around the X axis
void RotateVectorAxisX(T3DVec3* VectorToRotate, float Degrees)
{
    float Radians = T3D_DEG_TO_RAD(Degrees);

    VectorToRotate->v[1] += VectorToRotate->v[1] * cos(Radians) - VectorToRotate->v[2] * sin(Radians);
    VectorToRotate->v[2] += VectorToRotate->v[1] * sin(Radians) + VectorToRotate->v[2] * cos(Radians);
}

// Rotate a vector3 by x degrees around the Y axis
void RotateVectorAxisY(T3DVec3* VectorToRotate, float Degrees)
{
    float Radians = T3D_DEG_TO_RAD(Degrees);

    VectorToRotate->v[0] += VectorToRotate->v[0] * cos(Radians) + VectorToRotate->v[2] * sin(Radians);
    VectorToRotate->v[2] += -VectorToRotate->v[0] * sin(Radians) + VectorToRotate->v[2] * cos(Radians);
}

// Rotate a vector3 by x degrees around the Z axis
void RotateVectorAxisZ(T3DVec3* VectorToRotate, float Degrees)
{
    float Radians = T3D_DEG_TO_RAD(Degrees);

    VectorToRotate->v[0] += VectorToRotate->v[0] * cos(Radians) - VectorToRotate->v[1] * sin(Radians);
    VectorToRotate->v[1] += VectorToRotate->v[0] * sin(Radians) + VectorToRotate->v[1] * cos(Radians);
}

// Rotate a vector3 by x degrees
void RotateVector3ByDegrees(T3DVec3* Vector3ToRotate, T3DVec3 RotationDegrees)
{
    RotateVectorAxisX(Vector3ToRotate, RotationDegrees.v[0]);
    RotateVectorAxisY(Vector3ToRotate, RotationDegrees.v[1]);
    RotateVectorAxisZ(Vector3ToRotate, RotationDegrees.v[2]);
}

// Multiply the X, Y, and Z axes by a value
void MultiplyAxesByFloat(T3DVec3* VectorToUpdate, T3DVec3 AxesToUpdate, float MultiplyValue)
{
    VectorToUpdate->v[0] = VectorToUpdate->v[0] * (MultiplyValue * AxesToUpdate.v[0] != 0 ? MultiplyValue : 1.0f);
    VectorToUpdate->v[0] = VectorToUpdate->v[1] * (MultiplyValue * AxesToUpdate.v[1] != 0 ? MultiplyValue : 1.0f);
    VectorToUpdate->v[0] = VectorToUpdate->v[2] * (MultiplyValue * AxesToUpdate.v[2] != 0 ? MultiplyValue : 1.0f);
}

// Add a value to the X, Y, and Z axes
void AddFloatToAxes(T3DVec3* VectorToUpdate, T3DVec3 AxesToUpdate, float ValueToAdd)
{
    VectorToUpdate->v[0] = VectorToUpdate->v[0] + (ValueToAdd * AxesToUpdate.v[0]);
    VectorToUpdate->v[1] = VectorToUpdate->v[1] + (ValueToAdd * AxesToUpdate.v[1]);
    VectorToUpdate->v[2] = VectorToUpdate->v[2] + (ValueToAdd * AxesToUpdate.v[2]);
}

// Set all zeroes in a vector to one
// This is useful for cases where you need to multiply by a vector, and your axes to update are zero.
void OneifyVectorZeroes(T3DVec3* VectorToUpdate)
{
    for (int AxisIndex = 0; AxisIndex < 3; AxisIndex++)
    {
        if (VectorToUpdate->v[AxisIndex] == 0.0f) VectorToUpdate->v[AxisIndex] = 1.0f;
    }
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

// Get the normalized forward vector from two positions
T3DVec3 GetForwardVector(T3DVec3 Position1, T3DVec3 Position2)
{
    T3DVec3 ResultingVector;

    t3d_vec3_diff(&ResultingVector, &Position2, &Position1);
    t3d_vec3_norm(&ResultingVector);

    return ResultingVector;
}

// Get the normalized right vector from two positions
T3DVec3 GetRightVector(T3DVec3 ForwardVector)
{
    T3DVec3 ResultingVector;

    t3d_vec3_cross(&ResultingVector, &ForwardVector, &WorldUpVector);
    t3d_vec3_norm(&ResultingVector);

    return ResultingVector;
}

// Get the normalized up vector from two positions
T3DVec3 GetUpVector(T3DVec3 ForwardVector, T3DVec3 RightVector)
{
    T3DVec3 ResultingVector;

    t3d_vec3_cross(&ResultingVector, &RightVector, &ForwardVector);
    t3d_vec3_norm(&ResultingVector);

    return ResultingVector;
}

// Get the distance between two vectors as a float
// See https://math.stackexchange.com/a/42642 for the formula
float VectorDistance(T3DVec3 FirstVector, T3DVec3 SecondVector)
{
    return sqrt(
        pow(SecondVector.v[0] - FirstVector.v[0], 2) + 
        pow(SecondVector.v[1] - FirstVector.v[1], 2) + 
        pow(SecondVector.v[2] - FirstVector.v[2], 2)
    );
}

// ----- Matrix math -----
T3DMat4 CreateSRTMatrix(T3DVec3 Position, T3DVec3 Rotation, T3DVec3 Scale)
{
    T3DMat4 NewMatrix;

    // Create a matrix so the model can be rendered
    t3d_mat4_from_srt_euler(&NewMatrix,
        Scale.v,
        Rotation.v,
        Position.v
    );

    return NewMatrix;
}

void UpdateTransformMatrix(struct ModelTransform* Transform)
{
    Transform->ModelMatrix = CreateSRTMatrix(Transform->Position, Transform->Rotation, Transform->Scale);
    t3d_mat4_to_fixed(Transform->ModelMatrixFP, &Transform->ModelMatrix);
}

// ----- Range math -----
// Return zero if a number is below the minimum
float ZeroBelowMinimum(float Number, float Minimum)
{
    return Number >= Minimum ? Number : 0.0f;
}

// Return zero if a number is above the maximum
float ZeroAboveMaximum(float Number, float Maximum)
{
    return Number <= Maximum ? Number : 0.0f;
}

// Keep a number between a range, regardless of the input number's sign. The returned float will still have the same sign as the input.
float UnsignedKeepInRange(float Number, float Minimum, float Maximum)
{
    float ABSNumber = ABS(Number);
    int NumSign = SIGN(Number);

    if (NumSign == 0)
    {
        NumSign = 1;
    }

    return MAX(MIN(ABSNumber, Maximum), Minimum) * NumSign;
}

// ----- Array math -----
// Scale a float[3] by a float
void ScaleFloat3(float FloatToScale[3], float ScaleValue)
{
    FloatToScale[0] *= ScaleValue;
    FloatToScale[1] *= ScaleValue;
    FloatToScale[2] *= ScaleValue;
}

// Add a vector3 to a float[3]
void AddVector3ToFloat3(float Float3[3], T3DVec3 VectorToAdd)
{
    //Float3[0] = 
}