#include <iostream> // standard input/output stream library
#include </usr/local/opt/libomp/include/omp.h> // openmp library
#include <SDL2/SDL.h> // Include the SDL2 library for graphics rendering
#include <numeric> // Include numeric library for numeric operations
#include <complex> // Include complex library for complex number operations

// Function to determine if a complex number is in the Mandelbrot set
int mset(std::complex<double> c, int max_it) {
    std::complex<double> z(0, 0); // Initialize z to 0 + 0i
    for (int i = 0; i < max_it; i++) { // Iterate up to max_it times
        z = std::pow(z, 2) + c; // z = z^2 + c
        if (std::norm(z) > 10) { // If the magnitude of z exceeds 10
            return i; // Return the iteration count
        }
    }
    return 0; // Return 0 if z does not escape within max_it iterations
}

// Function to render the Mandelbrot set using SDL
void renderMandelbrot(SDL_Renderer* renderer, int windowsize, double scale, double offsetX, double offsetY, int max_it) {
    int num_threads = omp_get_max_threads(); // Get the maximum number of threads available
    omp_set_num_threads(num_threads); // Set the number of threads for OpenMP

    #pragma omp parallel for schedule(dynamic) collapse(2) // Parallelize the nested loops with OpenMP
    for (int x_point = 0; x_point < windowsize; x_point++) { // Loop over x-coordinates
        for (int y_point = 0; y_point < windowsize; y_point++) { // Loop over y-coordinates
            // Transform screen coordinates to complex plane coordinates
            double x = scale * (x_point - windowsize / 2.0) + offsetX;
            double y = scale * (y_point - windowsize / 2.0) - offsetY;
            int times = mset(std::complex<double>(x, y), max_it); // Determine escape time for the point
            
            if (times == 0) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Inside the set: black
            } else {
                // Color based on escape time
                int red = (times * 6) % 256;
                int green = (times * 12) % 256;
                int blue = (times * 14) % 256;
                SDL_SetRenderDrawColor(renderer, red, green, blue, 255); // Outside the set: color
            }
            
            #pragma omp critical // Ensure thread safety for rendering
            SDL_RenderDrawPoint(renderer, x_point, y_point); // Draw the point
        }
    }
}

// Main function
int main() {
    SDL_Init(SDL_INIT_EVERYTHING); // Initialize SDL

    int windowsize = 500; // Define the window size
    SDL_Window* window = SDL_CreateWindow("Mandelbrot Set", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowsize, windowsize, SDL_WINDOW_SHOWN); // Create the SDL window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // Create the SDL renderer

    double scale = 3.0 / windowsize; // Define the initial scale
    double offsetX = -0.5; // Initial x offset
    double offsetY = 0.0; // Initial y offset
    int max_it = 100; // Maximum number of iterations
    
    SDL_Event e; // SDL event variable
    bool quit = false; // Flag to quit the application
    bool stopZoom = false; // Flag to stop zooming
    
    while (!quit) { // Main loop
        while (SDL_PollEvent(&e)) { // Poll for SDL events
            if (e.type == SDL_QUIT) { // Quit event
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) { // Mouse button down event
                stopZoom = true;
            }
        }

        if (!stopZoom) { // If zooming is not stopped
            scale *= 0.8; // Zoom in
            max_it = static_cast<int>(max_it * 1.05); // Increase the maximum number of iterations
            offsetX = -0.743643887037151; // Update the x offset
            offsetY = 0.131825904205330; // Update the y offset
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set background color to black
        SDL_RenderClear(renderer); // Clear the renderer
        renderMandelbrot(renderer, windowsize, scale, offsetX, offsetY, max_it); // Render the Mandelbrot set
        SDL_RenderPresent(renderer); // Present the rendered image
    }
    
    SDL_DestroyRenderer(renderer); // Destroy the renderer
    SDL_DestroyWindow(window); // Destroy the window
    SDL_Quit(); // Quit SDL

    return 0; // Return success
}
