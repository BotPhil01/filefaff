workspace "filefaff"
     configurations { "debug", "release" }

    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    objdir "build/%{cfg.buildcfg}"
    includedirs { "./include" }

    files { "include/**.h", "src/**.cpp" }
    removefiles { "src/client.cpp" }
    removefiles { "src/server.cpp" }
    removefiles { "src/test.cpp" }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"

project "filefaffserver"
    files { "src/server.cpp" }

project "filefaffclient"
    files { "src/client.cpp" }

project "filefafftest"
    files { "src/test.cpp" }
