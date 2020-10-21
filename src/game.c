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
    
    init_logger("gf3d.log");    
    slog("gf3d begin");
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        1200,                   //screen width
        700,                    //screen height
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
            vector3d(10,0,0)
        );
    //NEW CODE BELOW 
    
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
    
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    Pipeline *fullscreenpipe = gf3d_pipeline_fullscreen_create(device, "shaders/fullscreen.spv", "shaders/RayMarch.spv", gf3d_vgraphics_get_view_extent(), 1024);
    gf3d_swapchain_setup_frame_buffers(fullscreenpipe);
    gf3d_fullscreen_create_uniform_buffer();
    VkCommandBuffer fullscreenCmd;
    //bufferFrame = gf3d_vgraphics_render_begin();
    //fullscreenCmd = gf3d_command_rendering_fullscreen_begin(bufferFrame, fullscreenpipe); //Already binds pipeline, no need to do it again.
    
    
    //NEW CODE OVER
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        //update game things here
        
        gf3d_vgraphics_rotate_camera(0.01);
        
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
        
        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        bufferFrame = gf3d_vgraphics_render_begin();
        gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_pipeline(),bufferFrame);
        gf3d_pipeline_reset_frame(fullscreenpipe, bufferFrame);
            commandBuffer = gf3d_command_rendering_begin(bufferFrame);
                //new draw code start
                //gf3d_pipeline_reset_frame(fullscreenpipe, bufferFrame);
            gf3d_model_draw(model2, bufferFrame, commandBuffer, modelMat2);
            gf3d_command_rendering_end(commandBuffer);

            //Will replace the draw from the previous command. need to figure out how to combine the two
            fullscreenCmd = gf3d_command_rendering_fullscreen_begin(bufferFrame, fullscreenpipe);

                //vkCmdBindPipeline(fullscreenCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, fullscreenpipe);
                //vkCmdDraw(fullscreenCmd, 3, 1, 0, 0);
                gf3d_vgraphics_draw_fullscreen(bufferFrame, fullscreenCmd, fullscreenpipe);
                gf3d_command_rendering_end(fullscreenCmd);
                
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
