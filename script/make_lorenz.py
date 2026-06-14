import json

def generate_lorenz_json(filename="lorenz.json", num_points=3000):
    raw_points = []
    
    # Standard parameters for the Lorenz equations
    sigma = 10.0
    rho = 28.0
    beta = 8.0 / 3.0
    
    # Initial coordinates and time step
    x, y, z = 0.1, 0.0, 0.0
    dt = 0.005
    
    # Discard the first 1000 steps as a "warm-up" to allow the trajectory to settle onto the attractor
    for _ in range(1000):
        dx = sigma * (y - x) * dt
        dy = (x * (rho - z) - y) * dt
        dz = (x * y - beta * z) * dt
        x += dx; y += dy; z += dz
        
    # Sample the actual drone coordinates (calculate raw positions)
    for _ in range(num_points):
        dx = sigma * (y - x) * dt
        dy = (x * (rho - z) - y) * dt
        dz = (x * y - beta * z) * dt
        x += dx; y += dy; z += dz
        
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
        "name": "LorenzAttractor",
        "drone_count": len(points),
        "points": points
    }
            
    # Export to a JSON file
    with open(filename, "w", encoding="utf-8") as f:
        json.dump(formation_data, f, indent=2)
            
    print(f"✅ Successfully generated Lorenz Attractor JSON: {filename} (Vertex count: {len(points)})")

if __name__ == "__main__":
    generate_lorenz_json()