// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "RenderManager.h"
#include "Engine/ECS/ECSTypes.h"
#include "Engine/Platform/DirectXConversion.h"

#include <DxLib.h>
#include <cassert>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")


struct RenderManager::CameraBuffer
{
    DirectX::XMFLOAT4X4 viewProjection;
    DirectX::XMFLOAT3 position;
    float padding;
};

struct RenderManager::LightBuffer
{
    DirectX::XMFLOAT3 direction;
    float ambient;
    DirectX::XMFLOAT4 lightcolor;
    DirectX::XMFLOAT4 fillcolor;
};

RenderManager::RenderManager() = default;
RenderManager::~RenderManager()
{
    Shutdown();
}


bool RenderManager::Init()
{
    m_device = (ID3D11Device*)GetUseDirect3D11Device();
    m_context = (ID3D11DeviceContext*)GetUseDirect3D11DeviceContext();

    if (!m_device || !m_context) {
        return false;
    }

    // Initialize all required GPU resources
    if (!CreateInstanceBuffer())      return false;
    if (!CreateCameraBuffer())        return false;
    if (!CreateLightBuffer())         return false;
    if (!CreateDepthStencilState())   return false;
    if (!CreateBlendState())          return false;
    if (!LoadShaders())               return false;

    return true;
}


void RenderManager::Shutdown()
{
    // Safely release all COM objects
    m_instanceBuffer.Reset();
    m_cameraBuffer.Reset();
    m_lightBuffer.Reset();
    m_vertexShader.Reset();
    m_inputLayout.Reset();
    m_depthStencilState.Reset();
    m_opaqueBlendState.Reset();

    for (auto& pair : m_pixelShader) {
        pair.second.Reset();
    }
    m_pixelShader.clear();

    for (auto& mesh : m_meshes) {
        mesh.second.vertexBuffer.Reset();
        mesh.second.indexBuffer.Reset();
    }
    m_meshes.clear();
}


