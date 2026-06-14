import math
import json

def generate_torus_knot_json(filename="torus_knot.json", num_points=3000, p=2, q=3):
    raw_points = []
    
    # Sample the trajectory of the knot using parameter 't' from 0 to 2π
    for i in range(num_points):
        t = 2.0 * math.pi * i / num_points
        
        # Base equations for the Torus Knot
        r = math.cos(q * t) + 2.0
        x = r * math.cos(p * t)
        y = r * math.sin(p * t)
        z = -math.sin(q * t)
        
        raw_points.append((x, y, z))

    # --- Strict Normalization (Centering & scaling to the [-1.0, 1.0] range) ---
    min_x = min(pt[0] for pt in raw_points)
    max_x = max(pt[0] for pt in raw_points)
    min_y = min(pt[1] for pt in raw_points)
    max_y = max(pt[1] for pt in raw_points)
    min_z = min(pt[2] for pt in raw_points)
    max_z = max(pt[2] for pt in raw_points)

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
        "name": f"TorusKnot_p{p}_q{q}",
        "drone_count": len(points),
        "points": points
    }
            
    # Export to a JSON file
    with open(filename, "w", encoding="utf-8") as f:
        json.dump(formation_data, f, indent=2)
            
    print(f"✅ Successfully generated Torus Knot JSON: {filename} (Vertex count: {len(points)})")

if __name__ == "__main__":
    # Changing the coprime integers 'p' and 'q' alters the shape and complexity of the knot
    generate_torus_knot_json(filename="torus_knot.json", num_points=3000, p=2, q=3)