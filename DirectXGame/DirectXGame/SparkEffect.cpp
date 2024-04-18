#include "SparkEffect.h"
#include "VertexBuffer.h" 
#include "GraphicsEngine.h" 

SparkEffect::SparkEffect(GraphicsEngine* graphics_engine) : m_graphics_engine(graphics_engine), m_elapsed_time(0.0f), m_current_sprite_index(0)
{
}

bool SparkEffect::init()
{
    if (!compileShaders())
        return false;

    if (!initVertexBuffer())
        return false;

    if (!initTexture())
        return false;

    return true;
}

void SparkEffect::update(float delta_time)
{
    m_elapsed_time += delta_time;

    if (m_elapsed_time >= 0.1f)
    {
        m_current_sprite_index++;
        if (m_current_sprite_index >= 16)
            m_current_sprite_index = 0;
        m_elapsed_time = 0.0f;
    }
}

void SparkEffect::render()
{
    m_graphics_engine->getImmediateDeviceContext()->setVertexShader(m_vertex_shader);
    m_graphics_engine->getImmediateDeviceContext()->setPixelShader(m_pixel_shader);

    m_graphics_engine->getImmediateDeviceContext()->setTexture(m_texture);

    m_graphics_engine->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);
    m_graphics_engine->getImmediateDeviceContext()->drawTriangleList(m_vertex_buffer->getSizeVertexList(), 0);
}

bool SparkEffect::compileShaders()
{
    if (!m_graphics_engine->compileVertexShader(L"SparkVertexShader.hlsl", "main", &m_vertex_shader_bytecode, &m_vertex_shader_size))
        return false;

    if (!m_graphics_engine->compilePixelShader(L"SparkPixelShader.hlsl", "main", &m_pixel_shader_bytecode, &m_pixel_shader_size))
        return false;

    m_vertex_shader = m_graphics_engine->createVertexShader(m_vertex_shader_bytecode, m_vertex_shader_size);
    if (!m_vertex_shader)
        return false;

    m_pixel_shader = m_graphics_engine->createPixelShader(m_pixel_shader_bytecode, m_pixel_shader_size);
    if (!m_pixel_shader)
        return false;

    return true;
}

bool SparkEffect::initVertexBuffer()
{
    Vector3D vertices[] = {
        Vector3D(-0.5f, -0.5f, 0.0f),
        Vector3D(0.5f, -0.5f, 0.0f),
        Vector3D(0.5f, 0.5f, 0.0f),
        Vector3D(-0.5f, -0.5f, 0.0f),
        Vector3D(0.5f, 0.5f, 0.0f),
        Vector3D(-0.5f, 0.5f, 0.0f)
    };

    m_vertex_buffer = m_graphics_engine->createVertexBuffer();
    if (!m_vertex_buffer)
        return false;

    if (!m_vertex_buffer->load(vertices, sizeof(Vector3D), ARRAYSIZE(vertices), m_vertex_shader_bytecode, m_vertex_shader_size))
        return false;

    return true;
}

bool SparkEffect::initTexture()
{
    if (!m_graphics_engine->createTextureFromFile(L"Spark02.png", &m_texture))
        return false;

    return true;
}
