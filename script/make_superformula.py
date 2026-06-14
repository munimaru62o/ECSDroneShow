import math
import json

# Radius calculation function for the 2D Superformula
def superformula(theta, m, a, b, n1, n2, n3):
    if n1 == 0: return 0
    t1 = abs((1/a) * math.cos(m * theta / 4.0)) ** n2
    t2 = abs((1/b) * math.sin(m * theta / 4.0)) ** n3
    val = t1 + t2
    if val == 0: return 0
    return val ** (-1.0 / n1)

def generate_superformula_json(filename="superformula.json", num_segments=64, num_rings=48):
    raw_points = []
    
    # Parameter settings for a beautiful crystal/star shape
    # Changing 'm' alters the rotational symmetry / number of points 
    # (e.g., m=5 for a starfish shape, m=7 for a 7-pointed star)
    m1, a1, b1, n1_1, n2_1, n3_1 = 7, 1.0, 1.0, 0.2, 1.7, 1.7
    m2, a2, b2, n1_2, n2_2, n3_2 = 7, 1.0, 1.0, 0.2, 1.7, 1.7

    for i in range(num_rings + 1):
        # Latitude (-π/2 to π/2)
        phi = -math.pi / 2.0 + math.pi * i / num_rings
        r2 = superformula(phi, m2, a2, b2, n1_2, n2_2, n3_2)

        for j in range(num_segments):
            # Longitude (-π to π)
            theta = -math.pi + 2.0 * math.pi * j / num_segments
            r1 = superformula(theta, m1, a1, b1, n1_1, n2_1, n3_1)

            # Convert to 3D coordinates (based on the spherical coordinate system)
            x = r1 * math.cos(theta) * r2 * math.cos(phi)
            y = r1 * math.sin(theta) * r2 * math.cos(phi)
            z = r2 * math.sin(phi)

            raw_points.append((x, y, z))

    # --- Strict Normalization (Centering & scaling to the [-1.0, 1.0] range) ---
    min_x = min(p[0] for p in raw_points)
    max_x = max(p[0] for p in raw_points)
    min_y = min(p[1] for p in raw_points)
    max_y = max(p[1] for p in raw_points)
    min_z = min(p[2] for p in raw_points)
    max_z = max(p[2] for p in raw_points)

    center_x = (min_x + max_x) / 2.0
    center_y = (min_y + max_y) / 2.0
    center_z = (min_z + max_z) / 2.0

    # Calculate the maximum distance from the center for uniform scaling across all axes
    max_scale = max(
        max_x - center_x,
        max_y - center_y,
        max_z - center_z
    )
    if max_scale == 0: max_scale = 1.0

    # Construct the normalized coordinates for the JSON output
    points = []
    for x, y, z in raw_points:
        nx = (x - center_x) / max_scale
        ny = (y - center_y) / max_scale
        nz = (z - center_z) / max_scale
        
        points.append({
            "position": [round(nx, 5), round(ny, 5), round(nz, 5)]
        })

    # Build the JSON data structure
    formation_data = {
        "name": f"Superformula_m{m1}",
        "drone_count": len(points),
        "points": points
    }

    # Export to a JSON file
    with open(filename, "w", encoding="utf-8") as f:
        json.dump(formation_data, f, indent=2)
            
    print(f"✅ Successfully generated Superformula JSON: {filename} (Vertex count: {len(points)})")

if __name__ == "__main__":
    generate_superformula_json()