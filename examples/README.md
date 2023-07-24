示例程序
========

该文件夹包括一些示例程序，可以在**根**目录下使用下面的命令构建所有的例子：

```bash
make examples
```

或者使用下面的命令构建特定的例子，以  `example_build_config.c` 在 Linux 下为例：

```bash
make target/example_build_config.out
```

大多数例子都使用 stdout 的 feature，设置如下环境变量将其启用：

```bash
# Linux
env MTFMT_BUILD_C_DEFS="-D_MSTR_USE_STD_IO=1"
```

或者

```powershell
# PowerShell
$Env:MTFMT_BUILD_C_DEFS="-D_MSTR_USE_STD_IO=1"
```

下表给出了目前可用的示例。

| 文件                                                                                                  | 描述                                     |
| :---------------------------------------------------------------------------------------------------- | :--------------------------------------- |
| [example_build_config.c](https://github.com/MtFmT-Lib/mtfmt/blob/master/examples/example_build_config.c) | 打印构建配置                             |
| [example_fmt_align.c](https://github.com/MtFmT-Lib/mtfmt/blob/master/examples/example_fmt_align.c)       | 格式化的对齐方式（左对齐、居中、右对齐） |
| [example_fmt_align.c](https://github.com/MtFmT-Lib/mtfmt/blob/master/examples/example_fmt_align.c)       | 不同进制下的整数格式化                   |
