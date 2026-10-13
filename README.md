
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)

**Point-based Individual Tree Delineation from 3D LiDAR Point Cloud Data**

## Table of Contents

- [Introduction](#introduction)
- [File Structure](#file-structure)
- [Prerequisites](#prerequisites)
- [Input Data](#input-data)
- [Parameter Settings](#parameter-settings)
- [Output Format](#output-format)
- [Authors](#authors)
- [License](#license)

## Introduction

This module implements a lightweight and easy-to-use point-based method for individual tree delineation from 3D point cloud data obtained by airborne laser scanning. The repository provides three equivalent implementations of the algorithm to suit different workflows and performance needs:

1. **C++ Version:** A pure C++ implementation consisting of a project generated from Visual Studio 2015. The core class for tree separation is named `FoxTree` and can be found in the respective `FoxTree.h` and `FoxTree.cpp` files.
2. **Python Sequential Version:** A modern Python re-implementation utilizing `scipy.spatial.cKDTree` for rapid nearest-neighbor and radius searches, processing files one by one with detailed execution time tracking.
3. **Python Parallel Version:** An optimized Python version designed for batch processing. It utilizes multi-core processing to run the separation algorithm on multiple `.xyz` files simultaneously, drastically reducing the total execution time for large datasets.

## File structure
This repository is organized as follows:

```text
.
├── Results/                 # Directory for output formatting and results
├── TestDatasets/            # Sample 3D LiDAR point cloud datasets
├── TreeSeparation/          # Main source code directory (Visual Studio 2015 project, FoxTree.h, FoxTree.cpp, etc.)
├── tree_ind.py              # Python implementation of tree individualization - Sequential version
├── tree_ind_parallelized.py         # Python implementation of tree individualization - Parallelized version
├── compare_efficiency.py    # Compares the execution time of Sequential vs. Parallel Python implementations. 
├── .gitignore               # Git ignore rules
├── LICENSE                  # LGPL-3.0 License
└── README.md                # Project documentation
```

## Prerequisites

(Choose either version/option)

For the **C++ version**, a 64-bit C++14 compiler (GCC, Clang or MSVC) and CMake are required. Build on Linux with:

```bash
cmake -S TreeSeparation/TreeSeparation -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

On Windows, install [MSYS2](https://www.msys2.org) (`winget install MSYS2.MSYS2`), then in the "MSYS2 UCRT64" shell install the toolchain and add `C:\msys64\ucrt64\bin` to your `PATH`:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
```

Then build from any terminal (`-static` makes `build\TreeSeparation.exe` run without MSYS2 on the `PATH`):

```bash
cmake -S TreeSeparation/TreeSeparation -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXE_LINKER_FLAGS=-static
cmake --build build
```

Avoid the legacy 32-bit MinGW.org compiler: it limits memory to 2–4 GB and ran about 2.7× slower on the test data.

For the **Python versions**, create the conda environment:

```bash
conda env create -f environment.yml
conda activate treeseparation
```

_(Note: The parallel version uses Python's built-in multiprocessing libraries, so no additional external dependencies are required)._

## Input Data

The input of this module is **TREE Points** only, as illustrated by the figures below.

![test-02](TestDatasets/test-02.png)

![Another test data](TestDatasets/test-03.png)

![One more test data](TestDatasets/test-04.png)

The format of the tree points is **_\*.xyz_**, such as:

```javascript {.line-numbers}
         x            y         z
     623772.9200 4834465.5900 77.7409
        ...         ...        ...
```

For the **Python implementations**, simply place your input `.xyz` files into the `0_Test_Datasets` directory. The scripts will automatically detect and process all point cloud files found within it.

**Note:** If the original data does not have color information, either initiate the last three columns with arbitrary integers or modify the code upon data loading.

## Parameter Settings

There are three parameters that have to be initialized for optimal individualization results:

1. **Searching radius**
2. **Vertical resolution**
3. **Minimum number of points per cluster**

**Hints on Parameter Settings:**

- **radius:** should be in accordance with the average point density (i.e., to ensure there is a sufficient number of points within the radius).
- **verticalResolution:** depends on the overall point density and the desired fineness of the results.
- **miniPtsPerCluster** defines the minimum number of points to define a cluster.

### C++ Usage

Pass the parameters on the command line _(Note that the parameters are based on geo-referenced point cloud data)_:

```bash
TreeSeparation input.xyz output.xyz radius verticalResolution miniPtsPerCluster direction
TreeSeparation test-02.xyz test-02-trees.xyz 1.0 0.7 3 1   # direction: 1 top-down, -1 bottom-up
```
### Python Usage

Pass the input and output files and the parameters as options (both scripts take the same options; see `--help`):

```bash
python tree_ind.py -i TestDatasets/test-02.xyz -o test-02-trees.xyz --radius 1.0 --vertical-resolution 0.7 --min-pts 3
python tree_ind_parallelized.py -i TestDatasets/test-02.xyz -o test-02-trees.xyz --radius 1.0 --vertical-resolution 0.7 --min-pts 3
```

Without `--input`, the scripts process every `.xyz` file in `TestDatasets` with the given (or default) parameters. The Python versions separate trees top-down only.

## Output Format

The output of this implementation is an ASCII format `*.xyz` file:

```text
treeID  x                  y                  z                   r    g    b
89      190248.7099609375  462600.2799987793  100.86000061035156  128  200  58
```

The first column is the **index of the tree** to which this point is assigned, followed by the point's coordinates exactly as read from the input. The last three columns are an RGB color per tree for direct visualization; it is derived from the tree index, so the C++ and Python versions write identical files.

Every input point is written exactly once: points in clusters smaller than the minimum size are assigned to the tree of their nearest assigned point. Layers are aligned to multiples of the vertical resolution, so tiles of one cloud processed with a buffer of at least the largest crown width (keeping trees whose top lies in the tile core) reproduce the untiled result. Write tiles without rounding coordinates; points sitting exactly on layer boundaries make results sensitive to tiny coordinate changes.

For the **Python versions**, outputs are automatically written to the `1_Individualized_trees` directory. The output filenames will automatically append your chosen parameters for easy tracking (e.g., `filename_1.0_1.0_5.xyz`).

The individual tree delineation results are given as the figures below:
![Individual tree delineation results](Results/test-02-results-1.0-0.7-3.png)
![Individual tree delineation results](Results/test-02-results-1.0-0.7-3_01.png)
![Individual tree delineation results](Results/test-03-results-1.0-0.5-3.png)
![Individual tree delineation results](Results/test-04-results-1.0-0.8-5.png)

In the C++ version, **nanoflann** is employed for **_KNN_** searching, which can be found from here [link](https://github.com/jlblancoc/nanoflann).

## Authors

Should you have any questions, comments, BUG(s) reporting, or IDEAS for further improvements? Please contact the authors

- **Jinhu Wang**
  jinhu.wang (at) hotmail.com
- **Roderik Lindenbergh**
  r.c.linderbergh (at) tudelft.nl
  http://doris.tudelft.nl/~rlindenbergh/

## License

This project is licensed under the LGPL-3.0 License - see the [LICENSE](LICENSE) file for details.

