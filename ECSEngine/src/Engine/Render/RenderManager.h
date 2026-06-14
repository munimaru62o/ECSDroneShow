// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Math/Color.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Matrix4.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>
#include <unordered_map>


/**
 * @class RenderManager
 * @brief A singleton manager handling DirectX 11 rendering pipelines, optimized for Hardware Instancing.
 *
 * This manager interfaces with the underlying D3D11 device (retrieved via DxLib) to create and manage
 * vertex/index buffers, constant buffers, and shaders. Its primary feature is rendering massive amounts
 * of entities efficiently using `DrawIndexedInstanced` in a single draw call.
 */
class RenderManager final : private NonCopyable
{
private:
    struct CameraBuffer;
    struct LightBuffer;
    struct GPUMeshData
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
        UINT indexCount = 0;
    };

private:
    RenderManager();
    ~RenderManager();

public:
    [[nodiscard]] static RenderManager& GetInstance()
    {
        static RenderManager instance;
        return instance;
    }

    bool Init();
    void Shutdown();

    void UpdateLight(const Vector3& direction, float ambient, const Color& lightColor = Color::White(), const Color& fillColor = Color::Black());
    void UpdateCamera(const Matrix4& viewProjection, const Vector3& position);

    [[nodiscard]] MeshID CreateMesh(const MeshSourceData& data);
    void DrawInstanced(MeshID meshId, const std::vector<InstanceData>& instanceData, PixelShaderType shaderType = PixelShaderType::Lit);

private:
    bool CreateVertexBuffer(GPUMeshData& mesh, const std::vector<Vertex3D>& vertices);
    bool CreateIndexBuffer(GPUMeshData& mesh, const std::vector<uint32_t>& indices);
    bool CreateInstanceBuffer();
    bool CreateCameraBuffer();
    bool CreateLightBuffer();
    bool CreateDepthStencilState();
    bool CreateBlendState();

    bool LoadShaders();
    bool LoadPixelShader(LPCWSTR filePath, LPCSTR entryPoint, PixelShaderType shaderType, Microsoft::WRL::ComPtr<ID3DBlob>& psBlob, Microsoft::WRL::ComPtr<ID3DBlob>& errorBlob);

private:
    ID3D11Device* m_device         = nullptr;
    ID3D11DeviceContext* m_context = nullptr;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cameraBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightBuffer;

    std::unordered_map<PixelShaderType, Microsoft::WRL::ComPtr<ID3D11PixelShader>> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_opaqueBlendState;

    std::unordered_map<MeshID, GPUMeshData> m_meshes;
    MeshID m_nextMeshId = 1;
};
