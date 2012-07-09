solution "relaunch"
   configurations { "Debug", "Release" }
	 links {"jack", "asound", "pthread"}

project "relaunch"
  language "C++"
  kind     "ConsoleApp"
  files  { "**.h", "**.cpp" }
 
  configuration { "Debug*" }
    defines { "_DEBUG", "DEBUG" }
    flags   { "Symbols" }
		buildoptions {"-fpermissive"}

  configuration { "Release*" }
    defines { "NDEBUG" }
    flags   { "Optimize" }
		buildoptions {"-fpermissive"}
