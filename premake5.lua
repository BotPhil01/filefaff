workspace "filefaff"
     configurations { "debug", "release" }

project "filefaff"
    kind "ConsoleApp"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "build/%{cfg.buildcfg}"
    includedirs { "./include" }

    files { "include/**.h", "src/**.c" }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"
