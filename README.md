# RetroGFX - A Low-Level Graphics Library for DOS

**RetroGFX** is a **custom software graphics library** written before modern graphics APIs like OpenGL, DirectX, and SDL were available. Designed for **VGA Mode 13h (320x200, 256 colors)**, this library provides essential **image manipulation, palette control, sprite rendering, scrolling, and real-time effects** through direct memory access.

This project showcases **low-level graphics programming, memory management, and efficient rendering techniques**, making it a valuable reference for those interested in **early game engine development**.

---

## 🖥 **Key Features**
### **1️⃣ Custom Software Rendering**
- **Direct pixel manipulation** using VGA memory (`0xA0000`).
- Optimized **memory handling and direct screen writes**.

### **2️⃣ Graphics & Image Processing**
- **Custom sprite rendering** (supports transparency).
- **Scrolling & viewport movement**.
- **Mosaic effects** and **image scaling**.
- **VGA palette management** (color manipulation, fading, blending).
- **PCX image file handling** (direct file decoding).

### **3️⃣ Performance Optimizations**
- **Fixed-point math** for performance.
- **Fast `sqrt()` approximation** for distance calculations.
- **Wait-for-VSync** logic to prevent screen tearing.
- **Optimized buffer swapping** for smooth rendering.

---

## 📁 **Project Structure**
- **`GRAPHICS.H`** - Function prototypes, macros, and data structures for rendering.
- **`GRAPHICS.C`** - Implementation of graphics functions, including image loading, transformations, and color effects.

---

## 🛠 **How It Works**
This library operates by **directly writing to VGA memory (`0xA0000`)**, bypassing BIOS functions for maximum performance. The rendering pipeline performs:
1. **Direct bitmap drawing** to video memory.
2. **Palette color adjustments** for fading and blending.
3. **Tile-based scrolling** for smooth level movement.
4. **Fixed-point optimizations** to handle real-time effects.

---

## 🚀 **Compiling & Running**
1. **Compiler**: Uses a **Borland C++ / Watcom C++ Compiler**.
2. **Run in DOS**: Use **DOSBox** for modern execution.
3. **Execute in Mode 13h**: Ensure the program is running in **320x200 256-color mode**.

```sh
# Example (Compiling with Borland C++)
bcc graphics.c -o graphics.exe
```

## 🔍 **Why This Matters**
This project is an example of pre-OpenGL/DirectX game development, demonstrating:

- Custom rendering pipelines before standard libraries.
- Low-level memory and graphics operations.
- Mathematical proficiency in vector calculations, matrix transformations, and image processing.
- This is an excellent study resource for anyone interested in graphics programming, retro game development, or optimizing low-level C code for graphics.

## 📜 **Source Code**

- Custom 2D rendering techniques.
- Manual VGA memory access.
- Palette and sprite rendering functions.
