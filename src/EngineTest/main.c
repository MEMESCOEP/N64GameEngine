/* N64 GAME ENGINE */
// Engine test program
// Written by MEMESCOEP
// November of 2024


/* LIBRARIES */
#include <stdlib.h>
#include <math.h>
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include "../N64GameEngine.h"
#include "../MathUtils.h"


/* VARIABLES */
struct CameraProperties CamProps;
struct ControllerState Input;
struct ModelTransform FloorModelTransform;
struct ModelTransform FenceModelTransform;
struct ModelTransform GearModelTransform1;
struct ModelTransform FenceModelTransforms[4];
struct ModelTransform BushModelTransforms[4];
struct ModelTransform HeadModelTransforms[4];
T3DViewport Viewport;
rdpq_font_t *NewFont;
T3DModel *HeadModels[4];
T3DModel *FloorModel;
T3DModel *FenceModel;
T3DModel *GearModel;
T3DModel *BushModel;
T3DVec3 SunDirection = {{-1.0f, 1.0f, 1.0f}};
color_t SkyColor = (color_t){0x1C, 0x2E, 0x4A, 0xFF};
uint8_t AmbientColor[4] = {80, 80, 100, 0xFF};
uint8_t SunColor[4] = {0x2D, 0x43, 0x67, 0xFF};
char *HeadModelPaths[4] = {"rom:/Pikachu.t3dm", "rom:/Mario.t3dm", "rom:/Link.t3dm", "rom:/FoxMcCloud.t3dm"};
float FencePositions[4][2] = {{175.0f, 175.0f}, {175.0f, -175.0f}, {-175.0f, -175.0f}, {-175.0f, 175.0f}};
float BushPositions[4][2] = {{175.0f, 175.0f}, {175.0f, -175.0f}, {-175.0f, -175.0f}, {-175.0f, 175.0f}};
float HeadPositions[4][2] = {{175.0f, 175.0f}, {175.0f, -175.0f}, {-175.0f, -175.0f}, {-175.0f, 175.0f}};
float RotationSpeed = 0.25f;
float ModelAngle = 0.0f;
int CameraMode = 0;
int DebugMode = 1;


/* FUNCTIONS */
color_t GetRainbowColor(float s) {
    float r = fm_sinf(s + 0.0f) * 127.0f + 128.0f;
    float g = fm_sinf(s + 2.0f) * 127.0f + 128.0f;
    float b = fm_sinf(s + 4.0f) * 127.0f + 128.0f;
    return RGBA32(r, g, b, 255);
}

