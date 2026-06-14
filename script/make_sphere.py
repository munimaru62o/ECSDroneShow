import json
import math

def generate_sphere_json(filename="sphere.json", num_segments=32, num_rings=16):
    raw_points = []
    
    # Sample aesthetically pleasing grid coordinates on the spherical surface using latitude and longitude
    for i in range(num_rings + 1):
        phi = math.pi * i / num_rings
        y = math.cos(phi)
        sin_phi = math.sin(phi)
        
        for j in range(num_segments):
            theta = 2.0 * math.pi * j / num_segments
            x = sin_phi * math.cos(theta)
            z = sin_phi * math.sin(theta)
            raw_points.append((x, y, z))
            
    # Since the generated coordinates are on a unit sphere (radius 1), 
    # they are naturally normalized within the [-1.0, 1.0] range. 
    # Therefore, we simply round the decimals for the final output.
    points = []
    for x, y, z in raw_points:
        points.append({
            "position": [round(x, 5), round(y, 5), round(z, 5)]
        })

    # Build the JSON data structure
    formation_data = {
        "name": "SphereFormation",
        "drone_count": len(points),
        "points": points
    }
            
    # Export to a JSON file
    with open(filename, "w", encoding="utf-8") as f:
        json.dump(formation_data, f, indent=2)
            
    print(f"✅ Successfully generated Sphere formation JSON: {filename} (Vertex count: {len(points)})")

if __name__ == "__main__":
    generate_sphere_json()