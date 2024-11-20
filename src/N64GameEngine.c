/* LIBRARIES */
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

heap_stats_t HeapStats;
rspq_block_t *DrawCommands = NULL;
surface_t *DisplaySurface = NULL;
surface_t *DepthBuffer;
long long LastHeapStatsUpdate = 0;
long long LastFPSUpdate = 0;
long long LastDeltaTime = 0;
long long FPSStart = 0;
long long DTStart = 0;
float CameraClipping[2] = {3.0f, 200.0f};
float DeltaTime = 0.0f;
int N64ClockSpeed = 93750000;
int FPSCheckCount = 0;
int FrameCount = 0;
int TargetFPS = 60;
int FPS = 0;


/* FUNCTIONS */
// ----- Debug functions -----
/*void ConsolePrint(char* Message)
{
    debugf("    BushPosIndex %d\n", BushPosIndex);
}*/

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

// ----- Engine functions -----
// Initializes the display, debug, timer, rdpq, etc
void InitSystem(resolution_t Resolution, bitdepth_t BitDepth, uint32_t BufferNum, filter_options_t Filters, bool Init3D)
{
    debug_init_isviewer();
    debug_init_usblog();

    debugf("[INFO] >> Initializing timer...\n");
    timer_init();

    debugf("[INFO] >> Initializing controller(s)...\n");
    controller_init();

    debugf("[INFO] >> Initializing filesystem...\n");
    asset_init_compression(2);
    dfs_init(DFS_DEFAULT_LOCATION);

    debugf("[INFO] >> Initializing display...\n");
    display_init(Resolution, BitDepth, BufferNum, GAMMA_NONE, Filters);

    debugf("[INFO] >> Initializing RDPQ...\n");
    rdpq_init();

    debugf("[INFO] >> Initializing Tiny3D...\n");
    //t3d_debug_print_init();
    t3d_init((T3DInitParams){});

    debugf("[INFO] >> All init stages done.\n");
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
    NewModelTransform.ModelMatrixFP = malloc_uncached(sizeof(T3DMat4FP));
    NewModelTransform.RenderBlock = NULL;
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
    rspq_block_begin();
    t3d_matrix_push(Transform->ModelMatrixFP);
    t3d_model_draw(ModelToRender);
    t3d_matrix_pop(1);
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
    debugf("[INFO] >> Set target FPS to %d.\n", Target);
}

// Calculate milliseconds per frame for a given framerate
float MSPFFromFPS(int FPSToConvert)
{
    return FPSToMS(FPSToConvert) / FPSToConvert;
}

// Convert milliseconds to timer ticks
float MSToTicks(int MS)
{
    return TIMER_TICKS_LL(MS * 1000.0f);
}

// Determines the number of milliseconds in a given framerate
float FPSToMS(int FPSToConvert)
{
    return TIMER_MICROS_LL(N64ClockSpeed / FPSToConvert) / 1000.0f;
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

// Render a 3D model on the screen (SkipDraw is for debugging only)
void RenderModel(T3DModel *ModelToRender, struct ModelTransform *Transform, bool CreateNewRenderBlock, bool SkipDraw)
{
    // Create a matrix so the model can be rendered
    t3d_mat4_from_srt_euler(&Transform->ModelMatrix,
        Transform->Scale,
        Transform->Rotation,
        Transform->Position
    );

    t3d_mat4_to_fixed(Transform->ModelMatrixFP, &Transform->ModelMatrix);

    // Create a new render block if the current transform does not yet have one
    if (CreateNewRenderBlock == true && Transform->RenderBlock)
    {
        rspq_wait();
        rspq_block_free(Transform->RenderBlock);
        AssignNewRenderBlock(Transform, ModelToRender);
    }
    else if (!Transform->RenderBlock)
    {
        AssignNewRenderBlock(Transform, ModelToRender);
    }    

    // Render the model
    if (SkipDraw == false)
    {
        rspq_block_run(Transform->RenderBlock);
    }
}

// Clear the screen and adjust lighting information
void ClearScreen(color_t ClearColor, uint8_t *GlobalLightColor, uint8_t *SunColor, T3DVec3 *SunDirection)
{
    t3d_screen_clear_color(ClearColor);
    t3d_screen_clear_depth();
    t3d_light_set_ambient(GlobalLightColor);
    t3d_light_set_directional(0, SunColor, SunDirection);
    t3d_light_set_count(1);
}

// Updates the projection and camera
void UpdateViewport(T3DViewport *Viewport, struct CameraProperties CamProps)
{
    t3d_viewport_set_projection(Viewport, T3D_DEG_TO_RAD(CamProps.FOV), CameraClipping[0], CameraClipping[1]);
    t3d_viewport_look_at(Viewport, &CamProps.Position, &CamProps.Target, &CamProps.UpDir);
}

// Begin a frame
void StartFrame(T3DViewport *Viewport)
{
    if (!DisplaySurface)
    {        
        DepthBuffer = display_get_zbuf();
    }

    DisplaySurface = display_get();

    rdpq_attach(DisplaySurface, DepthBuffer);
    t3d_frame_start();
    t3d_viewport_attach(Viewport);
}

// Finish a frame
void EndFrame()
{
    rdpq_detach_show();
    UpdateEngine();
}

// ----- Input functions -----
void GetControllerInput(struct ControllerState *StructToUpdate, int ControllerPort)
{
    if (IsControllerConnected(ControllerPort) == false)
    {
        return;
    }

    joypad_poll();
    joypad_inputs_t StickState = joypad_get_inputs(ControllerPort);

    StructToUpdate->ReleasedButtons = joypad_get_buttons_released(ControllerPort);
    StructToUpdate->PressedButtons = joypad_get_buttons_pressed(ControllerPort);
    StructToUpdate->HeldButtons = joypad_get_buttons(ControllerPort);
    StructToUpdate->StickState[0] = StickState.stick_x;
    StructToUpdate->StickState[1] = StickState.stick_y;
}

bool IsControllerConnected(int ControllerPort)
{
    return joypad_is_connected(ControllerPort);
}