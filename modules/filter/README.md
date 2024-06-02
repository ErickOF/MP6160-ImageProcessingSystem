# **Filter Module**

## **Filter Algorithm: Mean Filter**
The mean filter algorithm replaces each pixel value with the average of all
pixel values in an N by N window (N = 3, 5, 7, etc), where the pixel to
replace is the centering pixel. Assuming that the centering pixel is in (0, 0)
position:

$
img(row, col) = \sum_{i = -N/2}^{N/2} {\sum_{j = -N/2}^{-N/2} {img(row + i, col + j)}}
$

Also, it can computed using a convolution like:

$
img(row, col) = \begin{bmatrix}
  \frac{1}{N^{2}} & \dots  & \frac{1}{N^{2}} \\
  \vdots          & \ddots & \vdots          \\
  \frac{1}{N^{2}} & \dots  & \frac{1}{N^{2}}
\end{bmatrix}
\ast
\begin{bmatrix}
  img(row - N/2, col - N/2) & \dots  & img(row + N/2, col + N/2) \\
  \vdots                    & \ddots & \vdots                     \\
  img(row + N/2, col - N/2) & \dots  & img(row + N/2, col + N/2)
\end{bmatrix}
$


## **Usage**

A Makefile is used to compile the testbench and filter module. There are some
Makefile targets to compile the different modules and tests. By default, the
PV model is compiled. The available test options are:

* `TEST_MODE_ONE_WINDOW_NORMAL`: Runs only one window with fixed values.
* `TEST_MODE_ONE_WINDOW_RANDOM`: Runs only one window with random values.

Some additional options can used to debug and dump the waveform:

* `IPS_DEBUG_EN`: Prints the kernel, each window value, and the result.
* `IPS_DUMP_EN`: Creates a VCD file with each value of the window and kernel,
 and the result.

### **Compilation**
Runs the `make` command with the corresponding switches to compile.

For instance, to run only one window in debug mode and dump the signals:

```shell
make TEST_MODE_ONE_WINDOW_RANDOM=1 IPS_DUMP_EN=1 IPS_DEBUG_EN=1
```

### **Run**

```shell
make run
```

### **Open Waveform**
```shell
make waveform
```
