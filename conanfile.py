from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class mtfmtRecipe(ConanFile):
    name = "mtfmt"
    version = "0.1.0"

    # Optional metadata
    license = "GPLv3"
    author = "XiangYyang ", "HalfSweet HalfSweet@HalfSweet.cn"
    url = "https://github.com/MtFmT-Lib/mtfmt"
    description = ""
    topics = ("")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "inc/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        del self.settings.compiler.cppstd

        if(self.settings.compiler != "msvc"):
            del self.settings.compiler.libcxx

        if self.options.shared:
            self.options.rm_safe("fPIC")


    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["mtfmt"]

    

    

