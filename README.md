# my-LightCuts

Implementation of Lightcuts algorithm

[Link to github](https://github.com/KiK0S/my-lightcuts)

## Build

This is a project on codebase of MyRenderer, so building is the follows:

```
mkdir build
cd build
cmake ..
cd ..
cmake --build build
```


## Run

You can run an arbitrary scene, but scenes with lightcuts configuration are these 3 demos:

```
build/MyRenderer desk
build/MyRenderer desk-red
build/MyRenderer bedroom
```

## Demo, benchmarks

|Demo name|Rendering method|Time elapsed|Lights evaluated|Image|
|:---:|:---:|:---:|:---:|:----:|
|bedroom|Rasterizer| 0s | 216 | ![](img/bedroom-hq-0.png)|
|bedroom|Ray tracer| 1h26m | 100 | ![](img/bedroom-hq-1.png)|
|bedroom|Lightcuts| 1h47m | 84.6 | ![](img/bedroom-hq-2.png)|
|bedroom|Lightcuts, only diffuse bounded| 1h58m | 94.4 | ![](img/bedroom-hq-3.png)|
|bedroom|Lightcuts + sampling | 1h41m | 76.0 | ![](img/bedroom-hq-4.png)| 
|desk|Rasterizer| 0s | 216 | ![](img/desk-0.png)|
|desk|Ray tracer| 146s | 216 | ![](img/desk-1.png)|
|desk|Lightcuts| 416s | 107.4 | ![](img/desk-2.png)|
|desk|Lightcuts, only diffuse bounded| 490s | 126.2 | ![](img/desk-3.png)|
|desk|Lightcuts + sampling | 462s | 107.7 | ![](img/desk-4.png)|
|desk-red|Rasterizer| 0s | 216 | ![](img/desk-red-0.png)|
|desk-red|Ray tracer| 93s | 216 | ![](img/desk-red-1.png)|
|desk-red|Lightcuts| 415s | 159.1 | ![](img/desk-red-2.png)|
|desk-red|Lightcuts + sampling| 393s | 132.3 | ![](img/desk-red-3.png)|
