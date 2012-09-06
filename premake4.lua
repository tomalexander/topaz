solution("topaz")
configurations({"Debug", "Release", "Static", "StaticDebug"})
flags {"NoRTTI"}

configuration("Debug")
flags({"Symbols"})

configuration("Release")
flags({"OptimizeSpeed"})

configuration "Static"
flags({"OptimizeSpeed"})
flags({"StaticRuntime"})
defines({"SFML_STATIC", "GLEW_STATIC"})

configuration "StaticDebug"
flags({"Symbols"})
flags({"StaticRuntime"})
defines({"SFML_STATIC", "GLEW_STATIC"})

configuration "windows"
linkoptions({"-static"})

configuration("linux")
libdirs({"/usr/local/lib/"})

configuration({})

function link_library_deps()
   configuration "windows"
   links({"oolua", "lua", "physfs", "z", "pthread"})
   configuration {"linux", "Debug or Release"}
   links({"oolua", "lua", "physfs", "z", "pthread", "dl"})
   configuration {"linux", "Static*"}
   linkoptions {"/usr/local/lib/liboolua.a", "/usr/local/lib/liblua.a", "/usr/local/lib/libphysfs.a", "/usr/local/lib/libz.a", "-pthread", "/usr/lib/libdl.a"}
   links {"c"}
   configuration {}

   link_sfml()
   link_opengl()
end

function link_sfml()
   configuration "windows"
   links {"glew32"}
   configuration {"linux", "Debug or Release"}
   links {"GLEW"}
   configuration {"linux", "Static*"}
   linkoptions {"/usr/lib64/libGLEW.a"}
   configuration {}

   configuration "Debug or Release"
   links {"sfml-window", "sfml-graphics", "sfml-audio", "sfml-network", "sfml-system"}
   configuration {"linux", "Static*"}
   linkoptions {"/usr/local/lib/libsfml-window-s.a", "/usr/local/lib/libsfml-graphics-s.a", "/usr/local/lib/libsfml-audio-s.a", "/usr/local/lib/libsfml-network-s.a", "/usr/local/lib/libsfml-system-s.a"}
   configuration {"windows", "Static*"}
   links {"sfml-window-s", "sfml-graphics-s", "sfml-audio-s", "sfml-network-s", "sfml-system-s"}
   configuration {}

   configuration "windows"
   links {"jpeg"}
   configuration {"linux", "Debug or Release"}
   links {"rt", "jpeg"}
   configuration {"linux", "Static*"}
   linkoptions {"/usr/lib/librt.a", "/usr/lib/libjpeg.a"}
   configuration {}
end

function link_opengl()
   configuration "windows"
   links({"opengl32", "gdi32", "winmm", "user32"})
   configuration {"linux", "Debug or Release"}
   links({"GL", "Xrandr", "Xrender", "SM", "ICE", "Xext", "X11"})
   configuration {"linux", "Static*"}
   links {"GL", "X11"}
   linkoptions {"/usr/lib/libXrandr.a", "/usr/local/lib/libXrender.a", "/usr/lib/libSM.a", "/usr/lib/libICE.a", "/usr/local/lib/libXext.a"}
   configuration({})
end

project("wold")
kind("WindowedApp")
language("C++")
buildoptions({"-std=c++0x"})
link_library_deps();
files({"src/libmain.cpp", "src/egg_parser.cpp", "src/util.cpp", "src/model.cpp", "src/quaternion.cpp", "src/sqt.cpp", "src/matrix.cpp", "src/panda_node.cpp", "src/shaders.cpp", "src/vector.cpp", "src/camera.cpp", "src/free_view_camera.cpp", "src/light.cpp", "src/nolight.cpp", "src/lookat_camera.cpp", "src/uberlight.cpp", "src/floor_model.cpp", "src/unit.cpp", "src/joint.cpp", "src/animation.cpp", "src/vertex.cpp", "src/gl_program.cpp", "src/rigidbody.cpp", "src/gameobject.cpp", "src/collider.cpp", "src/sphere_collider.cpp", "src/sphere_primitive.cpp", "src/collision.cpp", "src/aabb_collider.cpp", "src/box_primitive.cpp", "src/main.cpp"})
