%echo off
%VULKAN_SDK%/Bin/glslangValidator.exe -o BasicVert.spv -V Basic.vert
%VULKAN_SDK%/Bin/glslangValidator.exe -o BasicFrag.spv -V Basic.frag