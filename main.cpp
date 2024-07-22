#include <iostream> //standard input/output stream library
#include </usr/local/opt/libomp/include/omp.h> //openmp library
#include <SDL2/SDL.h> //SDL2 library for graphics rendering
#include <numeric> //library for numeric operations
#include <complex> //library for complex numbers

//function to determine if a complex number is in the Mandelbrot set
int mset(std::complex<double> c, int max_it) {
    std::complex<double> z(0, 0); //initialize z to 0 + 0i
    for (int i = 0; i < max_it; i++) { //iterate max_it times
        z = std::pow(z, 2) + c; //the mandelbrot euqation, z = z^2 + c
        if (std::norm(z) > 10) { //if z iterates more than 10 times, consider it not part of the mandelbrot set
                    //norm is used because it saves the computation of the square root but still gives a good enough result for the program
            return i; //return the iteration count
        }
    }
    return 0; //return 0 if z lies within max_it
}

//function to render the Mandelbrot set using SDL
void renderMandelbrot(SDL_Renderer* renderer, int windowsize, double scale, double offsetX, double offsetY, int max_it) {
    int num_threads = omp_get_max_threads(); //get the maximum number of threads available
    omp_set_num_threads(num_threads); //set the number of threads using OpenMP

    #pragma omp parallel for schedule(dynamic) collapse(2) //parallelize window rendering with OpenMP
                                            //divide dynamically among the threads. collapse makes two loops into acting like one bigger loop
    for (int x_point = 0; x_point < windowsize; x_point++) { //loop over x-coordinates
        for (int y_point = 0; y_point < windowsize; y_point++) { //loop over y-coordinates
            //transform screen coordinates to complex plane coordinates
            double x = scale * (x_point - windowsize / 2.0) + offsetX;
            double y = scale * (y_point - windowsize / 2.0) - offsetY;
            int times = mset(std::complex<double>(x, y), max_it); //call mset with a complex number and return the iterations this complex number took
            
            //if the iterations (times) is zero, the value is inside the set
            if (times == 0) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //render the colour inside the set: black
            } else { //if the iterations (times) isn't zero, the value is outisde the set
                //and the color depends on how many iterations before escape
                int red = (times * 6) % 256;
                int green = (times * 12) % 256;
                int blue = (times * 14) % 256;
                SDL_SetRenderDrawColor(renderer, red, green, blue, 255); //render the colour outside the set: based on iterations
            }
            
            #pragma omp critical //ensure thread safety for rendering, only one thread can draw at a time
            SDL_RenderDrawPoint(renderer, x_point, y_point); //draw the point
        }
    }
}


int main() {
    SDL_Init(SDL_INIT_EVERYTHING); //initialize SDL

    int windowsize = 500; //define the window size
    SDL_Window* window = SDL_CreateWindow("Mandelbrot Set", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowsize, windowsize, SDL_WINDOW_SHOWN); //create the SDL window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //create the renderer

    double scale = 3.0 / windowsize; //define the initial scale for the mandelbrot compared to window size
    double offsetX = -0.5; //initial x offset, -0,5 so the picture is slightly to the left
    double offsetY = 0.0; //initial y offset
    int max_it = 100; //define maximum number of iterations
    
    SDL_Event e; //create an SDL event
    bool quit = false; //to quit the application
    bool stopZoom = false; //to stop zooming
    
    while (!quit) { //while the application isn't quit
        while (SDL_PollEvent(&e)) { // check for SDL events
            if (e.type == SDL_QUIT) { //quit
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) { //if the mouse is pressed
                stopZoom = true; //stop zooming
            }
        }

        if (!stopZoom) { //if zooming is not stopped
            scale *= 0.8; //zoom in
            max_it = static_cast<int>(max_it * 1.05); //increase the maximum number of iterations when zooming for detail
            offsetX = -0.743643887037151; //update the x offset, choose the point where to zoom in
            offsetY = 0.131825904205330; //same for y
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //set background color to black
        SDL_RenderClear(renderer); //clear the renderer
        renderMandelbrot(renderer, windowsize, scale, offsetX, offsetY, max_it); //render the Mandelbrot set
        SDL_RenderPresent(renderer); //present the rendered image
    }
    
    //when !quit isn't true:
    SDL_DestroyRenderer(renderer); //destroy the renderer
    SDL_DestroyWindow(window); //destroy the window
    SDL_Quit(); //quit SDL

    return 0;
}
