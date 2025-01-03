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
#include "../ColorUtils.h"
#include "../MathUtils.h"
#include "../TextUtils.h"
#include "../Globals.h"


/* VARIABLES */
struct CameraProperties CamProps;
struct ControllerState Input;
struct ModelTransform CameraForwardTransform;
struct ModelTransform FenceModelTransforms[4];
struct ModelTransform BushModelTransforms[4];
struct ModelTransform HeadModelTransforms[4];
struct ModelObject FloorObject;
struct ModelObject N64Object;
T3DViewport Viewport;
rdpq_font_t* DebugFont;
rdpq_font_t* CamFont;
T3DModel* HeadModels[4];
T3DModel* AxisModel;
T3DModel* BushModel;
T3DVec3 CamForwardDirection;
T3DVec3 SunDirection = {{-1.0f, 1.0f, 1.0f}};
color_t SkyColors[3] = {(color_t){0x94, 0xC4, 0xF2, 0xFF}, (color_t){0x45, 0x4A, 0x73, 0xFF}, (color_t){0x0A, 0x09, 0x13, 0xFF}};
color_t CamModeColor;
color_t SkyColor = (color_t){0x94, 0xC4, 0xF2, 0xFF};
uint8_t SunColors[3][4] = {{0xFB, 0xFF, 0xCD, 0xFF}, {0x4E, 0x54, 0x82, 0xFF}, {0x1D, 0x19, 0x36, 0xFF}};
uint8_t GlobalLightColor[4] = {0x50, 0x50, 0x64, 0xFF};
uint8_t SunColor[4] = {0xFB, 0xFF, 0xCD, 0xFF};
char* HeadModelPaths[4] = {"rom:/Pikachu.t3dm", "rom:/Mario.t3dm", "rom:/Link.t3dm", "rom:/FoxMcCloud.t3dm"};
char* CamModeDisplayText = "-- CAMERA MODE --";
char* CameraModeStr = "Orbit";
float FencePositions[4][2] = {{175.0f, 175.0f}, {175.0f, -175.0f}, {-175.0f, -175.0f}, {-175.0f, 175.0f}};
float BushPositions[4][2] = {{175.0f, 175.0f}, {175.0f, -175.0f}, {-175.0f, -175.0f}, {-175.0f, 175.0f}};
float HeadPositions[4][2] = {{175.0f, 175.0f}, {175.0f, -175.0f}, {-175.0f, -175.0f}, {-175.0f, 175.0f}};
float CameraControlSpeed = 50.0f;
float InstalledMemoryKB = 0.0f;
float CamTextTimeLeft = 0.0f;
float SkyLerpProgress = 0.0f;
float RotationSpeed = 0.15f;
float ModelAngle = 0.0f;
bool DrawAxisModel = false;
bool ShowCamMode = false;
int PreviousTimeColor = 0;
int CameraMode = 0;
int DebugMode = 1;
int TimeColor = 1;


