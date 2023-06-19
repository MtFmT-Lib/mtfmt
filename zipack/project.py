# -*- coding: utf-8 -*-
# SPDX-License-Identifier: LGPL-3.0

"""
Package tool
~~~~~~~~~~~~

 Package tool

 File: project.py
 Desc: 项目文件
 Date: 2023-06-16
 Auth: XiangYang<hinata.hoshino@foxmail.com>

"""

import os
import re
import glob
import rtoml
from typing import Dict, Any, List, Union, Tuple
from pathlib import Path
from dataclasses import dataclass
from enum import Enum


class ActionType(Enum):
    """
    表示文件所属的范畴
    """
    # 打包ZIP
    PACK = 0
    # 生成cmsis pack file
    CMSIS = 1


class FileCategory(Enum):
    """
    表示文件所属的范畴
    """
    # 默认
    DEFAULT = 0
    # 源文件
    SOURCES = 1
    # 头文件
    HEADERS = 2
    # 编译出来的二进制文件
    BINARYS = 3
    # 许可证
    LICENSE = 4


@dataclass
class PackageFileInfo:
    """
    构建输出的信息
    """
    # 文件们
    files: List[Path]

    # 指定它们放到哪里
    pack_dir: Path

    # 指定范畴
    category: FileCategory


@dataclass
class PackageVersion:
    """
    版本信息
    """
    major: int
    minor: int
    release: int

    def __str__(self) -> str:
        return f'{self.major}.{self.minor}.{self.release}'

    @staticmethod
    def parse(version: str):
        """
        解析版本号(XX.XX.XX)
        """
        matched = re.match('([0-9]+)\\.([0-9]+)\\.([0-9]+)', version)
        if matched is not None:
            groups = matched.groups()
            major = int(groups[0])
            minor = int(groups[1])
            release = int(groups[2])
            return PackageVersion(major, minor, release)
        else:
            raise AttributeError('Version is not "[0-9]+.[0-9]+.[0-9]+"')


@dataclass
class PackageInfo:
    """
    包信息
    """

    # 包名
    name: str

    # 版本
    version: PackageVersion

    # 作者
    authors: List[str]

    # 参与打包的文件
    package_files: List[PackageFileInfo]

    @staticmethod
    def load(toml_dat: Any):
        """
        从toml数据加载
        """
        name = toml_dat['name']
        files = PackageInfo._enum_file_items(toml_dat['file'])
        version = PackageVersion.parse(toml_dat['version'])
        # 作者信息
        if 'authors' in toml_dat:
            author_list = toml_dat['authors']
            authors = [str(s) for s in author_list]
        else:
            authors = []
        return PackageInfo(name, version, authors, files)

    @staticmethod
    def _enum_file_items(file_items):
        """
        枚举package.file配置项所指定的文件们
        """
        files = []
        for file_item in file_items:
            total_files = []
            for pattern in file_item['file']:
                total_files += PackageInfo._enum_files(pattern)
            output_dir = Path(file_item['output'])
            if 'category' in file_item:
                cat_name: str = file_item['category']
                category = FileCategory[cat_name.upper()]
            else:
                category = FileCategory.DEFAULT
            files.append(PackageFileInfo(total_files, output_dir, category))
        return files

    @staticmethod
    def _enum_files(pattern: str) -> List[Path]:
        """
        枚举所有文件
        """
        files = glob.iglob(pattern)
        return [os.path.abspath(f) for f in files]


@dataclass
class PackFileInfo:
    """
    自动生成的打包信息
    """
    # 名称
    name: str

    # 目标
    target: Path

    # 参与打包的文件分类
    categories: FileCategory

    @staticmethod
    def load(toml_dat: Any):
        """
        加载内容
        """
        name = toml_dat['name']
        target = Path(toml_dat['target'])
        if 'categories' in toml_dat:
            categories = [FileCategory[s.upper()]
                          for s in toml_dat['categories']]
        else:
            categories = [FileCategory.DEFAULT]
        return PackFileInfo(name, target, categories)


@dataclass
class AutogenFileInfo:
    """
    自动生成文件的信息
    """
    # 名称
    name: str

    # 目标
    target: Path

    @staticmethod
    def load(toml_dat: Any):
        """
        加载内容
        """
        name = toml_dat['name']
        target = Path(toml_dat['target'])
        return AutogenFileInfo(name, target)


class Project:
    """
    表示一个项目文件
    """

    # 包
    package: PackageInfo

    # 动作
    actions: Dict[str, Tuple[ActionType,
                             Union[AutogenFileInfo, PackFileInfo]]]

    # 输出目录
    output_dir: Path

    def __init__(self, output_dir: str, file: str):
        """
        加载项目文件
        """
        try:
            # 输出目录
            self.output_dir = os.path.abspath(output_dir)
            if not os.path.exists(self.output_dir):
                os.makedirs(self.output_dir)
            # 解析toml
            toml_dat = rtoml.load(Path(file))
            self.actions = {}
            self.package = PackageInfo.load(toml_dat['package'])
            action_dat = toml_dat['action']
            for action in action_dat:
                mode: str = action['mode']
                name: str = action['name']

                if name in self.actions:
                    raise AttributeError(f'Name "{name}" is not unique.')

                mode_val = ActionType[mode.upper()]
                if mode_val == ActionType.PACK:
                    patt_action = PackFileInfo.load(action)
                elif mode_val == ActionType.CMSIS:
                    patt_action = AutogenFileInfo.load(action)
                else:
                    raise AttributeError(f'Unsupport action mode "{mode}"')

                self.actions.update({name: (mode_val, patt_action)})
            # 打印项目信息
            self._print_project_info()
        except KeyError:
            raise AttributeError('Project file was missing some items.')
        except FileNotFoundError as e:
            raise AttributeError(f'Missing file {e.filename}.')

    def get_output_fullpath(self, file: str) -> Path:
        """
        取得基于输出目录下的文件file的完整路径
        """
        return os.path.join(self.output_dir, file)

    def _print_project_info(self):
        """
        打印项目信息
        """
        pkg = self.package
        print(f'> Package \033[1;32m{pkg.name}\033[0;0m')
        print(f'  Version \033[1;33m{pkg.version}\033[0;0m')
        print(f'  Available action: \033[1;33m{len(self.actions)}\033[0;0')
        for (idx, (name, dummy)) in enumerate(self.actions.items()):
            print(f'  - {(idx + 1):2d}: \033[1;32m{name}\033[0;0')
