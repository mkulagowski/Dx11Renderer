from conans import ConanFile, CMake

class Engine(ConanFile):
  settings = "os", "compiler", "build_type", "arch"
  name = "Engine"
  version = "1.0"
  description = "Dx11 engine"
  author = "mkulagowsk"
  generators = "cmake"
  requires = "libpng/1.6.36@bincrafters/stable"