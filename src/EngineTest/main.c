/* LIBRARIES */
#include <stdlib.h>
#include <math.h>
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include "N64GameEngine.h"
#include "MathUtils.h"


/* VARIABLES */
enum
{
    DebugFont = 1
};

struct CameraProperties CamProps;
struct ControllerState Input;
struct ModelTransform FloorModelTransform;
struct ModelTransform GearModelTransform1;
struct ModelTransform GearModelTransform2;
struct ModelTransform BushModelTransform;
T3DViewport Viewport;
rdpq_font_t *NewFont;
T3DModel *FloorModel;
T3DModel *GearModel;
T3DModel *BushModel;
T3DVec3 SunDirection = {{-1.0f, 1.0f, 1.0f}};
color_t SkyColor = (color_t){.r=0x87, .g=0xCE, .b=0xEB, .a=0xFF};
uint8_t AmbientColor[4] = {80, 80, 100, 0xFF};
uint8_t SunColor[4] = {0xFD, 0xFB, 0xD3, 0xFF};
float BushPositions[4][2] = {{-175.0f, -175.0f}, {175.0f, -175.0f}, {-175.0f, 175.0f}, {175.0f, 175.0f}};
float RotationSpeed = 0.25f;
float ModelAngle = 0.0f;
float Gear2Pos = 0.0f;
float PosChg2 = -10.0f;
int DebugMode = 1;
int BIndex = 0;


/* FUNCTIONS */
color_t GetRainbowColor(float s) {
    float r = fm_sinf(s + 0.0f) * 127.0f + 128.0f;
    float g = fm_sinf(s + 2.0f) * 127.0f + 128.0f;
    float b = fm_sinf(s + 4.0f) * 127.0f + 128.0f;
    return RGBA32(r, g, b, 255);
}

