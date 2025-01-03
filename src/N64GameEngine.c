/* N64 GAME ENGINE */
// Main engine file
// Written by MEMESCOEP
// November of 2024
// Thanks to the LibDragon and Tiny3D libraries for making this project possible
// LibDragon github -> https://github.com/DragonMinded/libdragon
// Tiny3D github -> https://github.com/HailToDodongo/tiny3d


/* LIBRARIES */
#include <stdarg.h>
#include <unistd.h>
#include "assert.h"
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include "N64GameEngine.h"
#include "ColorUtils.h"
#include "MathUtils.h"


/* VARIABLES */
struct CameraProperties DefaultCameraProperties = {
    (T3DVec3){{0.0f, 0.0f, 0.0f}},
    (T3DVec3){{0.0f, 0.0f, 0.0f}},
    (T3DVec3){{0.0f, 0.0f, 0.0f}},
    (T3DVec3){{0.0f, 0.0f, 0.0f}},
    (T3DVec3){{0.0f, 0.0f, 0.0f}},
    (T3DVec3){{0.0f, 1.0f, 0.0f}},
    70.0f
};

enum EngineDebugModes CurrentDebugMode = MINIMAL;
heap_stats_t HeapStats;
surface_t* DisplaySurface = NULL;
surface_t* DepthBuffer;
T3DVec3 WorldUpVector = {{0.0f, 1.0f, 0.0f}};
long long LastHeapStatsUpdate = 0;
float CameraClipping[2] = {10.0f, 200.0f};
float UsedMemPercentage = 0.0f;
float JoystickRange = 65.0f; // This is used to reduce erroneous inputs (EX: The player isn't touching a joystick but it still responds with a very small value)
float DeltaTime = 0.0f;
float TargetFPS = 60;
float FPS = 0;
bool DebugIsInitialized = false;
bool ShowMemoryWarnings = true;
bool VerifyEnoughMemory = true;
int FrameCount = 0;


/* FUNCTIONS */
// ----- Debug functions -----
// Set the engine's debug mode
// See the EngineDebugModes enum (defined in game engine header) for more details
void SetDebugMode(enum EngineDebugModes DebugMode)
{
    CurrentDebugMode = DebugMode;
}

// Get the engine's current debug mode
// See the EngineDebugModes enum (defined in game engine header) for more details
enum EngineDebugModes GetDebugMode()
{
    return CurrentDebugMode;
}

// Print a formatted or unformatted message to a console. Note that this only works
// if a console is available and debugging has been initialized. You can use the
// following format specifiers:
//  %d -> Integer
//  %f -> Float & double
//  %s -> String
//  %c -> Character
//  %v -> Vector3 (T3DVec3)
//  %m -> 4x4 Matrix (T3DMat4)
//  %C -> Color (color_t)
//  %H -> HSV (HSVColor)
void DebugPrint(char* Message, enum EngineDebugModes DebugMode, ...)
{
    if (DebugIsInitialized == false || CurrentDebugMode == NONE || (DebugMode == ALL && CurrentDebugMode == MINIMAL))
        return;

    if (CurrentDebugMode == ALL || DebugMode == ALL || DebugMode == CurrentDebugMode)
    {
        // This is essentially just a reimplementation of debugf with support for more types
        va_list ArgList;
        va_start(ArgList, Message);

        while (*Message != '\0')
        {
            if (*Message == '%')
            {
                Message++;

                if (*Message == 'd')
                {
                    debugf("%d", va_arg(ArgList, int));
                }
                else if (*Message == 's')
                {
                    debugf("%s", va_arg(ArgList, char*));
                }
                else if (*Message == 'c')
                {
                    debugf("%c", va_arg(ArgList, int));
                }
                else if (*Message == 'f')
                {
                    debugf("%f", va_arg(ArgList, double));
                }
                else if (*Message == 'v')
                {
                    T3DVec3 Vector = va_arg(ArgList, T3DVec3);
                    debugf("{X=%f, Y=%f, Z=%f}", Vector.v[0], Vector.v[1], Vector.v[2]);
                }
                else if (*Message == 'm')
                {
                    T3DMat4 Matrix = va_arg(ArgList, T3DMat4);
                    
                    for (int Y = 0; Y < 4; Y++)
                    {
                        for (int X = 0; X < 4; X++)
                        {
                            debugf("[%f] ", Matrix.m[Y][X]);
                        }

                        debugf("\n");
                    }
                }
                else if (*Message == 'C')
                {
                    color_t Color = va_arg(ArgList, color_t);
                    debugf("{R=%d, G=%d, B=%d, A=%d}", Color.r, Color.g, Color.b, Color.a);
                }
                else if (*Message == 'H')
                {
                    struct HSVColor Color = va_arg(ArgList, struct HSVColor);
                    debugf("{H=%f, S=%f, V=%f}", Color.H, Color.S, Color.V);
                }
                else if (*Message == '%')
                {
                    debugf("%%");
                }

                Message++;
            }
            else
            {
                debugf("%c", *Message);
                Message++;
            }
        }
        
        va_end(ArgList);
    }
}

