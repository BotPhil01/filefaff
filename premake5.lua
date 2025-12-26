workspace "filefaff"
     configurations { "debug", "release" }

    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    objdir "build/%{cfg.buildcfg}"
    includedirs { "./include" }

    files { "include/**.h", "src/**.cpp" }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"

project "filefaffserver"
    removefiles { "src/client.cpp" }

project "filefaffclient"
    removefiles { "src/server.cpp" }
