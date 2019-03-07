## Bare-bones Ray Marching of Shaded Fractal Surfaces
##### CS 248 Final Project by Cristobal Sciutto (SUNETID: csciutto)

`make && ./render && open image.ppm`

Dependencies: `ImageMagick`

### Implemented Features
- Sphere SDF
- Greedy Ray-marching Algorithm: step until SDF is negative
- Normal-based Light Intensity
- Phong Reflectance

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
