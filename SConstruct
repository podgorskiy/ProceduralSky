import fnmatch
import os
import time
import atexit
from SCons.Defaults import *

release = True

if(release):
	optimization = '-Os'
	debug = '-g0'
	lto = "1"
	closure = "1"
	assertions = "0"
	demangle = "0"
else:
	optimization = '-O0'
	debug = '-g3'
	lto = "0"
	closure = "0"
	assertions = "2"
	demangle = "1"


def main():
	env = Environment(ENV = os.environ, tools = ['gcc', 'g++', 'gnulink', 'ar', 'gas'])
		
	env.Replace(CC     = "emcc"    )
	env.Replace(CXX    = "em++"    )
	env.Replace(LINK   = "emcc"    )
	
	env.Replace(AR     = "emar"    )
	env.Replace(RANLIB = "emranlib")
	
	env.Replace(LIBLINKPREFIX = "")
	env.Replace(LIBPREFIX = "")
	env.Replace(LIBLINKSUFFIX = ".bc")
	env.Replace(LIBSUFFIX = ".bc")
 	env.Replace(OBJSUFFIX  = ".o")
 	env.Replace(PROGSUFFIX = ".js")
	
	env.Append( CPPFLAGS=[optimization] )
	env.Append( LINKFLAGS=[
		optimization,
		debug,
		"-s", "ASSERTIONS=" + assertions,
		"-s", "DEMANGLE_SUPPORT=" + demangle,
		"-s", "TOTAL_MEMORY=600000000",
		"--llvm-lto", lto,
		"--closure", closure,
		"-s", "NO_EXIT_RUNTIME=1",
		"-s", "DISABLE_EXCEPTION_CATCHING=1",
		"-s", "EXPORTED_FUNCTIONS=\"['_main','_resizeModule','_putchar']\"",
		"--preload-file", "data"
	] )

	timeStart = time.time()
	atexit.register(PrintInformationOnBuildIsFinished, timeStart)
	
	Includes = [
		"sources/SBFramework",
		"libs/glm",
		"libs/imgui",
		"libs/pugixml/src",
		"libs/SimpleText/include",
		"libs/lz4/lib"
	]
	
	imguiSources = [
		"libs/imgui/imgui.cpp",
		"libs/imgui/imgui_demo.cpp",
		"libs/imgui/imgui_draw.cpp"
	]
	
	lz4Sources = [
		"libs/lz4/lib/lz4.c",
		"libs/lz4/lib/lz4hc.c",
		"libs/lz4/lib/xxhash.c"
	]
	
	pugiSources = [
		"libs/pugixml/src/pugixml.cpp"	
	]
	
	sourcesPath = "sources"
	files = GlobR(sourcesPath, "*.cpp")
	
	env.Library('pugi', pugiSources)
	env.Library('imgui', imguiSources)
	env.Library('lz4', lz4Sources)
	
	program = env.Program('ProceduralSky', files, LIBS=['imgui', 'pugi', 'lz4'], CPPFLAGS=[optimization, '-std=c++11', debug], LIBPATH='.', CPPPATH = Includes)
	
	env.Depends(program, GlobR("data", "*"))
	
def PrintInformationOnBuildIsFinished(startTimeInSeconds):
	""" Launched when scons is finished """
	failures = GetBuildFailures()
	for failure in failures:
		print "Target [%s] failed: %s" % (failure.node, failure.errstr)
	timeDelta = time.gmtime(time.time() - startTimeInSeconds)
	print time.strftime("Build time: %M minutes %S seconds", timeDelta)
	
def GlobR(path, filter) : 
	matches = []
	for root, dirnames, filenames in os.walk(path):
  		for filename in fnmatch.filter(filenames, filter):
   			matches.append(os.path.join(root, filename)) 
	return matches

if __name__ == "SCons.Script":
	main()
