# Image Editor

## Installation and Run

This project is utilizing SFML.

```
git clone https://github.com/d3clane/MyPhotoshop
make
./build/bin/ps.out
```

## Project Goal

Main goal of the project - get used to working in team and with plugins architecture.

First step of the project was to come up with the standard(API) in our studying group and approve it together. It is an API about how plugins and photoshop program interact with each other, so that we can simply code plugins independently of concrete photoshop implementation and they would definitely be able to run and work as expected. 

As expected, first API was not perfect and there were some limitations we could do using these standard. So, after some time we came up with a second standard and approved it.

## API Architecture

The main idea of the API - everything is a plugin. Like, literally, everything. Of course, some of them are system plugins - like canvas, toolbar, menu bar, but they are still plugins. Plugin can ask photoshop to give a pointer to the concrete window on the screen and use this pointer to interact with it. For example brush plugin asks a pointer to canvas to draw on it and a pointer to toolbar to add himself there.

In this paradigm, plugins can do roughly anything in the photoshop, they are not really bounded by any limitations - and that's something that we wanted and what we achieved, as I believe.

Each plugin have an action - something, that have to be executed on each polled event. This actions are executed using `Action Controller` so that some of them could be undoable, e.g undo of the drawing on the screen or applying filter. 

Obviously, API should not depend on the particular graphics library (for example SFML) and therefore API also standardise common graphics shell - so that they could be implemented using different graphics library and switching from one to another doesn't lead to rewriting whole project - we only need to rewrite implementation of graphics functions.

## Photoshop Features

Here how it looks like, design is inspired by Krita:

![Image editor](ReadmeAssets/ImageEditor.png)

So, what are the main features?

Firstly, screen is divided into:
1. Canvas
2. Toolbar - left bar with eraser, brush, etc.
3. Menu bar - upper bar with file, tools, filters, etc.
4. Options bar - options of the concrete tool - upper part with the size slide and right bar that contains colors (is hidden by brightness filter render window).

Let's dive into each of the parts more deeply.

### Canvas

Canvas is pretty simple.

First of all, it is scrollable, it works the same as in other GUI applications.

Secondly, it is "mementable" - you can take a snapshot and restore canvas from it at any time. Therefore canvas can be considered "undoable" - using `Action Controller` different plugins can undo their actions and interactions with canvas.

### Toolbar

Simple instruments are expected to be added to toolbar. In my concrete case I implemented

1. Brush
2. Eraser
3. Shapes - line, ellipse, rectangle

Implementing brush and eraser was harder because it required interpolation. The problem is that events are sent not as often as I needed to draw continuous line. Therefore I needed to add points to the interpolator (I used Catmull-Rom interpolation) and then draw line using interpolated points.

### Options bar

Instruments from toolbar can have their own options for more precise tuning of it. There are a few standardised options:
1. Thickness
2. Opacity
3. Color palette

Concrete plugin can choose which of them to add to the option bar. Also it could create it's own options and add them.

### Menu bar

Menu bar defines common buttons:
1. File
2. Tools
3. Layer
4. Filters
5. Help

File provides two main functions - open file and save file.
Tools provides buttons "Do" and "Undo" to interact with the canvas.

The most interesting part - filters. I implemented 5 filters:
1. Negative
2. Box Blur
3. Bas Relief
4. Unsharp mask
5. Brightness filter - on the screenshot.

Each filter opens it's own render window with options and "Ok / Cancel" buttons. 

For example, brightness filter provides interactive graph to adjust brightness of the canvas pixels. Another example - box blur provides slider to choose the blur radius:

![Box blur](ReadmeAssets/boxBlur.png)

## Conclusion

If you wish to try to code a plugin using our API - it is provided in [api](/include/api/) folder.
