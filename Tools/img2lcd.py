import sys
import pyperclip
import numpy as np
from PIL import Image

# Load the modified image
img_path = sys.argv[1]
if not img_path:
    exit(1)
img = Image.open(img_path)

# Ensure the image is in 1-bit per pixel format
img_1bit = img.convert("1")

# Convert image to numpy array
img_array = np.array(img_1bit)

# Create an array to store the bytes for the C array
# The LCD has 8 pages (or banks) and each page is 128 pixels wide
# Each byte in a page represents a vertical column of 8 pixels
lcd_width = 128
lcd_height = 64
lcd_pages = lcd_height // 8  # 8 pixels per page
lcd_array = np.zeros((lcd_pages, lcd_width), dtype=np.uint8)

# Loop through the array and populate the lcd_array
for page in range(lcd_pages):
    for x in range(lcd_width):
        # Each byte represents a vertical column of 8 pixels
        byte = 0
        for bit in range(8):
            # Calculate the bit value, set to 1 if the pixel is black (on)
            # The image is inverted because on the LCD, a bit of 0 is off and 1 is on
            if img_array[page * 8 + bit, x] == 0:
                byte |= 1 << bit
        lcd_array[page, x] = byte


# Function to split the array into chunks of 16 elements
def split_by_n(lst, n):
    """A generator to divide a sequence into chunks of n units."""
    for i in range(0, len(lst), n):
        yield lst[i : i + n]


lines = []
# Split the C array into chunks of 16 elements
for i, chunk in enumerate(split_by_n(lcd_array.flatten(), 16)):
    # Convert the lcd_array to a C unsigned char array
    formatted_chunk = ", ".join(f"0x{b:02x}" for b in chunk) + ","
    if i % 8 == 0:
        lines.append(f"// Bank {i // 8}")
    lines.append(formatted_chunk)

result = "\n".join(lines) + "\n"
print(result)
pyperclip.copy(result)
print("Copied result to clipboard")
