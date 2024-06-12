# **SystemC Image Data Generation Tool for Project Testbench**

This tool will help to generate the required data for the testbench. It
generates grayscale, Sobel x, Sobel y, and combined Sobel x-y images.


## **Installation**

Before running, make sure you have installed Python OpenCV. You can run:
```shell
cd tools/datagen/
chmod +x install_pyopencv.sh
./install_pyopencv.sh
```

It will display the OpenCV version at the end of the installation.


## **Usage**

There are two versions of the script, a JupyterNotebook and a normal Python
script. For both of them, in the main function, the variables `img_name` and
`img_ext` must be updated. The `img_name` is only the name of the image that
is saved in `tools/datagen/src/imgs` and the `img_ext` is the extension of the
image. The `salt_probability` is used to set the probability of getting a salt
pixel in the image, while `pepper_probability` is used for the pepper pixels.
These two variables can be set to 0.0 to generate an image without noise. For
instance:

```python
salt_probability: float = 0.1
salt_probability: float = 0.1
img_name: str = 'car'
img_ext: str = 'jpg
```

will load `tools/datagen/src/imgs/car.jpg` image and generate:

* tools/datagen/src/imgs/car\_grayscale\_image.jpg
* tools/datagen/src/imgs/car\_noisy\_image.jpg
* tools/datagen/src/imgs/car\_sobel\_x\_result.jpg
* tools/datagen/src/imgs/car\_sobel\_y\_result.jpg
* tools/datagen/src/imgs/car\_sobel\_combined\_result.jpg

These generated images are ignored and must not be committed to the repository.


### **Python Script**

To run the Python script:

```shell
cd tools/datagen/
python3 datagen.py
```


### **JupyterNotebook**

You can open the file from the JupyterNotebook app or upload it to
[Google Colab](https://colab.research.google.com/).

