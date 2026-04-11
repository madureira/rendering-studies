from conan import ConanFile
from conan.tools.cmake import cmake_layout


class RenderingStudies(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("freetype/2.13.3")
        self.requires("glfw/3.4")

    def layout(self):
        cmake_layout(self)
