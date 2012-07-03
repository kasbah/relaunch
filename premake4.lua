solution "launchSequencer"
   configurations { "Debug", "Release" }
	 links {"jack", "lo"}

project "launchSequencer"
  language "C++"
  kind     "ConsoleApp"
  files  { "**.h", "**.cpp" }
 
  configuration { "Debug*" }
    defines { "_DEBUG", "DEBUG" }
    flags   { "Symbols" }

  configuration { "Release*" }
    defines { "NDEBUG" }
    flags   { "Optimize" }
