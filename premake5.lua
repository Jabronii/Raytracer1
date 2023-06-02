-- premake5.lua
workspace "Raytracer1"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Raytracer1"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "Raytracer1"