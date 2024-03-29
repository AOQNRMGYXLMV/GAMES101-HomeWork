
# Assignment 1

[Lecture 04](https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_04.pdf)

## 参数说明

```cpp
Eigen::Matrix4f get_model_matrix(float rotation_angle)
```
中的`rotation_angle`为角度制，在计算时需转化为弧度制

```cpp
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
```
中的`eye_fov`为水平视野宽度，`aspect_ratio`为视锥的宽高比
编程中采用**右手坐标系**，眼睛向z轴的负方向看去
传入的`zNear`和`zFar`为正，故取**相反数**

## 透视矩阵

    $$
    M_{perspective}=
        \begin{pmatrix}
        n & 0 & 0 & 0 \\
        0 & n & 0 & 0 \\
        0 & 0 & n+f & -nf \\
        0 & 0 & 1 & 0
        \end{pmatrix}
    $$

## 课后思考

### 透视变换对于`near`和`far`平面之间的点，变换之后`z`坐标是如何变化的？

设两平面中间一点为$(x, y, z, 1), (f < z < n)$
对这一点应用透视变换后

$$
    \begin{pmatrix}
    n & 0 & 0 & 0 \\
    0 & n & 0 & 0 \\
    0 & 0 & n+f & -nf \\
    0 & 0 & 1 & 0
    \end{pmatrix}
    \begin{pmatrix}
    x  \\
    y  \\
    z  \\
    1 
    \end{pmatrix}
    =
    \begin{pmatrix}
    nx  \\
    ny  \\
    (n+f)z-nf  \\
    z 
    \end{pmatrix}
    =
    \begin{pmatrix}
    \cfrac{nx}{z}  \\
    \cfrac{ny}{z}  \\
    (n+f)-\cfrac{nf}{z} \\
    1 
    \end{pmatrix}
$$

将z坐标相减得到：

$$
(n+f)-\cfrac{nf}{z} - z = \cfrac{(n+f)z-nf-z^2}{z}
$$