int main()
{
    // Set the engine's debug mode to minimal output
    SetDebugMode(MINIMAL);

    // Initialize the system and Tiny3D
    // We use 320x240@16 here because in this case it strikes a balance between performance and video quality
    InitSystem(RESOLUTION_320x240, DEPTH_16_BPP, 3, FILTERS_RESAMPLE_ANTIALIAS, true);
    SetTargetFPS(60);
    NewFont = RegisterFont("rom:/DEBUG.font64", 1);
    Viewport = t3d_viewport_create();

    t3d_vec3_norm(&SunDirection);
    
    // Set up the camera
    CamProps = DefaultCameraProperties;
    CamProps.Position = (T3DVec3){{0.0f, -25.0f, 125.0f}};
    CamProps.Target = (T3DVec3){{0.0f, -50.0f, 0.0f}};
    CamProps.FOV = 90.0f;

    // Load models and set up transforms
    FloorModel = t3d_model_load("rom:/Floor.t3dm");
    FenceModel = t3d_model_load("rom:/Fence.t3dm");
    GearModel = t3d_model_load("rom:/Gear.t3dm");
    BushModel = t3d_model_load("rom:/StretchyBush.t3dm");

    for (int ModelIndex = 0; ModelIndex < 4; ModelIndex++)
    {
        //HeadModels[ModelIndex] = t3d_model_load(HeadModelPaths[ModelIndex]);
    }

    // This scale may need to be tweaked to prevent "jitter" and texture distortion
    FloorModelTransform = CreateNewModelTransform();
    FloorModelTransform.Position[0] = 0.0f;
    FloorModelTransform.Position[1] = -100.0f;
    FloorModelTransform.Position[2] = 0.0f;
    FloorModelTransform.Scale[0] = 0.75f;
    FloorModelTransform.Scale[1] = 1.0f;
    FloorModelTransform.Scale[2] = 0.75f;

    FenceModelTransform = CreateNewModelTransform();
    FenceModelTransform.Position[0] =  0.0f;
    FenceModelTransform.Position[1] = -75.0f;
    FenceModelTransform.Position[2] = -125.0f;
    FenceModelTransform.Scale[0] = 0.25f;
    FenceModelTransform.Scale[1] = 0.25f;
    FenceModelTransform.Scale[2] = 0.25f;

    GearModelTransform1 = CreateNewModelTransform();
    GearModelTransform1.Position[1] = -40.0f;
    GearModelTransform1.Scale[0] = 0.2f;
    GearModelTransform1.Scale[1] = 0.2f;
    GearModelTransform1.Scale[2] = 0.2f;

    // Create transforms for the 4 bush models
    for (int BMIndex = 0; BMIndex < 4; BMIndex++)
    {
        struct ModelTransform NewTransform = CreateNewModelTransform();

        NewTransform.Position[0] = BushPositions[BMIndex][0];
        NewTransform.Position[1] = -95.0f;
        NewTransform.Position[2] = BushPositions[BMIndex][1];
        NewTransform.Scale[0] = 0.5f;
        NewTransform.Scale[1] = 0.5f;
        NewTransform.Scale[2] = 0.5f;

        BushModelTransforms[BMIndex] = NewTransform;
    }    
    
    // Rotate the camera downwards about 45 degrees
    //RotateCameraRelative(0.0f, -45.0f, 0.0f, &CamProps);
    //RotateVector3ByDegrees(&CamProps.Target, (T3DVec3){{45.0f, -45.0f, 0.0f}});

    // Game loop
    while (true)
    {
        // Update the viewport (screen projection and camera transform)
        UpdateViewport(&Viewport, CamProps);

        // We'll update the scene before we start drawing to avoid potential graphical issues
        // Apply transformations to the scene's objects
        ModelAngle += 1.5f * DeltaTime;
        GearModelTransform1.Rotation[0] = ModelAngle * RotationSpeed;
        GearModelTransform1.Rotation[1] = ModelAngle * RotationSpeed;
        GearModelTransform1.Rotation[2] = ModelAngle * RotationSpeed * 4.0f;    

        // Read controller input from port 1
        GetControllerInput(&Input, JOYPAD_PORT_1);

        // Rotate the camera around the center of the scene, at a speed of 15 degrees per second
        if (CameraMode == 0)
        {
            RotateCameraAroundPoint(15.0f * DeltaTime, &CamProps, (T3DVec3){{0.0f, -50.0f, 0.0f}});
            CamProps.Target = (T3DVec3){{0.0f, -50.0f, 0.0f}};
        }
        else
        {
            RotateCameraRelative(Input.StickStateNormalized[0] * 75.0f * DeltaTime, Input.StickStateNormalized[1] * 75.0f * DeltaTime, 0.0f, &CamProps);

            // Move the camera
            if (Input.HeldButtons.d_up)
            {
                CamProps.Position = t3d_vec3_dot(CamProps.Position, GetCameraForwardVector(CamProps.Position, CamProps.Target));
            }
        }    

        // Switch between debug modes
        //  0: No debug info is displayed
        //  1: Minimal debug info is displayed (MEM, Uptime, FPS)
        //  2: All debug info is displayed (Info from 1, Camera properties, stick input)
        if (Input.PressedButtons.b)
        {
            DebugMode++;

            if (DebugMode > 2) DebugMode = 0;
        }

        // Switch between camera modes
        //  0: Rotate around the center of the screen
        //  1: Manual control
        if (Input.PressedButtons.a)
        {
            CameraMode++;

            if (CameraMode > 1) CameraMode = 0;
        }

        // Start the frame and enter 3D mode
        // All 3D graphics operations should almost always take place in 3D mode
        StartFrame();
        Start3DMode(&Viewport);        
        ClearScreen(SkyColor);
        UpdateLightProperties(1, AmbientColor, SunColor, &SunDirection);

        // Render models
        RenderModel(FloorModel, &FloorModelTransform, true);

        // Draw 4 bushes; one in each corner
        // Also draw 4 fences
        for (int ModelIndex = 0; ModelIndex < 4; ModelIndex++)
        {
            //RenderModel(FenceModel, &FenceModelTransforms[ModelIndex], true);
            RenderModel(BushModel, &BushModelTransforms[ModelIndex], true);
        }

        // The colored band on the gear model is using the 'prim-color' property
        rdpq_set_prim_color(GetRainbowColor(ModelAngle * 0.24f));
        RenderModel(GearModel, &GearModelTransform1, true);

        // Enter 2D mode
        // All 2D graphics operations should (usually) take place in 2D mode
        Start2DMode();

        // Draw debug statistics
        // All 2D text should be drawn after everything else unless you have a specific reason not to
        if (DebugMode > 0)
        {
            rdpq_text_printf(NULL, 1, 5, 12, "MEM USED: %.2f KB", (HeapStats.used / 1024.0f) * 1.024f);
            rdpq_text_printf(NULL, 1, 5, 24, "UPTIME: %.2fs", UptimeMilliseconds() / 1000.0f);
            rdpq_text_printf(NULL, 1, 5, 36, "FPS: %d/%d (%.1f%%, DT=%.3fms)", FPS, TargetFPS, ((float)FPS / TargetFPS) * 100.0f, DeltaTime);
            
            if (DebugMode == 2)
            {
                T3DVec3 CamForwardVector = GetCameraForwardVector(CamProps.Position, CamProps.Target);

                rdpq_text_printf(NULL, 1, 5, 48, "CAM TGT: %.3f, %.3f, %.3f", CamProps.Target.v[0], CamProps.Target.v[1], CamProps.Target.v[2]);
                rdpq_text_printf(NULL, 1, 5, 60, "CAM POS: %.3f, %.3f, %.3f", CamProps.Position.v[0], CamProps.Position.v[1], CamProps.Position.v[2]);
                rdpq_text_printf(NULL, 1, 5, 72, "UP DIR: %.3f, %.3f, %.3f", CamProps.UpDir.v[0], CamProps.UpDir.v[1], CamProps.UpDir.v[2]);
                rdpq_text_printf(NULL, 1, 5, 84, "CAM FWD: %.3f, %.3f, %.3f", CamForwardVector.v[0], CamForwardVector.v[1], CamForwardVector.v[2]);
                rdpq_text_printf(NULL, 1, 5, 96, "STICK: X=%f || Y=%f", Input.StickStateNormalized[0], Input.StickStateNormalized[1]);
            }
        }

        EndFrame();
    }

    t3d_destroy();
    return 0;
}
