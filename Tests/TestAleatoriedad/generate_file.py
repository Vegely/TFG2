import subprocess
# Generate 100 MB
size = 100 * 1024 * 1024
with open("clean_test.bin", "wb") as f:
    # Run the generator and dump strictly to file
    process = subprocess.Popen(["rng_driver.exe"], stdout=f)
    try:
        process.wait(timeout=20) # Run for 5 seconds (adjust if needed to reach size)
    except subprocess.TimeoutExpired:
        process.terminate()
print("Done. File 'clean_test.bin' created.")