/* FUNCTIONS */
int main()
{
    // Set the engine's debug mode to output all available debug information
    SetDebugMode(ALL);

    // Initialize the system and Tiny3D. 320x240@16 mode is used here because it strikes a balance between performance and
    // video quality in this case. The display is also instructed to use resample antialiasing, with 1 buffer.
    InitSystem(RESOLUTION_320x240, DEPTH_16_BPP, 2, FILTERS_RESAMPLE_ANTIALIAS, true);

    debugf("\n[== N64 Game Engine Test Scene ==]\n");
    InstalledMemoryKB = HeapStats.total / 1024.0f;

    DebugPrint("[INFO] >> Registering fonts...\n", MINIMAL);
    DebugFont = RegisterFontBasic("rom:/DEBUG.font64", COLOR_WHITE, COLOR_TRANSPARENT, 1);
    CamFont = RegisterFontBasic("rom:/DEBUG.font64", COLOR_WHITE, COLOR_TRANSPARENT, 2);
    
    // Set up the camera and the viewport
    DebugPrint("[INFO] >> Creating T3D viewport...\n", MINIMAL);
    Viewport = t3d_viewport_create();

    DebugPrint("[INFO] >> Setting up camera...\n", MINIMAL);
    CamProps = DefaultCameraProperties;
    CamProps.Position = (T3DVec3){{0.0f, -25.0f, 125.0f}};
    CamProps.Target = (T3DVec3){{0.0f, -50.0f, 0.0f}};
    CamProps.FOV = 90.0f;

    // Load models and set up transforms
    // This is commented out because I haven't yet decided what models to use
    /*for (int ModelIndex = 0; ModelIndex < 4; ModelIndex++)
    {
        HeadModels[ModelIndex] = t3d_model_load(HeadModelPaths[ModelIndex]);
    }*/

    DebugPrint("[INFO] >> Loading models...\n", MINIMAL);
    CreateNewModelObject(&FloorObject, "rom:/Floor.t3dm");
    CreateNewModelObject(&N64Object, "rom:/N64.t3dm");

    AxisModel = t3d_model_load("rom:/XYZ.t3dm");
    BushModel = t3d_model_load("rom:/StretchyBush.t3dm");

    DebugPrint("[INFO] >> Setting up transforms...\n", MINIMAL);

    // Used to render the axis (XYZ) model
    CameraForwardTransform = CreateNewModelTransform();
    CameraForwardTransform.Position = CamProps.Target;
    CameraForwardTransform.Scale = (T3DVec3){{0.1f, 0.1f, 0.1f}};

    // This scale may need to be tweaked to prevent "jitter" and texture distortion
    FloorObject.Transform.Position = (T3DVec3){{0.0f, -100.0f, 0.0f}};
    FloorObject.Transform.Scale = (T3DVec3){{0.75f, 1.0f, 0.75f}};

    N64Object.Transform.Position.v[1] = -40.0f;
    N64Object.Transform.Scale = (T3DVec3){{0.075f, 0.075f, 0.075f}};

    t3d_vec3_norm(&SunDirection);

    // Create transforms for the 4 bush models
    for (int BMIndex = 0; BMIndex < 4; BMIndex++)
    {
        struct ModelTransform NewTransform = CreateNewModelTransform();

        NewTransform.Position = (T3DVec3){{BushPositions[BMIndex][0], -100.0f, BushPositions[BMIndex][1]}};
        NewTransform.Scale = (T3DVec3){{0.5f, 0.5f, 0.5f}};

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
        CamForwardDirection = CamProps.ForwardVector;
        N64Object.Transform.Rotation.v[0] = ModelAngle * -RotationSpeed;
        N64Object.Transform.Rotation.v[1] = ModelAngle * -RotationSpeed;

        ScaleFloat3(CamForwardDirection.v, 100.0f);
        t3d_vec3_add(&CameraForwardTransform.Position, &CamProps.Target, &CamForwardDirection);

        // Slowly fade the sky and global light colors to sunset
        Lerp1DUint8Array(SunColor, SunColors[PreviousTimeColor], SunColors[TimeColor], 4, SkyLerpProgress);
        LerpColor(&SkyColor, SkyColors[PreviousTimeColor], SkyColors[TimeColor], SkyLerpProgress);
        SkyLerpProgress += 0.025f * DeltaTime;

        if (SkyLerpProgress > 1.0f)
        {
            PreviousTimeColor++;
            SkyLerpProgress = 0.0f;
            TimeColor++;
            
            if (TimeColor > 2)
            {
                PreviousTimeColor = 2;
                TimeColor = 0;
            }
            else if (PreviousTimeColor > 2)
            {
                PreviousTimeColor = 0;
            }
        }

        DebugPrint("Current: %C || Target: %C || Time value: %f\n", ALL, SkyColor, SkyColors[TimeColor], SkyLerpProgress);

        // Read input from controller port 1 into a buffer
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
            //
            // -- BUG --
            // There is a bug somewhere that makes the camera rotate to 90 degrees downward every time the camera mode is switched to
            // manual control mode. The controls still work fine, the camera just starts off looking down
            RotateCameraRelative(Input.StickStateNormalized[0] * CameraControlSpeed * DeltaTime, Input.StickStateNormalized[1] * CameraControlSpeed * DeltaTime, 0.0f, &CamProps);
            
            if (Input.HeldButtons.d_up)
            {
                if (Input.HeldButtons.b)
                {
                    MoveCameraVertical(&CamProps, 50.0f * DeltaTime, false);
                }
                else
                {
                    MoveCameraLateral(&CamProps, 50.0f * DeltaTime, false);
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
                    MoveCameraLateral(&CamProps, -50.0f * DeltaTime, false);
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

        // Toggle the 3D axis model if the B button is held. Otherwise, change the debug mode
        //  0: No debug info is displayed
        //  1: Minimal debug info is displayed (MEM, Uptime, FPS)
        //  2: All debug info is displayed (Info from 1, Camera properties, stick input)
        if (Input.PressedButtons.z)
        {
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
            CamTextTimeLeft = 2.25f;
            CamModeColor = COLOR_WHITE;
            ShowCamMode = true;
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
        RenderModel(FloorObject, true);
        RenderModel(N64Object, true);
        
        for (int ModelIndex = 0; ModelIndex < 4; ModelIndex++)
        {
            //RenderModel(FenceModel, &FenceModelTransforms[ModelIndex], true);
            RenderModelWithTransform(BushModel, &BushModelTransforms[ModelIndex], true);
        }

        // Draw the Axis ("XYZ") model if it's enabled. The depth buffer is cleared before the model is rendered so it will appear in top of
        // everything. It's important that you only clear the depth buffer and draw this model AFTER everything else has been drawn, because
        // otherwise everything would be drawn with no depth. Z sorting is enabled because that causes an issue when rendering the model
        // where the X axis arrow would be visible through the Z axis arrow.
        if (DrawAxisModel == true)
        {
            t3d_screen_clear_depth();
            RenderModelWithTransform(AxisModel, &CameraForwardTransform, true);
        }

        // Enter 2D mode. In this mode, all 2D graphics are displayed on top of the 3D view. This makes it easier to add and show UI elements
        // and text without interfering with the 3D scene. Note that you should enter 2D mode after 3D mode (if you are using 3D) to ensure
        // that 2D graphics are drawn last.
        Start2DMode();

        if (ShowCamMode == true)
        {
            if (CamTextTimeLeft <= 0.0f)
            {
                if (CamModeColor.a <= 10)
                {
                    ShowCamMode = false;    
                }
                
                FadeAlpha(&CamModeColor, 0, 7.5f * DeltaTime);
            }

            CamTextTimeLeft -= 1.0f * DeltaTime; 
            
            rdpq_font_style(CamFont, 0, &(rdpq_fontstyle_t){
                .color = CamModeColor,
            });

            rdpq_text_printf(NULL, 2, 160 - (PixelStrWidth(CamModeDisplayText, 1) / 2.0f), 180, CamModeDisplayText);
            rdpq_text_printf(NULL, 2, 160 - (PixelStrWidth(CameraModeStr, 1) / 2.0f), 195, CameraModeStr);
        }

        if (DebugMode > 0)
        {
            rdpq_text_printf(NULL, 1, 5, 12, "MEM USED: %.2f / %.2f KB (%.2f%%)", HeapStats.used / 1024.0f, InstalledMemoryKB, UsedMemPercentage * 100.0f);
            rdpq_text_printf(NULL, 1, 5, 24, "UPTIME: %.2fs", UptimeMilliseconds() / 1000.0f);
            rdpq_text_printf(NULL, 1, 5, 36, "FPS: %.2f/%.2f (%.1f%%, DT=%.3fms)", FPS, TargetFPS, ((float)FPS / TargetFPS) * 100.0f, DeltaTime);
            
            if (DebugMode == 2)
            {
                rdpq_text_printf(NULL, 1, 5, 48, "STICK NORM: X=%f || Y=%f", Input.StickStateNormalized[0], Input.StickStateNormalized[1]);
                rdpq_text_printf(NULL, 1, 5, 60, "STICK: X=%d || Y=%d", Input.StickState[0], Input.StickState[1]);
                rdpq_text_printf(NULL, 1, 5, 72, "CAM TGT: %.3f, %.3f, %.3f", CamProps.Target.v[0], CamProps.Target.v[1], CamProps.Target.v[2]);
                rdpq_text_printf(NULL, 1, 5, 84, "CAM POS: %.3f, %.3f, %.3f", CamProps.Position.v[0], CamProps.Position.v[1], CamProps.Position.v[2]);
                rdpq_text_printf(NULL, 1, 5, 96, "CAM FWD: %.3f, %.3f, %.3f", CamProps.ForwardVector.v[0], CamProps.ForwardVector.v[1], CamProps.ForwardVector.v[2]);
                rdpq_text_printf(NULL, 1, 5, 108, "CAM RGT: %.3f, %.3f, %.3f", CamProps.RightVector.v[0], CamProps.RightVector.v[1], CamProps.RightVector.v[2]);
                rdpq_text_printf(NULL, 1, 5, 120, "CAM UP: %.3f, %.3f, %.3f", CamProps.UpVector.v[0], CamProps.UpVector.v[1], CamProps.UpVector.v[2]);
            }
        }
        
        EndFrame(&CamProps);
    }

    t3d_destroy();
    return 0;
}
