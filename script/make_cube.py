import os
import json

def generate_cube_json(filename="cube.json"):
    # The 8 corner coordinates of the cube (x, y, z)
    # * Already normalized to the [-1.0, 1.0] range
    corners = [
        (-1.0, -1.0, -1.0), # 0: Front, Bottom, Left
        ( 1.0, -1.0, -1.0), # 1: Front, Bottom, Right
        ( 1.0,  1.0, -1.0), # 2: Front, Top, Right
        (-1.0,  1.0, -1.0), # 3: Front, Top, Left
        (-1.0, -1.0,  1.0), # 4: Back, Bottom, Left
        ( 1.0, -1.0,  1.0), # 5: Back, Bottom, Right
        ( 1.0,  1.0,  1.0), # 6: Back, Top, Right
        (-1.0,  1.0,  1.0)  # 7: Back, Top, Left
    ]

    # Covers all 12 edges of the cube. To avoid crossing empty space diagonally, 
    # some edges are intentionally traversed twice to form a continuous path.
    path_indices = [
        0, 1, 2, 3, 0,  # Complete one loop around the front face
        4,              # Move from front-bottom-left to back-bottom-left
        5, 6, 7, 4,     # Complete one loop around the back face
        5, 1,           # Traverse from back-bottom-right to front-bottom-right
        2, 6,           # Traverse from front-top-right to back-top-right
        7, 3            # Traverse from back-top-left to front-top-left
        # * The final connection from '3' back to the initial '0' is handled 
        #   automatically by the '% numPoints' wrapping logic on the C++ side.
    ]

    points = []
    
    # Add vertices to the JSON list in the exact order of the continuous path
    for idx in path_indices:
        v = corners[idx]
        points.append({
            "position": [float(v[0]), float(v[1]), float(v[2])]
        })

    # Build the JSON data structure
    formation_data = {
        "name": "CubeFormation",
        "drone_count": len(points),
        "points": points
    }

    # Export to a JSON file
    with open(filename, "w", encoding="utf-8") as f:
        json.dump(formation_data, f, indent=2)
            
    print(f"✅ Successfully generated continuous cube path JSON: {filename} (Vertex count: {len(points)})")

if __name__ == "__main__":
    generate_cube_json()