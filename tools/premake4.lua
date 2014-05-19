

-- Setup a simple solution
solution "pm_octree_viewer"
   configurations { "Debug", "Release" }
 
   -- One project in the solution
   project "pm_octree_viewer"
      kind "ConsoleApp"
      language "C++"
      
      -- include all cpp and h files in the solution directory in the project
      files { "*.h", "*.cpp" }

      -- setup standard defines
      defines { "WIN32", "_CONSOLE" }

      -- setup include and lib directories (relative paths are preserved by premake)
      includedirs { "../../external/include", "../../common/include" }
      libdirs { "../../external/lib" }

      -- setup library files
      links { "opengl32", "glu32", "glut32", "xinput" }

      -- no precompiled headers and no runtime type info
      flags { "NoRTTI", "NoPCH" }

      -- Add the standard file type filters to the project, more complex things are possible
      vpaths { ["Header Files"] = { "**.h"} }
      vpaths { ["Source Files"] = { "**.cpp"} }
 
      -- Setup debug configuration
      configuration "Debug"
         -- Settings add to previous options
         defines { "_DEBUG" }
         flags { "Symbols" }
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }    

