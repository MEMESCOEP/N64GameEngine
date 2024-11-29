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
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include "N64GameEngine.h"
#include "MathUtils.h"


/* VARIABLES */
struct CameraProperties DefaultCameraProperties = {
    (T3DVec3){{0.0f, 0.0f, 0.0f}},
    (T3DVec3){{0.0f, 0.0f, 0.0f}},
    (T3DVec3){{0.0f, 1.0f, 0.0f}},
    70.0f
};

enum EngineDebugModes CurrentDebugMode = MINIMAL;
heap_stats_t HeapStats;
rspq_block_t *DrawCommands = NULL;
surface_t *DisplaySurface = NULL;
surface_t *DepthBuffer;
T3DVec3 WorldUpVector = {{0.0f, 1.0f, 0.0f}};
long long LastHeapStatsUpdate = 0;
long long LastFPSUpdate = 0;
long long LastDeltaTime = 0;
long long FPSStart = 0;
long long DTStart = 0;
float CameraClipping[2] = {10.0f, 200.0f};
float JoystickRange = 65.0f; // This is used to reduce erroneous inputs (EX: The player isn't touching a joystick but it still responds with a very small value)
float DeltaTime = 0.0f;
int FPSCheckCount = 0;
int FrameCount = 0;
int TargetFPS = 60;
int FPS = 0;


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

// Print a formatted or unformatted message to a console (only works if a console is available)
// You can use the following format specifiers:
//  %d -> Integer
//  %f -> Float & double
//  %s -> String
//  %c -> Character
//  %v -> Vector3 (T3DVec3)
void DebugPrint(char *Message, enum EngineDebugModes DebugMode, ...)
{
    if (CurrentDebugMode == NONE || (DebugMode == ALL && CurrentDebugMode == MINIMAL))
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

/*void ConsolePrint(char* Message)
{
    debugf("    BushPosIndex %d\n", BushPosIndex);
}*/

/*void FancyPrintMatrixFP(T3DMat4FP MatrixFP)
{
    for (int Y = 0; Y < 4; Y++)
    {
        for (int X = 0; X < 4; X++)
        {
            debugf("[%.2f] ", MatrixFP.m[Y][X]);
        }

        debugf("\n");
    }
}*/

// Print the contents of a 4x4 matrix
void FancyPrintMatrix(T3DMat4 Matrix)
{
    for (int Y = 0; Y < 4; Y++)
    {
        for (int X = 0; X < 4; X++)
        {
            debugf("[%.2f] ", Matrix.m[Y][X]);
        }

        debugf("\n");
    }
}

// Print the contents of a 3D vector
void FancyPrintVector3(T3DVec3 Vector)
{
    debugf("X=%.3f || Y=%.3f || Z=%.3f\n", Vector.v[0], Vector.v[1], Vector.v[2]);
}

// ----- Engine functions -----
// Initializes the display, debug, timer, rdpq, etc
void InitSystem(resolution_t Resolution, bitdepth_t BitDepth, uint32_t BufferNum, filter_options_t Filters, bool Init3D)
{
    debug_init_isviewer();
    debug_init_usblog();

    DebugPrint("[== N64 Game Engine ==]\n", ALL);    
    DebugPrint("[INFO] >> Initializing display (%dx%d @ %dBPP)...\n", MINIMAL, Resolution.width, Resolution.height, (BitDepth + 1) * 16);
    display_init(Resolution, BitDepth, BufferNum, GAMMA_NONE, Filters);

    DebugPrint("[INFO] >> Initializing timer...\n", ALL);
    timer_init();

    DebugPrint("[INFO] >> Initializing controllers...\n", ALL);
    controller_init();

    DebugPrint("[INFO] >> Initializing filesystem & assets...\n", ALL);
    asset_init_compression(2);
    dfs_init(DFS_DEFAULT_LOCATION);

    DebugPrint("[INFO] >> Initializing RDPQ...\n", ALL);
    rdpq_init();

    DebugPrint("[INFO] >> Initializing Tiny3D...\n", ALL);
    t3d_init((T3DInitParams){});

    DebugPrint("[INFO] >> All engine init stages done.\n", ALL);
}

// Update all engine data when required
void UpdateEngine()
{
    // Update FPS reading every x millisecond(s)
    // This has been commented out because we don't need to do it when calling display_get_fps, which has a slower update time but is slightly more accurate
    /*if (UptimeMilliseconds() - LastFPSUpdate >= FPS_UPDATE_MS)
    {
        LastFPSUpdate = UptimeMilliseconds();
        FPS = FPSCheckCount * (1000.0f / FPS_UPDATE_MS);
        FPSCheckCount = 0;
    }*/

    // Update heap statistics every x millisecond(s)
    if (UptimeMilliseconds() - LastHeapStatsUpdate >= HEAPSTATS_UPDATE_MS)
    {
        LastHeapStatsUpdate = UptimeMilliseconds();
        sys_get_heap_stats(&HeapStats);
    }

    FPSCheckCount++;
    FrameCount++;
    DeltaTime = display_get_delta_time();
    FPS = display_get_fps();
}

// ----- Registration functions -----
// Registers a font to the specified font ID
rdpq_font_t *RegisterFont(char *FontPath, int FontID)
{
    rdpq_font_t *NewFont = rdpq_font_load(FontPath);
    rdpq_font_style(NewFont, 0, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF),
    });

    rdpq_text_register_font(FontID, NewFont);
    return NewFont;
}

