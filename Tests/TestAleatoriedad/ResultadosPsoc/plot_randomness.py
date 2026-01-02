import numpy as np
import matplotlib.pyplot as plt

# --- CONFIGURATION ---
FILE_PATH = "psoc_trng_1gb.bin"   # Your filename
SAMPLES_TO_READ = 10000000      # 1 MB is sufficient for visualization
OUTPUT_FORMAT = "pdf"         # 'svg' or 'pdf' are best for vector graphics

def generate_pretty_plots():
    try:
        # 1. Load Data
        print(f"Reading data from {FILE_PATH}...")
        data = np.fromfile(FILE_PATH, dtype=np.uint8, count=SAMPLES_TO_READ)
        
        if data.size == 0:
            print("Error: File is empty!")
            return

        # Apply a clean, professional style
        # If this fails on your machine, remove the line below
        try:
            plt.style.use('seaborn-v0_8-whitegrid')
        except:
            plt.style.use('bmh') # Fallback style

        # ==========================================
        # FIGURE 1: HISTOGRAM (Uniformity)
        # ==========================================
        print("Generating Histogram...")
        plt.figure(figsize=(10, 6))
        
        # Plot with a nicer color and edge
        plt.hist(data, bins=256, range=(0, 255), 
                 color='#4C72B0', edgecolor='none', alpha=0.9, density=True)
        
        # Strict Limits
        plt.xlim(0, 255)
        
        # Labels and Titles
        plt.title("Byte Frequency Distribution (Histogram)", fontsize=14, fontweight='bold', pad=15)
        plt.xlabel("Byte Value (0–255)", fontsize=12)
        plt.ylabel("Probability Density", fontsize=12)
        plt.grid(axis='x', alpha=0) # Hide vertical grid lines for cleaner look
        
        # Save
        plt.tight_layout()
        plt.savefig(f"1_histogram.{OUTPUT_FORMAT}", format=OUTPUT_FORMAT)
        plt.close() # Free up memory

        # ==========================================
        # FIGURE 2: BITMAP (Visual Noise)
        # ==========================================
        print("Generating Bitmap...")
        plt.figure(figsize=(10, 10))
        
        # Calculate square dimension
        side = int(np.sqrt(data.size))
        if side > 1000: side = 1000 # Cap at 1000x1000 for performance
        
        img_data = data[:side*side].reshape((side, side))
        
        # Plot grayscale with no interpolation (crisp pixels)
        plt.imshow(img_data, cmap='gray', vmin=0, vmax=255, interpolation='nearest')
        
        # Remove all axes and whitespace
        plt.axis('off')
        plt.title(f"Bitmap Visualization ({side}x{side} pixels)", fontsize=14, fontweight='bold', pad=15)
        
        # Save with tight bounding box to remove white borders
        plt.savefig(f"2_bitmap.{OUTPUT_FORMAT}", format=OUTPUT_FORMAT, bbox_inches='tight', pad_inches=0.1)
        plt.close()

        # ==========================================
        # FIGURE 3: LAG PLOT (Correlations)
        # ==========================================
        print("Generating Lag Plot...")
        plt.figure(figsize=(8, 8))
        
        # Prepare X and Y (offset by 1)
        x = data[:-1]
        y = data[1:]
        
        # Plot only first 8000 points to keep vector file size manageable
        # Using a very small point size and transparency helps see clusters
        plt.scatter(x[:8000], y[:8000], s=3, alpha=0.4, color='#2F4F4F', edgecolors='none')
        
        # Strict Limits & Square Aspect Ratio
        plt.xlim(0, 255)
        plt.ylim(0, 255)
        plt.gca().set_aspect('equal', adjustable='box')
        
        # Labels
        plt.title("Lag Plot ($x_i$ vs $x_{i+1}$)", fontsize=14, fontweight='bold', pad=15)
        plt.xlabel("Current Byte Value ($x_i$)", fontsize=12)
        plt.ylabel("Next Byte Value ($x_{i+1}$)", fontsize=12)
        
        # Clean up ticks
        plt.xticks([0, 64, 128, 192, 255])
        plt.yticks([0, 64, 128, 192, 255])
        
        # Save
        plt.tight_layout()
        plt.savefig(f"3_lagplot.{OUTPUT_FORMAT}", format=OUTPUT_FORMAT)
        plt.close()

        print(f"\nSuccess! 3 files generated in the current folder ending in .{OUTPUT_FORMAT}")

    except FileNotFoundError:
        print(f"Error: Could not find '{FILE_PATH}'. Check the filename!")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    generate_pretty_plots()