// ----- Engine functions -----
// Stops the game and throws an error if there isn't enough memory to keep the console from crashing (>= 95% used). A warning
// will be printed to the console if the memory usage >= 75% and if memory warnings are enabled (ShowMemoryWarnings = true).
void CheckAvailableMemory()
{
    if (VerifyEnoughMemory == true)
    {
        if (ShowMemoryWarnings == true && UsedMemPercentage >= 0.75f)
        {
            DebugPrint("[WARNING] >> Over 75%% of memory is being used (currently at %f%%).\n", MINIMAL, UsedMemPercentage * 100.0f);
        }

        if (UsedMemPercentage >= 0.95f)
        {
            assertf(UsedMemPercentage >= 0.95f, "The console ran out of memory!");
        }   
    }
}

// Initializes the display, debug, timer, rdpq, etc
void InitSystem(resolution_t Resolution, bitdepth_t BitDepth, uint32_t BufferNum, filter_options_t Filters, bool InitDebug)
{
    if (InitDebug == true)
    {
        debug_init_isviewer();
        debug_init_usblog();
        DebugIsInitialized = true;
    }

    DebugPrint("[== N64 Game Engine ==]\n", ALL);    
    DebugPrint("[INFO] >> Initializing display (%dx%d @ %dBPP, %d buffers)...\n", MINIMAL, Resolution.width, Resolution.height, (BitDepth + 1) * 16, BufferNum);
    display_init(Resolution, BitDepth, BufferNum, GAMMA_NONE, Filters);
    SetTargetFPS(TargetFPS);

    DebugPrint("[INFO] >> Initializing timer...\n", ALL);
    timer_init();

    DebugPrint("[INFO] >> Initializing controllers...\n", ALL);
    controller_init();

    DebugPrint("[INFO] >> Initializing filesystem & assets (DEF_LOC: %d)...\n", ALL, DFS_DEFAULT_LOCATION);
    asset_init_compression(2);
    assert(dfs_init(DFS_DEFAULT_LOCATION) == DFS_ESUCCESS);

    DebugPrint("[INFO] >> Initializing RDPQ...\n", ALL);
    rdpq_init();

    DebugPrint("[INFO] >> Initializing Tiny3D...\n", ALL);
    t3d_init((T3DInitParams){});

    DebugPrint("[INFO] >> Updating heap statistics...\n", ALL);
    sys_get_heap_stats(&HeapStats);

    DebugPrint("[INFO] >> All engine init stages done.\n", ALL);
}

// Update all engine data when required
void UpdateEngine(struct CameraProperties* CamProps)
{
    UpdateCameraDirections(CamProps);

    // Update heap statistics every x millisecond(s)
    if (UptimeMilliseconds() - LastHeapStatsUpdate >= HEAPSTATS_UPDATE_MS)
    {
        LastHeapStatsUpdate = UptimeMilliseconds();
        sys_get_heap_stats(&HeapStats);
        
        UsedMemPercentage = (float)HeapStats.used / HeapStats.total;
        CheckAvailableMemory();
    }

    FrameCount++;
    DeltaTime = display_get_delta_time();
    FPS = display_get_fps();
}