// ----- Creation functions -----
// Creates a new model transform for use with 3D rendering
struct ModelTransform CreateNewModelTransform()
{
    struct ModelTransform NewModelTransform;

    // Allocate a fixed-point matrix pointer
    NewModelTransform.ModelMatrixFP = malloc_uncached(sizeof(T3DMat4FP));
    NewModelTransform.RenderBlock = NULL;

    // Set SRT transform data. This will prevent undefined behavior because we initialize to a known value
    // Note that rotation (euler angles) is in degrees
    NewModelTransform.Position[0] = 0.0f;
    NewModelTransform.Position[1] = 0.0f;
    NewModelTransform.Position[2] = 0.0f;
    NewModelTransform.Rotation[0] = 0.0f;
    NewModelTransform.Rotation[1] = 0.0f;
    NewModelTransform.Rotation[2] = 0.0f;
    NewModelTransform.Scale[0] = 1.0f;
    NewModelTransform.Scale[1] = 1.0f;
    NewModelTransform.Scale[2] = 1.0f;

    t3d_mat4_identity(&NewModelTransform.ModelMatrix);
    return NewModelTransform;
}

// Creates a new render block and assigns it to a model transform
void AssignNewRenderBlock(struct ModelTransform *Transform, T3DModel *ModelToRender)
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

// ----- Timing functions -----
// Returns the console's uptime in milliseconds (converts uptime in microseconds to milliseconds)
long long UptimeMilliseconds()
{
    return TIMER_MICROS_LL(timer_ticks()) / 1000;
}

// Determines the number of milliseconds to wait to reach the target framerate
void SetTargetFPS(int Target)
{
    TargetFPS = Target;
    display_set_fps_limit(TargetFPS);
    DebugPrint("[INFO] >> Set target FPS to %d.\n", MINIMAL, TargetFPS);
}

// ----- Camera Functions -----
// Rotate the camera to x degrees (not relative to its current angles) [BROKEN]
void RotateCameraToAngle(float XAngle, float YAngle, struct CameraProperties *CamProps)
{
    T3DVec3 UnitSpherePoint = Vec3UnitCirclePointFromAngle(XAngle, YAngle, CamProps->Position);
    
    CamProps->Target.v[0] = UnitSpherePoint.v[0];
    CamProps->Target.v[1] = UnitSpherePoint.v[1];
    CamProps->Target.v[2] = UnitSpherePoint.v[2];
}

// Rotate the camera by x degrees (relative to its current angles)
void RotateCameraRelative(float XAngle, float YAngle, float ZAngle, struct CameraProperties *CamProps)
{
    float XRadians = T3D_DEG_TO_RAD(XAngle);
    float YRadians = T3D_DEG_TO_RAD(YAngle);
    float ZRadians = T3D_DEG_TO_RAD(ZAngle);
    T3DVec3 TargetRotation;

    // Calculate the direction from the camera to the target
    TargetRotation.v[0] = CamProps->Target.v[0] - CamProps->Position.v[0];
    TargetRotation.v[2] = CamProps->Target.v[2] - CamProps->Position.v[2];
    //t3d_vec3_norm(&TargetRotation);

    // Left / Right rotation
    CamProps->Target.v[0] = CamProps->Position.v[0] + (TargetRotation.v[0] * cos(XRadians) - TargetRotation.v[2] * sin(XRadians));
    CamProps->Target.v[2] = CamProps->Position.v[2] + (TargetRotation.v[0] * sin(XRadians) + TargetRotation.v[2] * cos(XRadians));
    CamProps->UpDir.v[2] = ZRadians;

    // Up / Down rotation
    CamProps->Target.v[1] += YAngle;

    /*CamProps->Target.v[0] = CamProps->Position.v[0] + (cos(player.phi) * cos(player.theta));
    CamProps->Target.v[1] = CamProps->Position.v[1] + (cos(player.phi) * sin(player.theta));
    CamProps->Target.v[2] = CamProps->Position.v[2] + sin(player.phi);*/
}

// Rotate the camera by x degrees around a 3D point
void RotateCameraAroundPoint(float RotationAngle, struct CameraProperties *CamProps, T3DVec3 PointToRotateAround)
{
    float Radians = T3D_DEG_TO_RAD(RotationAngle);
    float DX = CamProps->Position.v[0] * cos(Radians) - CamProps->Position.v[2] * sin(Radians);
    float DZ = CamProps->Position.v[0] * sin(Radians) + CamProps->Position.v[2] * cos(Radians);

    CamProps->Position.v[0] = PointToRotateAround.v[0] + DX;
    CamProps->Position.v[2] = PointToRotateAround.v[2] + DZ;
    CamProps->Target.v[1] = PointToRotateAround.v[1];
}

