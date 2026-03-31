
# How to build LovyanGFX for Windows.

### Enable clang in VisualStudio

 - Download `VisualStudio Community 2022` from the [VisualStudio website](https://visualstudio.microsoft.com/ja/vs/)
 - Add the following options when installing VisualStudio
 - Check ` Desktop development with C++ `
 - In the right pane, under ` Installation details `, check ` C++ Clang tools for Windows ` <br>

![VisualStudio_Installer](./images/vs_install.png)

---

### Obtain SDL or OpenCV and place them in a folder of your choice
 - For SDL, obtain `Development Libraries` from [this download page](https://www.libsdl.org/download-2.0.php) <br>
    ![sdl2_download_page](./images/sdl2_download_page.png)
 - For OpenCV, obtain `Windows` from `Releases` on [this download page](https://opencv.org/releases/) <br>
    ![opencv_download_page](./images/opencv_download_page.png)
 - For OpenCV, run the installer and extract to a location of your choice. In this example, it is extracted directly under C:\ (created at C:\opencv) <br>
    ![opencv_install_directory](./images/opencv_install_directory.png)
 - The sample CMakeLists uses the paths for `C:\SDL2` and `C:\opencv` as default values

---

### Edit environment variables and add paths
 - Environment Variables > User Variables > Select `Path` > Edit > New
 - For SDL2, add `C:/SDL2/lib/x64/` (adjust according to where you placed it)
 - For OpenCV, add `C:/opencv/build/x64/vc15\bin` (adjust according to where you placed it)
 - * Make sure the path location contains DLL files
 - * After adding the path, restart your PC (or log off and log back on) <br>
![env_path](./images/env_path.png)

---

### Adjust the CMakeLists.txt in the sample folder
 - Open CMakeLists.txt and adjust the paths and filenames to match your environment
 - For SDL2: `examples_for_PC/CMake_SDL/CMakeLists.txt`
 - For OpenCV: `examples_for_PC/CMake_OpenCV/CMakeLists.txt`
 - For OpenCV, the `.lib` filename includes a version number, so adjust it to match the version you obtained <br>
![cmakelists_edit](./images/cmakelists_edit.png)

---

### Launch VisualStudio and open the sample folder
 - Launch VisualStudio, click `Open a local folder`, and select the sample folder
 - For SDL2: examples_for_PC/CMake_SDL 
 - For OpenCV: examples_for_PC/CMake_OpenCV <br>
![vs_open](./images/vs_open.png)


 - In the build target selection box at the top of the screen, select `LGFX_SDL` or `LGFX_OpenCV`
 - Click `Debug` > `Start Debugging` <br>
![vs_build](./images/vs_build.png)
![vs_debug](./images/vs_debug.png)

 - Debugging in progress (sample code) <br>
![vs_debugging_example](./images/vs_debugging_example.png)

 - Stop Debugging<br>
![vs_debug_stop](./images/vs_debug_stop.png)

 - If you encounter build errors like the one below, or DLL-not-found errors at runtime, review the paths in your environment settings and the paths/filenames in CMakeLists.txt. Don't forget to restart (or log off and log back on) after changing environment settings.

    ```txt
    Severity Level  Code  Description  Project  File  Line  Suppression State
    Error    'C:/opencv/build/x64/vc15/lib/opencv_world452d.lib', needed by 'LGFXOpenCV.exe', missing and no known rule to make it  C:\sandbox\LovyanGFX\examples\Advanced\CMake_OpenCV\out\build\x64-Debug\CMake_OpenCV  C:\sandbox\LovyanGFX\examples\Advanced\CMake_OpenCV\out\build\x64-Debug\ninja  1
    ```
  ![vs_error_notfound_sdl](./images/vs_error_notfound_sdl.png)
  ![vs_error_notfound_opencv](./images/vs_error_notfound_opencv.png)

