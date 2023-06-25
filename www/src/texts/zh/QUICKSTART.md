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

库提供的 CMSIS-PACK 是由 Github action 自动生成的。在[该页面](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/cmsis-pack.yml)找到分支名为`master`的项并下载压缩包，其中即是打包好的 CMSIS-PACK 文件。
