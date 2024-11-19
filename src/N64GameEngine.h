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
struct CameraProperties
{
    T3DVec3 Position;
    T3DVec3 Target;
    T3DVec3 UpDir;
    float FOV;
};

struct ControllerState
{
    joypad_buttons_t ReleasedButtons;
    joypad_buttons_t PressedButtons;
    joypad_buttons_t HeldButtons;
    int StickState[2];
};

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
extern float CameraClipping[2];
extern float DeltaTime;
extern float FOV3D;
extern int FrameCount;
extern int TargetFPS;
extern int FPS;


/* FUNCTIONS */
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

// ----- Drawing functions -----
void DrawString(char* Text, int FontID, int XPos, int YPos);
void RenderModel(T3DModel *ModelToRender, struct ModelTransform *Transform, bool CreateNewRenderBlock, bool SkipDraw);
void ClearScreen(color_t ClearColor, uint8_t *GlobalLightColor, uint8_t *SunColor, T3DVec3 *SunDirection);
void UpdateViewport(T3DViewport *Viewport, struct CameraProperties CamProps);
void StartFrame(T3DViewport *Viewport);
void EndFrame();

// ----- Input functions -----
void GetControllerInput(struct ControllerState *StructToUpdate, int ControllerPort);
bool IsControllerConnected(int ControllerPort);

#endif