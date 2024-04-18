#pragma once

#include "GraphicsEngine.h"
#include "VertexBuffer.h" 
#include "Vector3D.h" 

class SparkEffect
{
public:
    SparkEffect(GraphicsEngine* graphics_engine);
    bool init();
    void update(float delta_time);
    void render();

private:
    GraphicsEngine* m_graphics_engine;
    VertexShader* m_vertex_shader;
    PixelShader* m_pixel_shader;
    ID3DBlob* m_vertex_shader_bytecode;
    ID3DBlob* m_pixel_shader_bytecode;
    size_t m_vertex_shader_size;
    size_t m_pixel_shader_size;
    VertexBuffer* m_vertex_buffer;
    ID3D11ShaderResourceView* m_texture;
    float m_elapsed_time;
    int m_current_sprite_index;

    bool compileShaders();
    bool initVertexBuffer();
    bool initTexture();
};
