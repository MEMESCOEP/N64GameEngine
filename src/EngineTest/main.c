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
struct ModelTransform CameraForwardTransform;
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
T3DModel *AxisModel;
T3DModel *BushModel;
T3DModel *GearModel;
T3DVec3 SunDirection = {{-1.0f, 1.0f, 1.0f}};
color_t SkyColor = (color_t){0x94, 0xC4, 0xF2, 0xFF};
uint8_t GlobalLightColor[4] = {0x50, 0x50, 0x64, 0xFF};
uint8_t SunColor[4] = {0xFB, 0xFF, 0xCD, 0xFF};
char *HeadModelPaths[4] = {"rom:/Pikachu.t3dm", "rom:/Mario.t3dm", "rom:/Link.t3dm", "rom:/FoxMcCloud.t3dm"};
char *CameraModeStr = "Orbit";
float FencePositions[4][2] = {{175.0f, 175.0f}, {175.0f, -175.0f}, {-175.0f, -175.0f}, {-175.0f, 175.0f}};
float BushPositions[4][2] = {{175.0f, 175.0f}, {175.0f, -175.0f}, {-175.0f, -175.0f}, {-175.0f, 175.0f}};
float HeadPositions[4][2] = {{175.0f, 175.0f}, {175.0f, -175.0f}, {-175.0f, -175.0f}, {-175.0f, 175.0f}};
float CameraControlSpeed = 100.0f;
float RotationSpeed = 0.25f;
float ModelAngle = 0.0f;
bool DrawAxisModel = false;
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
    // Set the engine's debug mode to output all avaliable debug information
    SetDebugMode(ALL);

    // Initialize the system and Tiny3D
    // We use 320x240@16 here because in this case it strikes a balance between performance and video quality
    InitSystem(RESOLUTION_320x240, DEPTH_16_BPP, 3, FILTERS_RESAMPLE_ANTIALIAS, true);

    debugf("\n[== N64 Game Engine Test Scene ==]\n");
    DebugPrint("[INFO] >> Registering fonts...\n", MINIMAL);
    NewFont = RegisterFont("rom:/DEBUG.font64", 1);
    
    // Set up the camera and the viewport
    DebugPrint("[INFO] >> Creating T3D viewport...\n", MINIMAL);
    Viewport = t3d_viewport_create();

    DebugPrint("[INFO] >> Setting up camera...\n", MINIMAL);
    CamProps = DefaultCameraProperties;
    CamProps.Position = (T3DVec3){{0.0f, -25.0f, 125.0f}};
    CamProps.Target = (T3DVec3){{0.0f, -50.0f, 0.0f}};
    CamProps.FOV = 90.0f;

    // Load models and set up transforms
    DebugPrint("[INFO] >> Loading models and setting up transforms...\n", MINIMAL);
    FloorModel = t3d_model_load("rom:/Floor.t3dm");
    FenceModel = t3d_model_load("rom:/Fence.t3dm");
    AxisModel = t3d_model_load("rom:/XYZ.t3dm");
    BushModel = t3d_model_load("rom:/StretchyBush.t3dm");
    GearModel = t3d_model_load("rom:/Gear.t3dm");

    // Commented out because I haven't yet decided what models to use
    /*for (int ModelIndex = 0; ModelIndex < 4; ModelIndex++)
    {
        HeadModels[ModelIndex] = t3d_model_load(HeadModelPaths[ModelIndex]);
    }*/

    // Used to render the axis (XYZ) model
    CameraForwardTransform = CreateNewModelTransform();
    CameraForwardTransform.Position[0] = CamProps.Target.v[0];
    CameraForwardTransform.Position[1] = CamProps.Target.v[1];
    CameraForwardTransform.Position[2] = CamProps.Target.v[2];
    CameraForwardTransform.Scale[0] = 0.1f;
    CameraForwardTransform.Scale[1] = 0.1f;
    CameraForwardTransform.Scale[2] = 0.1f;

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

    t3d_vec3_norm(&SunDirection);

    // Create transforms for the 4 bush models
    for (int BMIndex = 0; BMIndex < 4; BMIndex++)
    {
        struct ModelTransform NewTransform = CreateNewModelTransform();

        NewTransform.Position[0] = BushPositions[BMIndex][0];
        NewTransform.Position[1] = -100.0f;
        NewTransform.Position[2] = BushPositions[BMIndex][1];
        NewTransform.Scale[0] = 0.5f;
        NewTransform.Scale[1] = 0.5f;
        NewTransform.Scale[2] = 0.5f;

        BushModelTransforms[BMIndex] = NewTransform;
    }

    DebugPrint("[INFO] >> Starting game loop...\n", MINIMAL);

    while (true)
    {
        // Update the viewport (screen projection and camera transform)
        UpdateViewport(&Viewport, CamProps);

        // You should try to update the scene before you start drawing or after the frame ends (where possible) to
        // avoid potential graphical issues. Note that T3D draws asynchronously, so you can't use one matrix to render multiple
        // models if the matrix changes more than once per frame.
        ModelAngle += 1.5f * DeltaTime;
        GearModelTransform1.Rotation[0] = ModelAngle * RotationSpeed;
        GearModelTransform1.Rotation[1] = ModelAngle * RotationSpeed;
        GearModelTransform1.Rotation[2] = ModelAngle * RotationSpeed * 4.0f;
        CameraForwardTransform.Position[0] = CamProps.Target.v[0];
        CameraForwardTransform.Position[1] = CamProps.Target.v[1];
        CameraForwardTransform.Position[2] = CamProps.Target.v[2];

        // Read controller input from port 1 into a buffer
        GetControllerInput(&Input, JOYPAD_PORT_1);

        if (CameraMode == 0)
        {
            // Rotate the camera around the center of the scene, at a speed of 15 degrees per second
            RotateCameraAroundPoint(15.0f * DeltaTime, &CamProps, (T3DVec3){{0.0f, -50.0f, 0.0f}});
        }
        else if (CameraMode == 1)
        {
            // Rotate and move the camera based on joystick / D-PAD input. If the B button is held, pressing the UP and DOWN buttons on
            // the D-PAD will move the camera up and down based on its current up vector instead of forward and backward
            RotateCameraRelative(Input.StickStateNormalized[0] * CameraControlSpeed * DeltaTime, Input.StickStateNormalized[1] * CameraControlSpeed * DeltaTime, 0.0f, &CamProps);
            
            if (Input.HeldButtons.d_up)
            {
                if (Input.HeldButtons.b)
                {
                    MoveCameraVertical(&CamProps, 50.0f * DeltaTime, false);
                }
                else
                {
                    MoveCameraLateral(&CamProps.Position, &CamProps.Target, 50.0f * DeltaTime, false);
                }
            }

            if (Input.HeldButtons.d_down)
            {
                if (Input.HeldButtons.b)
                {
                    MoveCameraVertical(&CamProps, -50.0f * DeltaTime, false);
                }
                else
                {
                    MoveCameraLateral(&CamProps.Position, &CamProps.Target, -50.0f * DeltaTime, false);
                }
            }

            if (Input.HeldButtons.d_left)
            {
                MoveCameraStrafe(&CamProps, -50.0f * DeltaTime, false);
            }

            if (Input.HeldButtons.d_right)
            {
                MoveCameraStrafe(&CamProps, 50.0f * DeltaTime, false);
            }
        }

        // Switch between debug modes
        //  0: No debug info is displayed
        //  1: Minimal debug info is displayed (MEM, Uptime, FPS)
        //  2: All debug info is displayed (Info from 1, Camera properties, stick input)
        if (Input.PressedButtons.z)
        {
            // Toggle the 3D axis model if the B button is held. Otherwise, change the debug mode
            if (Input.HeldButtons.b)
            {
                DrawAxisModel = !DrawAxisModel;
                DebugPrint("[INFO] >> %s drawing of axis model.\n", MINIMAL, DrawAxisModel == true ? "Enabled" : "Disabled");
            }
            else
            {
                DebugMode++;

                if (DebugMode > 2) DebugMode = 0;

                DebugPrint("[INFO] >> Set debug mode to %d.\n", MINIMAL, DebugMode);
            }
        }

        // Switch between camera modes
        //  0: Rotate around the center of the screen
        //  1: Manual control
        //  2: Static
        if (Input.PressedButtons.a)
        {
            CameraMode++;

            if (CameraMode > 2) CameraMode = 0;

            switch (CameraMode)
            {
                case 0:
                    CamProps.Position = (T3DVec3){{0.0f, -25.0f, 125.0f}};
                    CamProps.Target = (T3DVec3){{0.0f, -50.0f, 0.0f}};
                    CameraModeStr = "Orbit";
                    break;

                case 1:
                    CameraModeStr = "Manual";
                    break;

                case 2:
                    MoveCameraToPoint(&CamProps, (T3DVec3){{175.0f, 0.0f, 175.0f}});
                    CamProps.Position = (T3DVec3){{175.0f, 0.0f, 175.0f}};
                    CamProps.Target = (T3DVec3){{0.0f, -50.0f, 0.0f}};
                    CameraModeStr = "Fixed";
                    break;
            }

            DebugPrint("[INFO] >> Set camera mode to \"%s\" (mode %d).\n", MINIMAL, CameraModeStr, CameraMode);
        }

        // Start the frame and enter 3D mode
        // All 3D graphics operations should almost always take place in 3D mode
        StartFrame();
        Start3DMode(&Viewport);        
        ClearScreen(SkyColor);
        UpdateLightProperties(1, GlobalLightColor, SunColor, &SunDirection);

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
        rdpq_set_prim_color(GetRainbowColor(ModelAngle * 0.42f));
        RenderModel(GearModel, &GearModelTransform1, true);

        // Draw the Axis ("XYZ") model if it's enabled. We disable the Z buffer before we draw this so it will appear in top of everything.
        // It's important that we only disable the Z buffer and draw this model AFTER everything else has been drawn, because otherwise
        // everything would be drawn with no depth.
        if (DrawAxisModel == true)
        {
            rdpq_mode_zbuf(false, false);
            RenderModel(AxisModel, &CameraForwardTransform, true);
        }

        // Enter 2D mode. All 2D graphics operations should (usually) take place in 2D mode. 2D text should be drawn after LAST
        // else unless you have a specific reason not to
        Start2DMode();

        // Draw debug statistics
        if (DebugMode > 0)
        {
            rdpq_text_printf(NULL, 1, 5, 12, "MEM USED: %.2f KB", (HeapStats.used / 1024.0f) * 1.024f);
            rdpq_text_printf(NULL, 1, 5, 24, "UPTIME: %.2fs", UptimeMilliseconds() / 1000.0f);
            rdpq_text_printf(NULL, 1, 5, 36, "FPS: %d/%d (%.1f%%, DT=%.3fms)", FPS, TargetFPS, ((float)FPS / TargetFPS) * 100.0f, DeltaTime);
            rdpq_text_printf(NULL, 1, 5, 48, "CAM MODE: %s", CameraModeStr);
            
            if (DebugMode == 2)
            {
                T3DVec3 CamForwardVector = GetForwardVector(CamProps.Position, CamProps.Target);

                rdpq_text_printf(NULL, 1, 5, 60, "STICK NORM: X=%f || Y=%f", Input.StickStateNormalized[0], Input.StickStateNormalized[1]);
                rdpq_text_printf(NULL, 1, 5, 72, "STICK: X=%d || Y=%d", Input.StickState[0], Input.StickState[1]);
                rdpq_text_printf(NULL, 1, 5, 84, "CAM TGT: %.3f, %.3f, %.3f", CamProps.Target.v[0], CamProps.Target.v[1], CamProps.Target.v[2]);
                rdpq_text_printf(NULL, 1, 5, 96, "CAM POS: %.3f, %.3f, %.3f", CamProps.Position.v[0], CamProps.Position.v[1], CamProps.Position.v[2]);
                rdpq_text_printf(NULL, 1, 5, 108, "CAM FWD: %.3f, %.3f, %.3f", CamForwardVector.v[0], CamForwardVector.v[1], CamForwardVector.v[2]);
                rdpq_text_printf(NULL, 1, 5, 120, "CAM UP: %.3f, %.3f, %.3f", CamProps.UpDir.v[0], CamProps.UpDir.v[1], CamProps.UpDir.v[2]);
            }
        }

        EndFrame();
    }

    t3d_destroy();
    return 0;
}
