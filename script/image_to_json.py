import argparse
import json
import math
import random
from PIL import Image

def image_to_normalized_formation(image_path, output_path, max_drones=1000, alpha_threshold=128, pixel_art_mode=False):
    """
    Loads an image, automatically scales it down to fit within a specified maximum drone count, 
    and outputs a formation JSON with coordinates normalized to the [-1.0, 1.0] range.
    """
    try:
        # Open the image and convert it to RGBA mode
        img = Image.open(image_path).convert("RGBA")
        
        # 1. Count the current number of non-transparent (valid) pixels
        alpha_hist = img.getchannel('A').histogram()
        valid_pixels = sum(alpha_hist[alpha_threshold:])
        print(f"[{image_path}] Successfully loaded (Original size: {img.width}x{img.height}, Valid pixels: {valid_pixels})")
        
        # 2. If the valid pixel count exceeds the target max drones, scale down the image
        if valid_pixels > max_drones:
            # Calculate the scale factor based on the area ratio
            scale = math.sqrt(max_drones / valid_pixels)
            new_width = max(1, int(img.width * scale))
            new_height = max(1, int(img.height * scale))
            
            # Resize the image (use NEAREST filter for pixel art to prevent color blending)
            resample_filter = Image.Resampling.NEAREST if pixel_art_mode else Image.Resampling.LANCZOS
            img = img.resize((new_width, new_height), resample_filter)
            
            print(f" -> Automatically resized image to {new_width}x{new_height} to stay within the {max_drones} target drone count.")
        
        # 3. Normalize coordinates and extract pixel data
        width, height = img.size
        center_x = (width - 1) / 2.0
        center_y = (height - 1) / 2.0
        
        max_scale = max(width / 2.0, height / 2.0)
        if max_scale == 0: max_scale = 1.0
            
        points = []
        
        for y in range(height):
            for x in range(width):
                r, g, b, a = img.getpixel((x, y))
                
                # Skip transparent pixels
                if a < alpha_threshold:
                    continue
                
                # Normalize coordinates to the [-1.0, 1.0] range
                nx = (x - center_x) / max_scale
                # Invert Y-axis so standard image orientation matches 3D space
                ny = -(y - center_y) / max_scale 
                nz = 0.0 
                
                points.append({
                    "position": [round(nx, 5), round(ny, 5), round(nz, 5)],
                    "color": [r, g, b]
                })

        # Final active drone count
        actual_count = len(points)

        formation_data = {
            "name": "ImageFormation",
            "drone_count": actual_count,
            "points": points
        }

        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(formation_data, f, indent=2)

        print(f"Conversion successful! Final drone count: {actual_count} (Output path: {output_path})")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Automatically scales down an image and converts it into a normalized formation JSON.")
    parser.add_argument("input", help="Path to the input image")
    parser.add_argument("output", help="Path for the output JSON")
    parser.add_argument("--max", type=int, default=1000, help="Target maximum number of drones (Default: 1000)")
    parser.add_argument("--threshold", type=int, default=128, help="Alpha transparency threshold 0-255 (Default: 128)")
    parser.add_argument("--pixel", action="store_true", help="Enable pixel art mode (uses nearest-neighbor resampling)")
    
    args = parser.parse_args()
    
    image_to_normalized_formation(args.input, args.output, args.max, args.threshold, args.pixel)