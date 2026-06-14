import json
import sys
import os
import math

def convert_obj_to_json(obj_filepath, max_points=500):
    print(f"Processing started: {obj_filepath}")
    if not os.path.exists(obj_filepath):
        print(f"❌ Error: File '{obj_filepath}' not found.")
        return

    vertices = []
    
    # First, read all vertices from the OBJ file
    with open(obj_filepath, 'r') as f:
        for line in f:
            if line.startswith('v '):
                parts = line.strip().split()
                try:
                    vertices.append({
                        "x": float(parts[1]),
                        "y": float(parts[2]),
                        "z": float(parts[3])
                    })
                except Exception:
                    pass

    total_vertices = len(vertices)
    print(f"Original vertex count: {total_vertices}")

    if total_vertices == 0:
        print("❌ Error: No vertices found in the file.")
        return

    # Decimation (Downsampling) process
    if total_vertices > max_points:
        print(f"⚠️ Vertex count too high. Downsampling to {max_points} vertices...")
        # Calculate the uniform step size for decimation
        step = total_vertices / max_points
        reduced_vertices = []
        for i in range(max_points):
            # Retrieve the vertex by rounding down the floating-point index
            index = math.floor(i * step)
            reduced_vertices.append(vertices[index])
        vertices = reduced_vertices

    # Export process
    formation_name = os.path.splitext(os.path.basename(obj_filepath))[0]
    output_data = {
        "name": formation_name,
        "points": vertices
    }
    
    json_filepath = f"{formation_name}.json"
    with open(json_filepath, 'w') as f:
        json.dump(output_data, f, indent=2)
        
    print(f"✅ Conversion successful! Exported {len(vertices)} vertices to {json_filepath}.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python obj_to_json.py <filename.obj> [max_vertices]")
    else:
        max_pts = int(sys.argv[2]) if len(sys.argv) > 2 else 500
        convert_obj_to_json(sys.argv[1], max_pts)