// ----- Creation functions -----
// Creates a new render block and assigns it to a model transform
void AssignNewRenderBlock(struct ModelTransform* Transform, T3DModel* ModelToRender)
{
    if (Transform->RenderBlock != NULL)
    {
        rspq_wait();
        rspq_block_free(Transform->RenderBlock);
    }

    rspq_block_begin();
    t3d_model_draw(ModelToRender);
    Transform->RenderBlock = rspq_block_end();
}

// Creates a new model transform for use with 3D rendering
struct ModelTransform CreateNewModelTransform()
{
    struct ModelTransform NewModelTransform;

    // Allocate a fixed-point matrix pointer
    NewModelTransform.ModelMatrixFP = malloc_uncached(sizeof(T3DMat4FP));
    NewModelTransform.RenderBlock = NULL;

    // Set SRT transform data. This will prevent undefined behavior because we initialize to a known value
    // Note that rotation (euler angles) is in degrees
    NewModelTransform.Position = (T3DVec3){{0.0f, 0.0f, 0.0f}};
    NewModelTransform.Rotation = (T3DVec3){{0.0f, 0.0f, 0.0f}};
    NewModelTransform.Scale = (T3DVec3){{1.0f, 1.0f, 1.0f}};

    t3d_mat4_identity(&NewModelTransform.ModelMatrix);
    return NewModelTransform;
}

// Creates a new model object
void CreateNewModelObject(struct ModelObject* ModelOBJToUpdate, char* ModelPath)
{
    CreateNewModelObjectPredefined(ModelOBJToUpdate, t3d_model_load(ModelPath));
}

// Creates a new model object
void CreateNewModelObjectPredefined(struct ModelObject* ModelOBJToUpdate, T3DModel* Model)
{
    ModelOBJToUpdate->Transform = CreateNewModelTransform();
    ModelOBJToUpdate->Model = Model;

    if (ModelOBJToUpdate->Transform.RenderBlock == NULL)
    {
        AssignNewRenderBlock(&ModelOBJToUpdate->Transform, ModelOBJToUpdate->Model);
    }
}

// ----- Timing functions -----
// Returns the console's uptime in milliseconds (converts uptime in microseconds to milliseconds)
long long UptimeMilliseconds()
{
    return TIMER_MICROS_LL(timer_ticks()) / 1000;
}

// Sets the target framerate. Note that this will be limited by the refresh rate of the N64, so in
// some regions (PAL)  this will be slower than 60 FPS (NTSC)
void SetTargetFPS(float Target)
{
    TargetFPS = Target;
    display_set_fps_limit(TargetFPS);
    DebugPrint("[INFO] >> Set target FPS to %f.\n", MINIMAL, TargetFPS);
}

// ----- Camera Functions -----
// Update the camera's forward, right, and up vectors
void UpdateCameraDirections(struct CameraProperties* CamProps)
{
    CamProps->ForwardVector = GetForwardVector(CamProps->Position, CamProps->Target);
    CamProps->RightVector = GetRightVector(CamProps->ForwardVector);
    CamProps->UpVector = GetUpVector(CamProps->ForwardVector, GetRightVector(CamProps->ForwardVector));
}

// Rotate the camera to x degrees (not relative to its current angles) [BROKEN]
void RotateCameraToAngle(float XAngle, float YAngle, struct CameraProperties* CamProps)
{
    T3DVec3 UnitSpherePoint = Vec3UnitCirclePointFromAngle(XAngle, YAngle, CamProps->Position);
    
    CamProps->Target.v[0] = UnitSpherePoint.v[0];
    CamProps->Target.v[1] = UnitSpherePoint.v[1];
    CamProps->Target.v[2] = UnitSpherePoint.v[2];
}

