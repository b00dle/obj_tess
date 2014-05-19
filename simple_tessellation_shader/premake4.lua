

local solutionName = "simple_tessellation_shader_" .. _ACTION

-- Setup a simple solution
solution(solutionName)
   configurations { "Debug", "Release" }
 
   -- One project in the solution
   project(solutionName)
      kind "ConsoleApp"
      language "C++"
      targetname "simple_tessellation_shader"
      
      -- include all cpp, h, and shader files in the solution directory in the project
      files { "*.h", "*.cpp","*.glsl" }

      -- setup standard defines
      defines { "WIN32", "_CONSOLE", "_WINDOWS" }

      -- setup include and lib directories (relative paths are preserved by premake)
      includedirs { "../sdk_swak" }
      libdirs { "../sdk_swak" }

      -- setup library files
      links { "opengl32", "glu32","sdk_swak", }

      -- no precompiled headers 
      flags { "NoPCH" }

      -- Add the standard file type filters to the project, more complex things are possible
      vpaths { ["Header Files"] = { "**.h"} }
      vpaths { ["Source Files"] = { "**.cpp"} }
	  vpaths { ["Shader Files"] = { "**.glsl"} }
 
      -- Setup debug configuration
      configuration "Debug"
         -- Settings add to previous options
         defines { "_DEBUG" }
         flags { "Symbols" }
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }    

