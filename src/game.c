#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_physics.h"
#include "gf3d_player.h"
#include "gf3d_sprite.h"
#include "gfc_audio.h"
#include "gf3d_editor.h"
#include <time.h> 

#define FPS 30
#define MPF 1/FPS * 100
#define GetCurrentDir _getcwd
#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>

int main(int argc,char *argv[])
{

    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    printf("Current working dir: %s\n", buff);


    int done = 0;
    int a;
    Uint8 validate = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    Model *model;
    Matrix4 modelMat;
    Model *model2;
    Matrix4 modelMat2;
    
    //Current Validation layers are slowing down program! Need to fix this! (If run in Git bash this isnt a problem)
    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"-disable_validate") == 0)
        {
            validate = 0;
        }
    }
    //800 600 Main screen size
    init_logger("gf3d.log");    
    slog("gf3d begin");
    gf3d_vgraphics_init( //1200 700 (Old resolution: WARNING WILL MAKE THE GAME RUN MUCH SLOWER)
        "gf3d",                 //program name
        800,                   //screen width
        600,                    //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        validate                //validation
    ); //This function has been edited slightly
    
    // main game loop
    slog("gf3d main loop begin");
    model = gf3d_model_load("dino"); 
    gfc_matrix_identity(modelMat);
    model2 = gf3d_model_load("dino");
    gfc_matrix_identity(modelMat2);
    gfc_matrix_make_translation(
            modelMat2,
            vector3d(0,1,0)
        );
    //NEW CODE BELOW 
    gf3d_camera_set_position(vector3d(0, 0, 1));
    gf3d_vgraphics_set_camera(vector3d(0, 1, 1));
    Model *model3 = gf3d_model_load("cube");
    Matrix4 modelMat3;
    gfc_matrix_identity(modelMat3);
    gfc_matrix_make_translation(
        modelMat3,
        vector3d(0, 10, 0)
    );
 
        //Scales up the model
    modelMat3[0][0] *= 2;
    modelMat3[1][1] *= 2;
    modelMat3[2][2] *= 2;
    //Setup descripterSets for the pipleine so that we can use the UBO
    
    initEntList();
    phyEngine_init();
    //playerManInit();
    //addEntity(vector4d(0, 1, -5, 1), vector4d(0, 0, 0, 1), vector4d(1, 1, 1, 1), vector4d(0, 1, 0, 1),vector3d(-1, 0 , 0), 0,0);
    //addEntity(vector4d(-1, 2, 1, 1), vector4d(35, 0, 0, 1), vector4d(1, 1, 1, 1), vector4d(0, 1, 0, 1), vector3d(0, -0.1, 0), 1,0);
    //addEntity(vector4d(-1, 1, 1, 1), vector4d(0, 0, 0, 1), vector4d(1, 1, 1, 1), vector4d(0, 1, 0, 1), vector3d(0, 0, 0), 1, 0);
    //Entity *ground = addEntity(vector4d(0, -0.25, 0, 1), vector4d(0, 0, 0, 1), vector4d(50, 1, 50, 1), vector4d(0, 0, 0.5, 1), vector3d(0, 0, 0), 1,0);
    
    //ground->pSelf->mass = 0;
    //ground->pSelf->friction = 0;
    //addWalls();
     //move to player init
    
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    Pipeline *fullscreenpipe = gf3d_pipeline_fullscreen_create(device, "shaders/fullscreen.spv", "shaders/RayMarch.spv", gf3d_vgraphics_get_view_extent(), 1024);
    gf3d_swapchain_setup_frame_buffers(fullscreenpipe);
    gf3d_fullscreen_create_uniform_buffer();
    VkCommandBuffer fullscreenCmd;

    //UI START
    Sprite *hud = gf3d_sprite_load("images/hud.png", -1, -1, 0);
    Sprite* bg = gf3d_sprite_load("images/Main_Screen.png", -1, -1, 0);



    //UI END
    //Sound Start
    gfc_audio_init(10, 1, 1, 10, true, true);
    Sound *s = gfc_sound_load("sounds/Homestuck - The Felt - 05 Clockwork Reversal.wav", .1, 1);
    gfc_sound_play(s, 99, 0.1, -1, -1);
    //Sound End

    //Editor Start
    editorInit();
    loadLevel();
    //Editor End

    //bufferFrame = gf3d_vgraphics_render_begin();
    //fullscreenCmd = gf3d_command_rendering_fullscreen_begin(bufferFrame, fullscreenpipe); //Already binds pipeline, no need to do it again.
    int spriteMode = true;
    int editMode = false;
    int editInit = false;
    double last = time(NULL);
    float fps = 0;
    float totalF = 0;
    float updateF = 0;
    float totalTime = 0;
    float updateTime = 0;

    float angle = 0;
    //NEW CODE OVER
    while(!done)
    {
        
        totalF++;
        updateF++;
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        if (keys[SDL_SCANCODE_RETURN]) spriteMode = false;
        if (keys[SDL_SCANCODE_E] && spriteMode) {
            spriteMode = false;
            editMode = true;
        }
        if (spriteMode) {
            double currentS = time(NULL);
            double deltaS = currentS - last;
            totalTime += deltaS;
            last = currentS;
        }
        if (editMode) {
            if (!editInit) {
                tabSelect(1);
                //Move camera into place
                gf3d_vgraphics_set_camera(vector3d(14, 19, 0));
                //gf3d_vgraphics_rotate_camera(180);
                angle = -90;
                angle /= 57.2957795;
                gf3d_vgraphics_rotate_camera(angle);
                angle = 45;
                angle /= 57.2957795;
                gf3d_vgraphics_rotate_camera_axis(angle,vector3d(1,0,0));
                saveLevel();
                editInit = true;
            }
            //angle += 0.01;
            //gf3d_vgraphics_rotate_camera_axis(0.01, vector3d(1, 0, 0));
            editorUpdate();
        }
        if (!spriteMode && !editMode) {
            //update game things here
        
            //gf3d_vgraphics_rotate_camera(0.01);
            //gf3d_vgraphics_mouse_look();
            float speed = 1;
            if (keys[SDL_SCANCODE_LSHIFT]) speed = 5;
            if(keys[SDL_SCANCODE_D]) gf3d_vgraphics_rotate_camera(0.01*speed);
            else if(keys[SDL_SCANCODE_A]) gf3d_vgraphics_rotate_camera(-0.01*speed);
            if (keys[SDL_SCANCODE_S]) {
                gf3d_vgraphics_move_camera(-1,0.01*speed);
            }
            else if (keys[SDL_SCANCODE_W]) {
                gf3d_vgraphics_move_camera(1,0.01*speed);
            }
            if (keys[SDL_SCANCODE_SPACE]) {
                RaycastResult result;
                get_RaycastAhead(&result);
                if (result.hit) {
                    Vector3D in = result.point;
                    //vector3d_add(in, result.point, -2.5 * result.normal);
                   addEmpty(in);
               
                }
            }
        
            gfc_matrix_rotate(
                modelMat,
                modelMat,
                0.002,
                vector3d(1,0,0));
            gfc_matrix_rotate(
                modelMat2,
                modelMat2,
                0.002,
                vector3d(0,0,1));

            double current = time(NULL);
            double delta = current - last;
            totalTime += delta;
            updateTime += delta;
            //fps = totalF / totalTime;
            //if ((int)totalTime % 200000000) printf("CURRENT FPS: %f\n", fps);
            if (updateTime > 2) {
                fps = updateF / updateTime;
                updateF = 0;
                updateTime = 0;
                printf("CURRENT FPS: %f\n", fps);
            }
            //Level update section
            //EntityThink
            //pre player sync
            //physicsUpdate(0.05);//PhyUpdate 
            playerUpdate();
            //post player sync
            updateEntAll();
            last = current;
        }
        //Level Draw
        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        bufferFrame = gf3d_vgraphics_render_begin();
        //gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_pipeline(),bufferFrame);
        gf3d_pipeline_reset_frame(fullscreenpipe, bufferFrame);
        gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_overlay_pipeline(), bufferFrame);
            //commandBuffer = gf3d_command_rendering_begin(bufferFrame);
                //new draw code start
                //gf3d_pipeline_reset_frame(fullscreenpipe, bufferFrame);
           // gf3d_model_draw(model2, bufferFrame, commandBuffer, modelMat2);
           // gf3d_command_rendering_end(commandBuffer);
             
            //Main graphics rendering
        
           
                fullscreenCmd = gf3d_command_rendering_fullscreen_begin(bufferFrame, fullscreenpipe);
                
                    gf3d_vgraphics_draw_fullscreen(bufferFrame, fullscreenCmd, fullscreenpipe);

                gf3d_command_rendering_end(fullscreenCmd);


          
          //Main Grpahics rending end
                if(spriteMode){
                    fullscreenCmd = gf3d_command_rendering_fullscreen_begin(bufferFrame, gf3d_vgraphics_get_graphics_overlay_pipeline());

                    gf3d_sprite_draw(bg, vector2d(0, 0), vector2d(1, 1), 0, bufferFrame, fullscreenCmd);
                    //gf3d_sprite_draw(hud, vector2d(50, 0), vector2d(1, 1), 0, bufferFrame, commandBuffer);

                    gf3d_command_rendering_end(fullscreenCmd);
                }
                
                
                
                //new draw code end
                //gf3d_model_draw(model,bufferFrame,commandBuffer,modelMat);
                //gf3d_model_draw(model2,bufferFrame,commandBuffer,modelMat2);
                //gf3d_model_draw(model3, bufferFrame, commandBuffer, modelMat3);
                
            //gf3d_command_rendering_end(commandBuffer);
            
        gf3d_vgraphics_render_end(bufferFrame);
        

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