void RenderManager::UpdateLight(const Vector3& direction, float ambient, const Color& lightColor, const Color& fillColor)
{
    if (!m_lightBuffer) {
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = m_context->Map(m_lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (SUCCEEDED(hr)) {
        LightBuffer* lb = (LightBuffer*)mapped.pData;
        lb->direction = ToXMFloat3(direction);
        lb->ambient = ambient;
        lb->lightcolor = ToXMFloat4(lightColor);
        lb->fillcolor = ToXMFloat4(fillColor);
        m_context->Unmap(m_lightBuffer.Get(), 0);
    }
}


void RenderManager::UpdateCamera(const Matrix4& viewProjection, const Vector3& position)
{
    if (!m_cameraBuffer) {
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = m_context->Map(m_cameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (SUCCEEDED(hr)) {
        CameraBuffer* cb = (CameraBuffer*)mapped.pData;
        cb->viewProjection = ToDXFloat4(viewProjection);
        cb->position = ToXMFloat3(position);
        m_context->Unmap(m_cameraBuffer.Get(), 0);
    }
}


MeshID RenderManager::CreateMesh(const MeshSourceData& data)
{
    
    if (data.vertices.empty() || data.indices.empty() || !m_device) {
        return INVALID_MESH_ID;
    }

    GPUMeshData mesh;
    mesh.indexCount = static_cast<UINT>(data.indices.size());

    if (!CreateVertexBuffer(mesh, data.vertices)) {
        return INVALID_MESH_ID;
    }

    if (!CreateIndexBuffer(mesh, data.indices)) {
        return INVALID_MESH_ID;
    }

    // Register the newly created GPU buffers and return the handle
    MeshID newId = m_nextMeshId++;
    m_meshes[newId] = mesh;

    return newId;
}


void RenderManager::DrawInstanced(MeshID meshId, const std::vector<InstanceData>& instanceData, PixelShaderType shaderType)
{
    if (instanceData.empty() || instanceData.size() > MAX_ENTITIES) {
        return;
    }

    auto it = m_meshes.find(meshId);
    if (it == m_meshes.end()) {
        return;
    }

    const GPUMeshData& mesh = it->second;
    UINT instanceCount = static_cast<UINT>(instanceData.size());

    // 1. Upload dynamic instance data to the GPU (Map/Unmap)
    D3D11_MAPPED_SUBRESOURCE mappedInstanceBuffer;
    HRESULT hr = m_context->Map(m_instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedInstanceBuffer);
    if (SUCCEEDED(hr)) {
        memcpy(mappedInstanceBuffer.pData, instanceData.data(), sizeof(InstanceData) * instanceCount);
        m_context->Unmap(m_instanceBuffer.Get(), 0);
    }

    // 2. Bind Constant Buffers to the Shader Stages
    m_context->VSSetConstantBuffers(0, 1, m_cameraBuffer.GetAddressOf());
    m_context->PSSetConstantBuffers(1, 1, m_lightBuffer.GetAddressOf());

    // 3. Setup the Rendering Pipeline State
    m_context->IASetInputLayout(m_inputLayout.Get());
    m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_context->PSSetShader(m_pixelShader[shaderType].Get(), nullptr, 0);

    m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    UINT sampleMask = 0xffffffff;
    m_context->OMSetBlendState(m_opaqueBlendState.Get(), blendFactor, sampleMask);

    // Unbind unnecessary shader stages
    m_context->GSSetShader(nullptr, nullptr, 0);
    m_context->HSSetShader(nullptr, nullptr, 0);
    m_context->DSSetShader(nullptr, nullptr, 0);

    // 4. Bind Vertex and Instance buffers to the Input Assembler
    ID3D11Buffer* vertexBuffers[2] = { mesh.vertexBuffer.Get(), m_instanceBuffer.Get() };
    UINT strides[2] = { sizeof(Vertex3D) , sizeof(InstanceData) };
    UINT offsets[2] = { 0, 0 };
    m_context->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);

    m_context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 5. Issue the Hardware Instanced Draw Call
    m_context->DrawIndexedInstanced(mesh.indexCount, instanceCount, 0, 0, 0);

    // Refresh DxLib's internal state since we manually modified the D3D11 context
    RefreshDxLibDirect3DSetting();
}


bool RenderManager::CreateVertexBuffer(GPUMeshData& mesh, const std::vector<Vertex3D>& vertices)
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.ByteWidth = sizeof(Vertex3D) * static_cast<UINT>(vertices.size());
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

    HRESULT hr = m_device->CreateBuffer(&desc, &initData, mesh.vertexBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}


bool RenderManager::CreateIndexBuffer(GPUMeshData& mesh, const std::vector<uint32_t>& indices)
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.ByteWidth = sizeof(uint32_t) * mesh.indexCount;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = indices.data();

    HRESULT hr = m_device->CreateBuffer(&desc, &initData, mesh.indexBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}


bool RenderManager::CreateInstanceBuffer()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(InstanceData) * MAX_ENTITIES;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_device->CreateBuffer(&desc, nullptr, m_instanceBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}


bool RenderManager::CreateCameraBuffer()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(CameraBuffer);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_device->CreateBuffer(&desc, nullptr, m_cameraBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}


bool RenderManager::CreateLightBuffer()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(LightBuffer);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_device->CreateBuffer(&desc, nullptr, m_lightBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}


bool RenderManager::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC desc = {};
    desc.DepthEnable = TRUE;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    desc.StencilEnable = FALSE;
    desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    HRESULT hr = m_device->CreateDepthStencilState(&desc, m_depthStencilState.GetAddressOf());
    return SUCCEEDED(hr);
}


bool RenderManager::CreateBlendState()
{
    D3D11_BLEND_DESC desc = {};
    ZeroMemory(&desc, sizeof(desc));

    // Disable blending and allow full RGBA color writing for Opaque geometry
    desc.RenderTarget[0].BlendEnable = FALSE;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HRESULT hr = m_device->CreateBlendState(&desc, m_opaqueBlendState.GetAddressOf());
    return SUCCEEDED(hr);
}


bool RenderManager::LoadShaders()
{
    HRESULT hr;
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

    // Compile Vertex Shader
    hr = D3DCompileFromFile(L"data/shader/Instancing.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        return false;
    }
    m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());

    // Compile Pixel Shaders
    if (!LoadPixelShader(L"data/shader/Instancing.hlsl", "PSLit", PixelShaderType::Lit, psBlob, errorBlob)) {
        return false;
    }
    if (!LoadPixelShader(L"data/shader/Instancing.hlsl", "PSUnlit", PixelShaderType::UnLit, psBlob, errorBlob)) {
        return false;
    }
    if (!LoadPixelShader(L"data/shader/Instancing.hlsl", "PSHologram", PixelShaderType::Hologram, psBlob, errorBlob)) {
        return false;
    }
    if (!LoadPixelShader(L"data/shader/Instancing.hlsl", "PSToon", PixelShaderType::Toon, psBlob, errorBlob)) {
        return false;
    }

    // Validate structure sizes to prevent offset mismatch in Input Layout
    static_assert(sizeof(Vector3) == 12);
    static_assert(sizeof(Color) == 16);
    static_assert(sizeof(Matrix4) == 64);

    // Define the Input Layout corresponding to Vertex3D + InstanceData
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        // --- Slot 0: Per-Vertex Data (Vertex3D) ---
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        // --- Slot 1: Per-Instance Data (InstanceData) ---
        // Matrix4 (Requires 4 separate float4 semantic slots)
        { "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

        // Color (Color / XMFLOAT4)
        { "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
    };

    hr = m_device->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        m_inputLayout.GetAddressOf()
    );

    return SUCCEEDED(hr);
}


bool RenderManager::LoadPixelShader(LPCWSTR filePath, LPCSTR entryPoint, PixelShaderType shaderType, Microsoft::WRL::ComPtr<ID3DBlob>& psBlob, Microsoft::WRL::ComPtr<ID3DBlob>& errorBlob)
{
    HRESULT hr = D3DCompileFromFile(filePath, nullptr, nullptr, entryPoint, "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        return false;
    }
    m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader[shaderType].GetAddressOf());
    return true;
}
