#ifndef __GF3D_VGRAPHICS_H__
#define __GF3D_VGRAPHICS_H__

#include <vulkan/vulkan.h>
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_pipeline.h"
#include "gf3d_commands.h"

#define GF3D_VGRAPHICS_DISCRETE 2   //Choosing whether to use discrete [1] or integrated graphics [0]

/**
 * @brief init Vulkan / SDL, setup device and initialize infrastructure for 3d graphics
 */
void gf3d_vgraphics_init(
    char *windowName,
    int renderWidth,
    int renderHeight,
    Vector4D bgcolor,
    Bool fullscreen,
    Bool enableValidation
);

/**
 * @brief After initialization 
 */
VkDevice gf3d_vgraphics_get_default_logical_device();

VkPhysicalDevice gf3d_vgraphics_get_default_physical_device();

VkExtent2D gf3d_vgraphics_get_view_extent();

void gf3d_vgraphics_clear();

Uint32 gf3d_vgraphics_render_begin();
void gf3d_vgraphics_render_end(Uint32 imageIndex);

int gf3d_vgraphics_create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer * buffer, VkDeviceMemory * bufferMemory);

void gf3d_vgraphics_copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

uint32_t gf3d_vgraphics_find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);

void gf3d_vgraphics_move_camera(float dir,float speed);

void gf3d_vgraphics_rotate_camera(float degrees);

void gf3d_vgraphics_rotate_camera_axis(float degrees,Vector3D axis);

void gf3d_vgraphics_mouse_look();

/**
 * @brief draws the fullscreen triangle that the raymarch shader is using
 * @Param frame from the frame buffer
 * @Param the command buffer for the pipeline
 * @Parma the pointer for the fullscreen pipeline
 */
void gf3d_vgraphics_draw_fullscreen(Uint32 bufferFrame, VkCommandBuffer commandBuffer, Pipeline* pipe);

/**
 * @brief Updates the UBO (mainly of the camera) sent to the vertex shader for the fullscreen pipeline
 * @Param the descriptorset that needs updating
 * @Param swapchain of the system
 * @Parma ubo of the entity to be rendered (will be removed for now) --> Matrix4 modelMat
 */
void gf3d_vgraphics_update_fullscreen_descriptor_set(VkDescriptorSet descriptorSet, Uint32 chainIndex);
/**
 * @brief allocates memory for the uniform buffers for the fullscreen pipeline
 */
void gf3d_fullscreen_create_uniform_buffer();

/*breif sets the camera position*/
void gf3d_vgraphics_set_camera(Vector3D pos);

void gf3d_vgraphics_set_resolution(Vector2D res);

VkBuffer gf3d_vgraphics_get_uniform_buffer_by_index(Uint32 index);

UniformBufferObject gf3d_vgraphics_get_uniform_buffer_object();

UniformBufferObject* gf3d_get_pointer_to_UBO();

Pipeline *gf3d_vgraphics_get_graphics_pipeline();
Pipeline* gf3d_vgraphics_get_graphics_overlay_pipeline();

Command *gf3d_vgraphics_get_graphics_command_pool();

VkImageView gf3d_vgraphics_create_image_view(VkImage image, VkFormat format);


#endif
