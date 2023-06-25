---
typora-root-url: ..\
---

本页面简要描述了在各个环境下添加该库的方法。

[toc]

# 1 下载源代码

本小节给出了可行的下载源代码的方式。最简单的下载方式是单击[这个链接](https://github.com/MtFmT-Lib/mtfmt/archive/refs/heads/master.zip)，它定向到 Github 的主分支上，可以直接从[仓库](https://github.com/MtFmT-Lib/mtfmt)下载 zip 文件。此外，还可使用 git 进行克隆。

```bash
git clone --depth=1 git@github.com:MtFmT-Lib/mtfmt.git
# 或者: https://github.com/MtFmT-Lib/mtfmt.git
```

在使用 git 的情况下，还可考虑使用子模块。

```bash
git submodule add git@github.com:MtFmT-Lib/mtfmt.git
```

# 2 添加到工程

本小节描述了各个环境下添加库的方法。在下载下来的源代码包中包括两个文件夹。

1. `src` 文件夹，其中包含所有需要的源代码文件
2. `inc` 文件夹，其中包含所有的头文件，需要注意的是 C++ 版本的头文件也在其中，其以 `.hpp` 结尾。对于 C 应用来说，不需要这部分。

如果将源代码添加到工程，可行的方式之一是将这两个文件夹复制到自己的工程目录下。

## 2.1 Keil 环境

库提供了 CMSIS-PACK 支持，可以从源代码添加，也可以直接导入包到 Keil 的包管理器中。

### 2.1.1 使用包管理器

库提供的 CMSIS-PACK 是由 Github action 自动生成的。在[该页面](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/cmsis-pack.yml)找到分支名为 `master` 的项并下载压缩包，其中即是打包好的 CMSIS-PACK 文件。在 Keil 中，打开包管理器。

![图 2.1 打开包管理器](./img/keil_pkg_manager.png)

将下载好的文件解压出来，压缩包中唯一的文件即为 CMSIS-PACK 的文件，其以 `.pack` 后缀名结尾。接着，在包管理器中手动导入该文件。

![图 2.2 手动导入包文件](./img/keil_import.png)

导入完成后可以看到名为 mtfmt 的包。

![图 2.3 导入完成后的包](./img/keil_pkg_installed.png)

现在可以退回到自己的工程下了，在自己的工程下配置并勾选相关的包，如[图2.4](#figure_2_4)。

![图 2.4 工程文件的包依赖](./img/keil_opt_manager_env.png)

包的分类是 Utility，在该分类下勾选即可完成添加。

![图 2.5 进行包的添加](./img/keil_run_env.png)

添加完成后，推荐阅读[第三章](#section_3)了解如何对库进行配置，第三章内容包括如何配置内存分配器等一系列内容。


### 2.1.2 从源代码添加

所有从源代码添加的步骤均包括添加源文件和添加包含目录两个部分。源文件是编译器需要编译的内容，包含目录则描述了编译器去何处查找头文件。

首先，在工程中添加源代码文件，单击添加已有文件的菜单，如[图2.6](#figure_2_6)所示。

![图 2.6 添加已有文件的菜单项](./img/keil_add_ext_src_nlfs.png)

# 3 客制化

本章描述了几个可选功能如何开启或者关闭。