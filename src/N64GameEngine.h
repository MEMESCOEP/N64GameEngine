/* N64 GAME ENGINE */
// Main engine header
// Written by MEMESCOEP
// November of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


// Define N64GAMEENGINE_H if it isn't already
#ifndef N64GAMEENGINE_H
#define N64GAMEENGINE_H
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>


/* DEFINITIONS */
#define HEAPSTATS_UPDATE_MS 100
#define FPS_UPDATE_MS 250


/* VARIABLES */
// Internal engine debug modes
//  ALL -> All available debug information is printed
//  NONE -> No debug information is printed
//  MINIMAL -> Minimal debug information is printed (FPS target updates, )
enum EngineDebugModes
{
    ALL,
    NONE,
    MINIMAL
};

// Stores the camera's position, target (3D point to look at), it's up direction, and the FOV
struct CameraProperties
{
    T3DVec3 Position;
    T3DVec3 Target;
    T3DVec3 UpDir;
    float FOV;
};

// Stores state information about buttons and joysticks
struct ControllerState
{
    joypad_buttons_t ReleasedButtons;
    joypad_buttons_t PressedButtons;
    joypad_buttons_t HeldButtons;
    float StickStateNormalized[2];
    int StickState[2];
};

// Stores world space transform information like SRT matrix data
// Note that the rotation (euler angles here) is in degrees
// Manually creating a transform is not recommended
struct ModelTransform
{
    rspq_block_t *RenderBlock;
    T3DMat4FP *ModelMatrixFP;
    T3DMat4 ModelMatrix;
    float Position[3];
    float Rotation[3];
    float Scale[3];
};

extern struct CameraProperties DefaultCameraProperties;
extern heap_stats_t HeapStats;
extern rspq_block_t *DrawCommands;
extern T3DVec3 WorldUpVector;
extern float CameraClipping[2];
extern float DeltaTime;
extern float FOV3D;
extern int FrameCount;
extern int TargetFPS;
extern int FPS;


/* FUNCTIONS */
// ----- Debug functions -----
void SetDebugMode(enum EngineDebugModes DebugMode);
enum EngineDebugModes GetDebugMode();
void DebugPrint(char *Message, enum EngineDebugModes DebugMode, ...);
void FancyPrintMatrixFP(T3DMat4FP MatrixFP);
void FancyPrintMatrix(T3DMat4 Matrix);
void FancyPrintVector3(T3DVec3 Vector);

// ----- Engine functions -----
void InitSystem(resolution_t Resolution, bitdepth_t BitDepth, uint32_t BufferNum, filter_options_t Filters, bool Init3D);
void UpdateEngine();

// ----- Registration functions -----
rdpq_font_t *RegisterFont(char *FontPath, int FontID);

// ----- Creation functions -----
struct ModelTransform CreateNewModelTransform();
void AssignNewRenderBlock(struct ModelTransform *Transform, T3DModel *ModelToRender);

// ----- Timing functions -----
long long UptimeMilliseconds();
void SetTargetFPS(int Target);
float MSPFFromFPS(int FPSToConvert);
float MSToTicks(int MS);
float FPSToMS(int FPSToConvert);

// ----- Camera Functions -----
void RotateCameraToAngle(float XAngle, float YAngle, struct CameraProperties *CamProps);
void RotateCameraRelative(float XAngle, float YAngle, float ZAngle, struct CameraProperties *CamProps);
void RotateCameraAroundPoint(float RotationAngle, struct CameraProperties *CamProps, T3DVec3 PointToRotateAround);
void MoveCameraVertical(struct CameraProperties *CamProps, float DistanceStep, bool UseWorldUp);
void MoveCameraLateral(T3DVec3 *CameraPosition, T3DVec3 *CameraTarget, float DistanceStep, bool UseWorldForward);
void MoveCameraStrafe(struct CameraProperties *CamProps, float DistanceStep, bool UseWorldRight);

// ----- Drawing functions -----
void DrawString(char* Text, int FontID, int XPos, int YPos);
void RenderModel(T3DModel *ModelToRender, struct ModelTransform *Transform, bool UpdateMatrix);
void RenderMultiModel(T3DModel *ModelToRender, struct ModelTransform *Transform, bool UpdateMatrix);
void ClearScreen(color_t ClearColor);
void UpdateLightProperties(int LightCount, uint8_t *GlobalLightColor, uint8_t *SunColor, T3DVec3 *SunDirection);
void UpdateViewport(T3DViewport *Viewport, struct CameraProperties CamProps);
void StartFrame();
void EndFrame();
void Start3DMode(T3DViewport *Viewport);
void Start2DMode();

// ----- Input functions -----
void GetControllerInput(struct ControllerState *StructToUpdate, int ControllerPort);

#endif