import sys
import os

# Default dimensions (can modify if needed)
width  = 96
height = 64

if len(sys.argv) < 2:
    print("Usage: python raw_to_565.py <input.raw> [width height]")
    sys.exit(1)

input_file = sys.argv[1]

# Optional width/height override
if len(sys.argv) == 4:
    width  = int(sys.argv[2])
    height = int(sys.argv[3])

# Check file existence
if not os.path.isfile(input_file):
    print(f"Error: file '{input_file}' not found.")
    sys.exit(1)

with open(input_file, "rb") as f:
    data = f.read()

expected_size = width * height * 3

if len(data) != expected_size:
    print(f"Warning: expected {expected_size} bytes, got {len(data)} bytes")

with open("bitmap.h", "w") as out:
    out.write("const uint8_t bitmap[] = {\n")

    for i in range(0, len(data), 3):
        r = data[i]
        g = data[i+1]
        b = data[i+2]

        rgb565 = ((r & 0xF8) << 8) | \
                 ((g & 0xFC) << 3) | \
                 (b >> 3)

        out.write(f"0x{rgb565>>8:02X}, 0x{rgb565&0xFF:02X}, ")

        if (i // 3 + 1) % width == 0:
            out.write("\n")

    out.write("};\n")

print("Conversion complete → bitmap.h generated.")