// Rotate the camera by x degrees (relative to its current angles)
void RotateCameraRelative(float XAngle, float YAngle, float ZAngle, struct CameraProperties* CamProps)
{
    T3DVec3 TargetRotation = (T3DVec3){{0.0f, 0.0f, 0.0f}};
    float USRadius = t3d_vec3_distance(&CamProps->Position, &CamProps->Target);
    float XRadians = T3D_DEG_TO_RAD(XAngle);
    float YRadians = T3D_DEG_TO_RAD(YAngle);
    float ZRadians = T3D_DEG_TO_RAD(ZAngle);

    // Calculate the direction from the camera to the target
    TargetRotation.v[0] = CamProps->Target.v[0] - CamProps->Position.v[0];
    TargetRotation.v[2] = CamProps->Target.v[2] - CamProps->Position.v[2];
    t3d_vec3_norm(&TargetRotation);

    // Left / Right rotation
    CamProps->Target.v[0] = CamProps->Position.v[0] + (TargetRotation.v[0] * cos(XRadians) - TargetRotation.v[2] * sin(XRadians));
    CamProps->Target.v[2] = CamProps->Position.v[2] + (TargetRotation.v[0] * sin(XRadians) + TargetRotation.v[2] * cos(XRadians));
    CamProps->UpDir.v[2] = ZRadians;

    // Up / Down rotation
    CamProps->Target.v[1] += sin(YRadians) * USRadius;
}

// Rotate the camera by x degrees around a 3D point
void RotateCameraAroundPoint(float RotationAngle, struct CameraProperties* CamProps, T3DVec3 PointToRotateAround)
{
    float Radians = T3D_DEG_TO_RAD(RotationAngle);
    float DX = CamProps->Position.v[0] * cos(Radians) - CamProps->Position.v[2] * sin(Radians);
    float DZ = CamProps->Position.v[0] * sin(Radians) + CamProps->Position.v[2] * cos(Radians);

    CamProps->Position.v[0] = PointToRotateAround.v[0] + DX;
    CamProps->Position.v[2] = PointToRotateAround.v[2] + DZ;
    CamProps->Target.v[1] = PointToRotateAround.v[1];
}

// Moves the camera up and down along its local or world up vector
void MoveCameraVertical(struct CameraProperties* CamProps, float DistanceStep, bool UseWorldUp)
{
    T3DVec3 CamUpVector = WorldUpVector;
 
    if (UseWorldUp == false)
    {
        CamUpVector = CamProps->UpVector;
    }

    ScaleFloat3(CamUpVector.v, DistanceStep);
    t3d_vec3_add(&CamProps->Position, &CamProps->Position, &CamUpVector);
    t3d_vec3_add(&CamProps->Target, &CamProps->Target, &CamUpVector);
}

// Moves the camera along its local or world forward vector
void MoveCameraLateral(struct CameraProperties* CamProps, float DistanceStep, bool UseWorldForward)
{
    if (UseWorldForward == true)
    {
        
    }
    else
    {
        ScaleFloat3(CamProps->ForwardVector.v, DistanceStep);
        t3d_vec3_add(&CamProps->Position, &CamProps->Position, &CamProps->ForwardVector);
        t3d_vec3_add(&CamProps->Target, &CamProps->Target, &CamProps->ForwardVector);
    }
}

// Moves the camera side to side along its local or world right vector
void MoveCameraStrafe(struct CameraProperties* CamProps, float DistanceStep, bool UseWorldRight)
{
    if (UseWorldRight == true)
    {
        
    }
    else
    {
        ScaleFloat3(CamProps->RightVector.v, DistanceStep);
        t3d_vec3_add(&CamProps->Position, &CamProps->Position, &CamProps->RightVector);
        t3d_vec3_add(&CamProps->Target, &CamProps->Target, &CamProps->RightVector);
    }
}

// Moves the camera to a specific point in 3D space (X, Y, Z) while keeping its rotation the same
void MoveCameraToPoint(struct CameraProperties* CamProps, T3DVec3)
{
    T3DVec3 TargetDifference;
    
    t3d_vec3_diff(&TargetDifference, &CamProps->Position, &CamProps->Target);
}

// ----- Drawing functions -----
// Draws a string on the screen at the specified X & Y coords, and font
void DrawString(char* Text, int FontID, int XPos, int YPos)
{
    int StrLength = strlen(Text);
    rdpq_paragraph_t* par = rdpq_paragraph_build(&(rdpq_textparms_t){
        .valign = VALIGN_TOP,
        .align = ALIGN_LEFT,
        .width = 320,
        .height = 240,
        .wrap = WRAP_WORD,
    }, FontID, Text, &StrLength);

    rdpq_paragraph_render(par, XPos, YPos);
    rdpq_paragraph_free(par);
}

