## Bare-bones Ray Marching of Shaded Fractal Surfaces
##### CS 248 Final Project by Cristobal Sciutto (SUNETID: csciutto)

`make && ./render && open scene.gif`

Dependencies: `ImageMagick`, for GIF generation

### TODO
- Submit to cluster for efficiency
- Rotation in 3-space

```
.
├── images           // rendering examples
└── src
    ├── render.cpp   // main ray marching
    ├── sdf.cpp/h    // definition of signed distance functions
    ├── animate.h    // camera animation api
    ├── threading.h  // concurrency primitives
    ├── Mat3.h       // matrix implementation
    ├── Vec3.h       // vector implementation
    └── utils.h      // helper functions
```

### Implemented SDFs
- Primitives: Sphere, Cube, Plane
- Displacement with sinusoids
- Unions, Intersects, Differences
- Repeated primitives with modulus
- Menger Sponge of arbitrary dimension

### Implemented Features
- Mat3 and Vec3 implementations
- Greedy Ray-marching Algorithm: step until SDF is negative
- Normal-based Light Intensity
- Phong Reflectance
- ThreadPool and Semaphore implementations (based on CS110)
- Parallel rendering of images with ThreadPool
- Soft Shadows via Inigo Quilez
- Multiple light sources
- Light attenuation
- Supersampling for Anti-aliasing
- Camera movement API (translation and rotation)

### Motivation
- Coding graphics code from scratch, without the abstractions of OpenGL APIs
- Mathematical nature of signed distance functions (SDFs)

### Deliverables
- Documented ray marching source code
- Capable of general-purpose primitive rendering
- Soft shadows and penumbra
- Complex fractal structure SDF demo
- Final video exemplifying work

### Sources
- Ray Marching and SDFs: <http://jamie-wong.com/2016/07/15/ray-marching-signed-distance-functions/>
- TinyKaboom: <https://github.com/ssloy/tinykaboom/wiki/KABOOM!-in-180-lines-of-code>
- Fractal SDFs: <http://blog.hvidtfeldts.net/index.php/2011/06/distance-estimated-3d-fractals-part-i/>
- Inigo Quilez Ray Marching: <https://www.iquilezles.org/www/articles/raymarchingdf/raymarchingdf.htm>
