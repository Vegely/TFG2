import serial
import time
import sys

# --- CONFIGURATION ---
COM_PORT = 'COM6'        # <--- CHANGE THIS to your actual port
BAUD_RATE = 4000000      # Must match main.c exactly
FILE_NAME = "psoc_trng_1gb.bin"
TARGET_SIZE_GB = 1       # How many GB to capture
# ---------------------

TARGET_BYTES = TARGET_SIZE_GB * 1024 * 1024 * 1024

def capture_stream():
    try:
        # Open Serial Port
        print(f"Connecting to {COM_PORT} at {BAUD_RATE} baud...")
        ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=2)
        
        # Open Output File
        with open(FILE_NAME, "wb") as f:
            print("Sending start signal ('s')...")
            # Clear any garbage in the buffer
            ser.reset_input_buffer()
            # Send 's' to tell PSoC to start
            ser.write(b's')
            
            bytes_collected = 0
            start_time = time.time()
            
            print(f"Starting capture of {TARGET_SIZE_GB} GB...")

            while bytes_collected < TARGET_BYTES:
                # Read large chunks. The driver handles the details.
                # Requesting 16KB usually gives good performance.
                chunk = ser.read(16384)
                
                if not chunk:
                    continue

                f.write(chunk)
                bytes_collected += len(chunk)

                # Update Progress Display
                elapsed = time.time() - start_time
                if elapsed > 0:
                    mb_speed = (bytes_collected / (1024*1024)) / elapsed
                    percent = (bytes_collected / TARGET_BYTES) * 100
                    
                    sys.stdout.write(f"\rProgress: {percent:.2f}% | "
                                     f"{bytes_collected/(1024*1024):.0f} MB | "
                                     f"Speed: {mb_speed:.2f} MB/s")
                    sys.stdout.flush()

        print(f"\n\nSuccess! Saved to {FILE_NAME}")
        ser.close()

    except serial.SerialException as e:
        print(f"\nError: {e}")
        print("Check if the COM port is correct and not open in another program.")
    except KeyboardInterrupt:
        print("\nStopped by user.")

if __name__ == "__main__":
    capture_stream()