int main()
{   
    // Initialize the system and Tiny3D
    InitSystem(RESOLUTION_320x240, DEPTH_16_BPP, 3, FILTERS_RESAMPLE_ANTIALIAS, true);
    SetTargetFPS(60);
    NewFont = RegisterFont("rom:/DEBUG.font64", DebugFont);
    Viewport = t3d_viewport_create();

    t3d_vec3_norm(&SunDirection);
    
    // Set up the camera
    CamProps = DefaultCameraProperties;
    CamProps.Position = (T3DVec3){{0.0f, 0.0f, 100.0f}};
    CamProps.FOV = 90.0f;

    // Load models and set up transforms
    FloorModel = t3d_model_load("rom:/Floor.t3dm");
    GearModel = t3d_model_load("rom:/Gear.t3dm");
    BushModel = t3d_model_load("rom:/StretchyBush.t3dm");

    FloorModelTransform = CreateNewModelTransform();
    FloorModelTransform.Position[0] = 0.0f;
    FloorModelTransform.Position[1] = -100.0f;
    FloorModelTransform.Position[2] = 0.0f;
    FloorModelTransform.Scale[0] = 1.0f;
    FloorModelTransform.Scale[1] = 1.0f;
    FloorModelTransform.Scale[2] = 1.0f;

    GearModelTransform1 = CreateNewModelTransform();
    GearModelTransform1.Position[2] = 0.0f;
    GearModelTransform1.Scale[0] = 0.2f;
    GearModelTransform1.Scale[1] = 0.2f;
    GearModelTransform1.Scale[2] = 0.2f;

    GearModelTransform2 = CreateNewModelTransform();
    GearModelTransform2.Position[0] = 0.0f;
    GearModelTransform2.Position[1] = -50.0f;
    GearModelTransform2.Position[2] = -125.0f;
    GearModelTransform2.Rotation[0] = 1.5f;
    GearModelTransform2.Rotation[1] = -3.0f;
    GearModelTransform2.Scale[0] = 0.075f;
    GearModelTransform2.Scale[1] = 0.075f;
    GearModelTransform2.Scale[2] = 0.075f;

    BushModelTransform = CreateNewModelTransform();
    BushModelTransform.Position[0] = 0.0f;
    BushModelTransform.Position[1] = -75.0f;
    BushModelTransform.Position[2] = 0.0f;
    BushModelTransform.Scale[0] = 0.3f;
    BushModelTransform.Scale[1] = 0.3f;
    BushModelTransform.Scale[2] = 0.3f;
    AssignNewRenderBlock(&BushModelTransform, BushModel);

    // Rotate the camera downwards about 45 degrees
    RotateCameraRelative(0.0f, -45.0f, 0.0f, &CamProps);

    // Game loop
    while (true)
    {
        // Update the viewport (screen projection and camera transform)
        UpdateViewport(&Viewport, CamProps);

        // Update the scene before we start drawing
        // Apply transformations to the gears
        ModelAngle += 1.5f * DeltaTime;
        Gear2Pos -= PosChg2 * DeltaTime;

        if (Gear2Pos > 25.0f)
        {
            Gear2Pos = 25.0f;
            PosChg2 = -PosChg2;
        }
        else if (Gear2Pos < -25.0f)
        {
            Gear2Pos = -25.0f;
            PosChg2 = -PosChg2;
        }

        // Gear 1
        GearModelTransform1.Rotation[0] = ModelAngle * RotationSpeed;
        GearModelTransform1.Rotation[1] = ModelAngle * RotationSpeed;
        GearModelTransform1.Rotation[2] = ModelAngle * RotationSpeed;

        // Gear 2
        GearModelTransform2.Rotation[2] = ModelAngle * RotationSpeed * 16.0f;

        // Rotate the camera around the center of the scene, at a speed of 15 degrees per second
        RotateCameraRelative(15.0f * DeltaTime, 0.0f, 5.0f * DeltaTime, &CamProps);

        // Read controller input from port 1
        GetControllerInput(&Input, JOYPAD_PORT_1);

        // Switch between debug modes
        //  0: No debug info is displayed
        //  1: Minimal debug info is displayed (MEM, Uptime, FPS)
        //  2: All debug info is displayed (Info from 1, Camera properties, stick input)
        if (Input.PressedButtons.b)
        {
            DebugMode++;

            if (DebugMode > 2) DebugMode = 0;
        }

        // Start the frame
        StartFrame(&Viewport);        
        ClearScreen(SkyColor, AmbientColor, SunColor, &SunDirection);

        // Render models
        RenderModel(FloorModel, &FloorModelTransform, false, false);

        // Update the bush transform's position so 4 bushes can be drawn (one per corner)
        // Please god let this work, what black magic is going on here
        BushModelTransform.Position[0] = BushPositions[BIndex][0];
        BushModelTransform.Position[2] = BushPositions[BIndex][1];

        for (int BushPosIndex = 0; BushPosIndex < 4; BushPosIndex++)
        {
            BIndex += BushPosIndex + 1;
            BushModelTransform.Position[0] = BushPositions[BushPosIndex][0];
            BushModelTransform.Position[2] = BushPositions[BushPosIndex][1];
            RenderModel(BushModel, &BushModelTransform, true, false); // BushPosIndex > 0 for SkipDraw parameter (debugging)
        }

        // The colored-band on the gear model is using the 'prim-color' property
        rdpq_set_prim_color(GetRainbowColor(ModelAngle * 0.24f));
        RenderModel(GearModel, &GearModelTransform1, false, false);
        rdpq_set_prim_color(GetRainbowColor(ModelAngle * 0.42));
        RenderModel(GearModel, &GearModelTransform2, false, false);

        // Draw debug statistics (all rendered text should be drawn after everything else, unless you have a specific reason not to)
        if (DebugMode > 0)
        {
            rdpq_text_printf(NULL, DebugFont, 5, 12, "MEMORY USED: %.2fkb", (HeapStats.used / 1024.0f) * 1.024f);
            rdpq_text_printf(NULL, DebugFont, 5, 24, "UPTIME: %.2fs", UptimeMilliseconds() / 1000.0f);
            rdpq_text_printf(NULL, DebugFont, 5, 36, "FPS: %d/%d (%.1f%%, DT=%.3fms)", FPS, TargetFPS, ((float)FPS / TargetFPS) * 100.0f, DeltaTime);
            
            if (DebugMode == 2)
            {
                T3DVec3 CamForwardVector = GetCameraForwardVector(CamProps.Position, CamProps.Target);
                rdpq_text_printf(NULL, DebugFont, 5, 48, "CAM TGT: %.3f, %.3f, %.3f", CamProps.Target.v[0], CamProps.Target.v[1], CamProps.Target.v[2]);
                rdpq_text_printf(NULL, DebugFont, 5, 60, "CAM POS: %.3f, %.3f, %.3f", CamProps.Position.v[0], CamProps.Position.v[1], CamProps.Position.v[2]);
                rdpq_text_printf(NULL, DebugFont, 5, 72, "UP DIR: %.3f, %.3f, %.3f", CamProps.UpDir.v[0], CamProps.UpDir.v[1], CamProps.UpDir.v[2]);
                rdpq_text_printf(NULL, DebugFont, 5, 84, "CAM FWD: %.3f, %.3f, %.3f", CamForwardVector.v[0], CamForwardVector.v[1], CamForwardVector.v[2]);
                rdpq_text_printf(NULL, DebugFont, 5, 96, "STICK: X=%d || Y=%d", Input.StickState[0], Input.StickState[1]);
                rdpq_text_printf(NULL, DebugFont, 5, 108, "BIndex: %d", BIndex);
            }
        }
        
        EndFrame();

        BIndex = 0;
        /*if (FrameCount % 30 == 0)
        {
            BIndex++;
        }

        if (BIndex > 3)
        {
            BIndex = 0;
        }*/
    }

    t3d_destroy();
    return 0;
}