// Moves the camera up and down along its local or world up vector
void MoveCameraVertical(struct CameraProperties *CamProps, float DistanceStep, bool UseWorldUp)
{
    T3DVec3 CamUpVector = WorldUpVector;
 
    if (UseWorldUp == false)
    {
        T3DVec3 CamForwardVector = GetForwardVector(CamProps->Position, CamProps->Target);
        CamUpVector = GetUpVector(CamForwardVector, GetRightVector(CamForwardVector));
    }

    t3d_vec3_scale(&CamUpVector, &CamUpVector, DistanceStep);
    t3d_vec3_add(&CamProps->Position, &CamProps->Position, &CamUpVector);
    t3d_vec3_add(&CamProps->Target, &CamProps->Target, &CamUpVector);
}

// Moves the camera along its local or world forward vector
void MoveCameraLateral(T3DVec3 *CameraPosition, T3DVec3 *CameraTarget, float DistanceStep, bool UseWorldForward)
{
    if (UseWorldForward == true)
    {
        
    }
    else
    {
        T3DVec3 CamForwardVector = GetForwardVector(*CameraPosition, *CameraTarget);

        t3d_vec3_scale(&CamForwardVector, &CamForwardVector, DistanceStep);
        t3d_vec3_add(CameraPosition, CameraPosition, &CamForwardVector);
        t3d_vec3_add(CameraTarget, CameraTarget, &CamForwardVector);
    }
}

// Moves the camera side to side along its local or world right vector
void MoveCameraStrafe(struct CameraProperties *CamProps, float DistanceStep, bool UseWorldRight)
{
    if (UseWorldRight == true)
    {
        
    }
    else
    {
        T3DVec3 CamForwardVector = GetForwardVector(CamProps->Position, CamProps->Target);
        T3DVec3 CamRightVector = GetRightVector(CamForwardVector);

        t3d_vec3_scale(&CamRightVector, &CamRightVector, DistanceStep);
        t3d_vec3_add(&CamProps->Position, &CamProps->Position, &CamRightVector);
        t3d_vec3_add(&CamProps->Target, &CamProps->Target, &CamRightVector);
    }
}

// Moves the camera to a specific point in 3D space (X, Y, Z) while keeping its rotation the same
void MoveCameraToPoint(struct CameraProperties *CamProps, T3DVec3)
{
    T3DVec3 TargetDifference;
    
    t3d_vec3_diff(&TargetDifference, &CamProps->Position, &CamProps->Target);
    FancyPrintVector3(TargetDifference);
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

// [USES TRANSFORM] Render a 3D model at the specified SRT
void RenderModel(T3DModel *ModelToRender, struct ModelTransform *Transform, bool UpdateMatrix)
{
    if (UpdateMatrix == true)
    {
        UpdateTransformMatrix(Transform);
    }

    // Render the model
    if (Transform->RenderBlock == NULL)
    {
        AssignNewRenderBlock(Transform, ModelToRender);
    }    

    t3d_matrix_push_pos(1);
    t3d_matrix_set(Transform->ModelMatrixFP, true);
    rspq_block_run(Transform->RenderBlock);
    t3d_matrix_pop(1);
}

// [USES TRANSFORM] Render a 3D model without pushing / popping a matrix. The push / pop action should be done externally with the "t3d_matrix_push_pos" and "t3d_matrix_pop" functions
void RenderMultiModel(T3DModel *ModelToRender, struct ModelTransform *Transform, bool UpdateMatrix)
{
    if (UpdateMatrix == true)
    {
        UpdateTransformMatrix(Transform);
    }

    // Render the model
    if (Transform->RenderBlock == NULL)
    {
        AssignNewRenderBlock(Transform, ModelToRender);
    }    

    t3d_matrix_set(Transform->ModelMatrixFP, true);
    rspq_block_run(Transform->RenderBlock);
}

// Clear the screen and adjust lighting information
void ClearScreen(color_t ClearColor)
{
    t3d_screen_clear_color(ClearColor);
    t3d_screen_clear_depth();
}

// Set the light count and color
void UpdateLightProperties(int LightCount, uint8_t *GlobalLightColor, uint8_t *SunColor, T3DVec3 *SunDirection)
{
    t3d_light_set_ambient(GlobalLightColor);
    t3d_light_set_directional(0, SunColor, SunDirection);
    t3d_light_set_count(1);
}

// Update the projection and camera
void UpdateViewport(T3DViewport *Viewport, struct CameraProperties CamProps)
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
void EndFrame()
{
    rdpq_detach_show();
    UpdateEngine();
}

// Configure RDPQ for 3D
void Start3DMode(T3DViewport *Viewport)
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
void GetControllerInput(struct ControllerState *StructToUpdate, int ControllerPort)
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