// Render a 3D model
void RenderModel(struct ModelObject ModelOBJ, bool UpdateMatrix)
{
    RenderModelWithTransform(ModelOBJ.Model, &ModelOBJ.Transform, UpdateMatrix);
}

// Render a 3D model with the specified SRT
void RenderModelWithTransform(T3DModel* ModelToRender, struct ModelTransform* Transform, bool UpdateMatrix)
{
    if (UpdateMatrix == true)
    {
        UpdateTransformMatrix(Transform);
    }

    if (Transform->RenderBlock == NULL)
    {
        AssignNewRenderBlock(Transform, ModelToRender);
    }

    // Render the model
    t3d_matrix_push_pos(1);
    t3d_matrix_set(Transform->ModelMatrixFP, true);
    rspq_block_run(Transform->RenderBlock);
    t3d_matrix_pop(1);
}

// Clear the screen and adjust lighting information
void ClearScreen(color_t ClearColor)
{
    t3d_screen_clear_color(ClearColor);
    t3d_screen_clear_depth();
}

// Set the light count and color
void UpdateLightProperties(int LightCount, uint8_t* GlobalLightColor, uint8_t* SunColor, T3DVec3* SunDirection)
{
    t3d_light_set_ambient(GlobalLightColor);
    t3d_light_set_directional(0, SunColor, SunDirection);
    t3d_light_set_count(1);
}

// Update the projection and camera
void UpdateViewport(T3DViewport* Viewport, struct CameraProperties CamProps)
{
    t3d_viewport_set_projection(Viewport, T3D_DEG_TO_RAD(CamProps.FOV), CameraClipping[0], CameraClipping[1]);
    t3d_viewport_look_at(Viewport, &CamProps.Position, &CamProps.Target, &CamProps.UpDir);
}

// Begin a frame
void StartFrame()
{
    if (!DisplaySurface)
    {        
        DepthBuffer = display_get_zbuf();
    }

    DisplaySurface = display_get();

    rdpq_attach(DisplaySurface, DepthBuffer);
}

// Finish a frame
void EndFrame(struct CameraProperties* CamProps)
{
    rdpq_detach_show();
    UpdateEngine(CamProps);
}

// Configure RDPQ for 3D
void Start3DMode(T3DViewport* Viewport)
{
    t3d_frame_start();
    t3d_viewport_attach(Viewport);
}

// Configure RDPQ for 2D
void Start2DMode()
{
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
}

// ----- Input functions -----
// Get input from a controller at the specified port.
// There are usually only 4 ports available for reading, and can be addressed using any integer between (and including) 0 and 3
void GetControllerInput(struct ControllerState* StructToUpdate, int ControllerPort)
{
    // Don't do anything if the controller isn't connected
    if (joypad_is_connected(ControllerPort) == false)
    {
        return;
    }

    joypad_poll();
    joypad_inputs_t StickState = joypad_get_inputs(ControllerPort);

    // Ensure the joystick value is between a certain range.
    // This eliminates erroneous inputs (EX: The player isn't touching a joystick but it still responds with a very small value) 
    float StickValuesInRange[2] = {
        ZeroBelowMinimum(ABS(StickState.stick_x), JoystickRange * 0.15f) * SIGN(StickState.stick_x),
        ZeroBelowMinimum(ABS(StickState.stick_y), JoystickRange * 0.15f) * SIGN(StickState.stick_y)
    };

    StructToUpdate->ReleasedButtons = joypad_get_buttons_released(ControllerPort);
    StructToUpdate->PressedButtons = joypad_get_buttons_pressed(ControllerPort);
    StructToUpdate->HeldButtons = joypad_get_buttons(ControllerPort);    
    StructToUpdate->StickStateNormalized[0] = UnsignedKeepInRange(StickValuesInRange[0] / JoystickRange, 0.0f, 1.0f);
    StructToUpdate->StickStateNormalized[1] = UnsignedKeepInRange(StickValuesInRange[1] / JoystickRange, 0.0f, 1.0f);
    StructToUpdate->StickState[0] = StickValuesInRange[0];
    StructToUpdate->StickState[1] = StickValuesInRange[1];
}