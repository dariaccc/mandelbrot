# Mandelbrot Set Visualizer

## What is the Mandelbrot Set?

The **Mandelbrot set** is a collection of complex numbers `C` for which a recursive function remains bounded instead of diverging to infinity. The recursive formula is:
zₙ₊₁ = zₙ² + C, with z₀ = 0


Here, `C` is a complex number made of a real and an imaginary part. If the magnitude of `z` stays bounded during iteration, then `C` belongs to the Mandelbrot set. Otherwise, it escapes to infinity.

### Visual Representation

Below, the yellow dot represents a value `C`, and the green dots represent iterations of `z`.  
- In the left image: `z` stays bounded → `C` is in the Mandelbrot set.  
- In the right image: `z` spirals to infinity → `C` is not in the set.

> _Images from: [GeoGebra Source](https://www.geogebra.org/m/BUVhcRSv#material/Npd3kBKn)_

![Bounded Iteration Example](INSERT_IMAGE_URL_HERE)
![Escaping Iteration Example](INSERT_IMAGE_URL_HERE)

## Fractal Nature

When plotted on the complex plane, the Mandelbrot set forms a stunning fractal. Zooming in reveals infinite levels of detail—new shapes keep appearing regardless of how far you zoom in.

---

## Code Overview

This project renders the Mandelbrot set using **C++**, **SDL2**, and **OpenMP** for parallel computing.

### Key Features
- **Zooming** into the “seahorse valley” region of the set.
- **Parallelized rendering** using OpenMP.
- **Dynamic iteration adjustment** for performance scaling.

### File Structure

#### `mset` function
Determines whether a complex number `C` is part of the Mandelbrot set by iterating `z = z² + C` up to a maximum iteration `max_it`.  
- Returns: the iteration count at which `z` escapes (if it does), otherwise `0`.

#### `renderMandelbrot` function
Handles pixel-by-pixel rendering:
- Translates screen coordinates to complex numbers.
- Calls `mset` for each pixel.
- Uses OpenMP for parallel processing.
- Pixel color depends on the number of iterations:
  - `0` → black (in the set).
  - Otherwise → color based on escape time.

#### `main` function
- Initializes SDL2.
- Manages user interactions (zoom and quit).
- Dynamically adjusts `max_it` to balance performance and detail.
- Stops zooming on mouse click.

---

## Parallelization with OpenMP

- Uses `omp_get_max_threads()` to leverage all CPU cores.
- Applies `#pragma omp parallel for collapse(2) schedule(dynamic)` in `renderMandelbrot`:
  - `collapse(2)` flattens nested loops for balanced workload distribution.
  - `schedule(dynamic)` allows flexible thread assignment depending on computation load.
- `#pragma omp critical` ensures only one thread accesses critical drawing sections to avoid race conditions.

---

## Performance Comparison

### Scatter Plot: Serial vs Parallel (18 Renditions)

![Performance Scatter Plot](INSERT_IMAGE_URL_HERE)


- **Rendition 1**: Similar timing for both versions due to initial SDL setup overhead.
- **Renditions 2+**: Parallel version outpaces the serial version, doubling speed in some cases.
- **Adaptation**: If rendering exceeds 1.5 seconds, the program reduces `max_it`, simplifying computation.
  - Serial version reduces quality faster.
  - Parallel version maintains detail longer due to better efficiency.

---

## Conclusion

This Mandelbrot renderer demonstrates the advantages of parallel computing:
- **Setup time** is similar for both serial and parallel modes.
- **Main rendering** benefits heavily from parallelism.
- **Dynamic tuning** helps maintain interactivity and responsiveness.

While the serial version degrades in visual quality to maintain speed, the parallel version preserves detail longer, offering both performance and visual fidelity.

---
