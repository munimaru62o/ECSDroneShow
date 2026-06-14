cbuffer CameraBuffer : register(b0)
{
    row_major matrix ViewProjection;
    float3 CameraPosition;
    float padding;
};

cbuffer LightBuffer : register(b1)
{
    float3 LightDirection;  // Direction of the main directional light
    float AmbientLight;     // Intensity of the ambient environmental light
    float4 LightColor;      // Color of the main light
    float4 FillColor;       // Color of the secondary/rim light
};

// ---------------------------------------------------------
// Input Structures
// ---------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    
    // Instancing data: World matrix reconstructed from 4 float4 rows
    float4 WorldRow0 : WORLD0;
    float4 WorldRow1 : WORLD1;
    float4 WorldRow2 : WORLD2;
    float4 WorldRow3 : WORLD3;
    float4 Color     : COLOR;
};

struct PS_INPUT
{
    float4 Pos      : SV_POSITION;
    float3 WorldPos : WORLD_POSITION;
    float4 Color    : COLOR;
    float3 Normal   : NORMAL;
};

// ---------------------------------------------------------
// Vertex Shader
// ---------------------------------------------------------
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    
    // Reconstruct the world matrix from instanced row data
    matrix worldMatrix = matrix(input.WorldRow0, input.WorldRow1, input.WorldRow2, input.WorldRow3);
    
    // Transform local position to world space
    float4 worldPos = mul(float4(input.Pos, 1.0f), worldMatrix);
    
    // Transform world position to clip space (SV_POSITION)
    output.Pos = mul(worldPos, ViewProjection);

    output.WorldPos = worldPos.xyz;

    float3 axisX = input.WorldRow0.xyz;
    float3 axisY = input.WorldRow1.xyz;
    float3 axisZ = input.WorldRow2.xyz;

    float3 nx = cross(axisY, axisZ);
    float3 ny = cross(axisZ, axisX);
    float3 nz = cross(axisX, axisY);

    float3 worldNormal = input.Normal.x * nx + input.Normal.y * ny + input.Normal.z * nz;

    output.Normal = normalize(worldNormal);
    output.Color = input.Color;
    
    return output;
}

// ---------------------------------------------------------
// Pixel Shaders
// ---------------------------------------------------------
// Standard Lit Shader (Half-Lambert + Blinn-Phong)
float4 PSLit(PS_INPUT input) : SV_Target
{
    float3 N = normalize(input.Normal);
    float3 L = normalize(-LightDirection);
    float3 V = normalize(CameraPosition - input.WorldPos);

    // Main diffuse: Half-Lambert lighting wrapped and squared for softer terminator
    float mainDiffuse = dot(N, L) * 0.5f + 0.5f;
    mainDiffuse = mainDiffuse * mainDiffuse;
    
    // Fill diffuse: Illuminate from the exact opposite direction of the main light
    float fillDiffuse = dot(N, -L) * 0.5f + 0.5f;
    fillDiffuse = fillDiffuse * fillDiffuse;
    
    float3 mainColor = LightColor.rgb * mainDiffuse;
    float3 fillColor = FillColor.rgb * fillDiffuse;
    
    // Specular: Blinn-Phong reflection model
    float3 H = normalize(L + V);
    float specular = pow(saturate(dot(N, H)), 64.0f);
    
    // Combine lighting components
    float3 finalColor = input.Color.rgb * (mainColor + fillColor + AmbientLight);
    finalColor += LightColor.rgb * specular * 0.15f;
    
    return float4(saturate(finalColor), input.Color.a);
}

// Unlit Shader (Color only)
float4 PSUnlit(PS_INPUT input) : SV_Target
{
    return input.Color;
}

// Hologram / Sci-Fi Shader
float4 PSHologram(PS_INPUT input) : SV_Target
{
    float3 N = normalize(input.Normal);
    float3 L = normalize(-LightDirection);
    float3 V = normalize(CameraPosition - input.WorldPos); 
    float3 baseColor = input.Color.rgb; 

    // --- Diffuse ---
    float NdotL = saturate(dot(N, L));
    float mainDiffuse = sqrt(NdotL); // Elevate dark areas by taking the square root
    
    // --- Specular ---
    // High shininess (e.g., 128-256) yields a metallic/glass-like sharp highlight
    float3 H = normalize(L + V);
    float NdotH = saturate(dot(N, H));
    
   float shininess = 256.0f; 
    float specularIntensity = pow(NdotH, shininess);
    
    float specularPower = 6.0f; 
    float3 specularColor = LightColor.rgb * specularIntensity * specularPower;

    // --- Rim Light ---
    float NdotV = saturate(dot(N, V));
    float rim = pow(1.0f - NdotV, 4.0f); 
    // Suppress rim light in shadowed areas to preserve volumetric depth
    rim *= saturate(NdotL + 0.2f);

    // --- Distance Fade (LOD) ---
    // Smoothly fade out specular and rim effects at a distance to prevent 
    // over-exposure (blowout) when massive numbers of entities cluster together.
    float distanceToCamera = length(CameraPosition - input.WorldPos);
    float fade = saturate(1.0f - (distanceToCamera - 200.0f) / 400.0f); 

    // --- Composition ---
    // Start with the base diffuse, then additively blend lighting effects
    float3 finalColor = baseColor * mainDiffuse * LightColor.rgb;
    
    finalColor += (FillColor.rgb * rim * 1.5f) * fade; // Add distance-faded rim light
    finalColor += specularColor * fade;                // Add distance-faded specular
    finalColor += baseColor * AmbientLight;            // Add ambient light

    return float4(saturate(finalColor), input.Color.a);
}

// Cel-Shaded / Toon Shader
float4 PSToon(PS_INPUT input) : SV_Target
{
    float3 N = normalize(input.Normal);
    float3 L = normalize(-LightDirection);
    float3 V = normalize(CameraPosition - input.WorldPos);

    // --- Diffuse (Stepped) ---
    float diffuse = dot(N, L) * 0.5f + 0.5f;
    diffuse *= diffuse; // Half-Lambert curve before stepping

    if (diffuse > 0.75f) {
        diffuse = 1.0f;
    }
    else if (diffuse > 0.35f) {
        diffuse = 0.6f;
    }
    else {
        diffuse = 0.25f;
    }

    // --- Rim (Stepped) ---
    float rim = 1.0f - saturate(dot(N, V));
    rim = pow(rim, 3.0f);
    rim = step(0.6f, rim); // Hard cutoff for cartoonish rim lighting

    // --- Specular (Stepped) ---
    float3 H = normalize(L + V);
    float specular = pow(saturate(dot(N, H)), 64.0f);
    specular = step(0.4f, specular); // Hard cutoff for sharp anime-style highlights

    // --- Composition ---
    float3 finalColor = input.Color.rgb * diffuse * LightColor.rgb;

    finalColor += LightColor.rgb * specular * 0.25f;
    finalColor += FillColor.rgb * rim * 0.15f;
    finalColor += input.Color.rgb * AmbientLight;

    return float4(saturate(finalColor), input.Color.a);
}