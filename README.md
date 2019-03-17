## Bare-bones Ray Marching of Shaded Fractal Surfaces

![](images/process/final.gif)

A project report is available at `/docs/report.pdf`, explaining some of the techniques explored in this project.

`make && ./render && open scene.gif`

```
├── images
│   ├── expo         // rendering highlights
│   └── process      // process images
└── src
    ├── render.cpp   // main ray marching
    ├── sdf.cpp/h    // definition of signed distance functions
    ├── animate.h    // camera animation api
    ├── threading.h  // concurrency primitives
    ├── Mat3.h       // matrix implementation
    ├── Vec3.h       // vector implementation
    └── utils.h      // helper functions
```

Dependencies: `ImageMagick`, for GIF generation

### Implemented SDFs
- Primitives: Sphere, Cube, Plane
- Displacement with sinusoids
- Unions, Intersects, Differences
- Repeated primitives with modulus
- Menger Sponge of arbitrary dimension

### Implemented Features
- Mat3 and Vec3 implementations
- Ray-marching Algorithm: step until SDF is negative
- Normal-based Light Intensity
- Phong Reflectance
- ThreadPool and Semaphore implementations (based on CS110)
- Parallel rendering of images with ThreadPool
- Soft Shadows via Inigo Quilez
- Multiple light sources
- Light attenuation
- Supersampling for Anti-aliasing
- Camera movement API (translation, pan, and rotation)

### Motivation
- Coding graphics code from scratch, without the abstractions of OpenGL APIs
- Mathematical nature of signed distance functions (SDFs)

### Sources
- Ray Marching and SDFs: <http://jamie-wong.com/2016/07/15/ray-marching-signed-distance-functions/>
- TinyKaboom: <https://github.com/ssloy/tinykaboom/wiki/KABOOM!-in-180-lines-of-code>
- Fractal SDFs: <http://blog.hvidtfeldts.net/index.php/2011/06/distance-estimated-3d-fractals-part-i/>
- Inigo Quilez Ray Marching: <https://www.iquilezles.org/www/articles/raymarchingdf/raymarchingdf.htm>
