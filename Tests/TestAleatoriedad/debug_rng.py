import subprocess
import os

def debug_rng():
    EXE_PATH = "rng_driver.exe"
    
    # 1. Run the generator for a split second
    try:
        process = subprocess.Popen(
            [EXE_PATH], 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE
        )
        # Read just 20 bytes
        raw_bytes = process.stdout.read(20)
        process.terminate()
    except Exception as e:
        print(f"Error: {e}")
        return

    print("RAW OUTPUT INSPECTION:")
    print("-" * 30)
    print(f"Bytes read: {len(raw_bytes)}")
    
    # 2. Print the Hex values
    hex_vals = " ".join(f"{b:02X}" for b in raw_bytes)
    print(f"Hex: {hex_vals}")
    
    # 3. Check for specific corruption signatures
    if len(raw_bytes) == 0:
        print("❌ FAIL: No data received.")
    elif raw_bytes.count(0x00) > 10:
        print("❌ FAIL: Too many null bytes (00). Likely UTF-16 encoding issue.")
    elif b'\x0D\x0A' in raw_bytes:
        print("❌ FAIL: Found '0D 0A'. Windows Text Mode is corrupting data.")
    else:
        print("✅ PASS: Data looks like valid raw binary.")

if __name__ == "__main__":
    debug_rng()