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
from typing import Dict, Any, List, Union, Tuple, Optional
from pathlib import Path
from dataclasses import dataclass
from enum import Enum
from deps import DependenceGraphic


class ActionType(Enum):
    """
    表示文件所属的范畴
    """
    # 啥也不干
    NONE = 0
    # 打包ZIP
    PACK = 1
    # 生成cmsis pack file
    CMSIS = 2


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
    # 资源
    RESOURCES = 4
    # 许可证
    LICENSE = 5


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
    category: Union[FileCategory, str]


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
class PackageSupportInfo:
    """
    支持信息
    """

    # 主页
    homepage_url: str

    # issue
    issue_url: str

    # 组织页面
    organization_url: str

    @staticmethod
    def load(toml_dat: Any):
        """
        解析tomldata
        """
        issue_url = toml_dat['issue'] if 'issue' in toml_dat else ''
        homepage_url = toml_dat['homepage'] if 'homepage' in toml_dat else ''
        organization_url = toml_dat['organization'] if 'organization' in toml_dat else ''
        return PackageSupportInfo(homepage_url, issue_url, organization_url)


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

    # 关键字
    keywords: List[str]

    # 组织名
    organization_name: str

    # 描述
    description: str

    # 支持信息
    support: Optional[PackageSupportInfo]

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
        # 关键字
        if 'keywords' in toml_dat:
            keyword_list = toml_dat['keywords']
            keywords = [str(s) for s in keyword_list]
        else:
            keywords = []
        # 组织名
        organization_name = toml_dat['organization'] if 'organization' in toml_dat else ''
        # 描述
        description = toml_dat['description'] if 'description' in toml_dat else 'No description'
        # 支持信息
        if 'support' in toml_dat:
            support_info = PackageSupportInfo.load(toml_dat['support'])
        else:
            support_info = None
        return PackageInfo(
            name,
            version,
            authors,
            keywords,
            organization_name,
            description,
            support_info,
            files
        )

    def add_target_files(self, name: str, target_dir: Path, outputs: List[Path]):
        """
        把target file添加到file list
        """
        category = 'target:' + name
        pkg_info = PackageFileInfo(outputs, target_dir, category)
        self.package_files.append(pkg_info)

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
    categories: Union[FileCategory, str]

    @staticmethod
    def load(toml_dat: Any):
        """
        加载内容
        """
        name = toml_dat['name']
        target = Path(toml_dat['target'])
        dependences = []
        if 'categories' in toml_dat:
            categories = []
            for s in toml_dat['categories']:
                if s[0] == '@':
                    categories.append('target:' + s[1:])
                    dependences.append(s[1:])
                else:
                    categories.append(FileCategory[s.upper()])
        else:
            categories = [FileCategory.DEFAULT]
        return (dependences, PackFileInfo(name, target, categories))


@dataclass
class CMSISPdscFileInfo:
    """
    自动生成的CMSIS pdsc文件信息
    """
    # 名称
    name: str

    # 目标
    target: Path

    # c-class
    c_class: str

    # c-group
    c_group: str

    @staticmethod
    def load(toml_dat: Any):
        """
        加载内容
        """
        name = toml_dat['name']
        target = Path(toml_dat['target'])
        c_class = toml_dat['cmsis-Cclass']
        c_group = toml_dat['cmsis-Cgroup']
        return ([], CMSISPdscFileInfo(name, target, c_class, c_group))


@ dataclass
class AutogenFileInfo:
    """
    自动生成文件的信息
    """
    # 名称
    name: str

    # 目标
    target: Path

    @ staticmethod
    def load(toml_dat: Any):
        """
        加载内容
        """
        name = toml_dat['name']
        target = Path(toml_dat['target'])
        return ([], AutogenFileInfo(name, target))


@dataclass
class EmptyActionInfo:
    """
    空的action
    """
    # 名称
    name: str


class State(Enum):
    """
    状态机状态
    """
    APPEND = 0
    BRACKET = 1
    REPLACEMENT = 2
    REPLACEMENT_ENDQ = 3
    REPLACEMENT_END = 4


class Project:
    """
    表示一个项目文件
    """

    # 包
    package: PackageInfo

    # 动作
    actions: Dict[str, Tuple[ActionType,
                             Union[AutogenFileInfo, CMSISPdscFileInfo, PackFileInfo]]]

    # 输出目录
    output_dir: Path

    # toml文件的raw reference
    toml_raw: Any

    # 依赖关系图
    deps: DependenceGraphic

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
            self.toml_raw = toml_dat
            self.actions = {}
            self.package = PackageInfo.load(toml_dat['package'])
            # 加载actions
            action_dep: Dict[str, List[str]] = {}
            action_dat = toml_dat['action']
            for action in action_dat:
                mode: str = action['mode']
                name: str = action['name']

                if name in self.actions or name == 'all':
                    raise AttributeError(f'Name "{name}" is not unique.')

                mode_val = ActionType[mode.upper()]
                if mode_val == ActionType.PACK:
                    dependences, patt_action = PackFileInfo.load(action)
                elif mode_val == ActionType.CMSIS:
                    dependences, patt_action = CMSISPdscFileInfo.load(action)
                else:
                    raise AttributeError(f'Unsupport action mode "{mode}"')

                action_dep.update({name: dependences})
                self.actions.update({name: (mode_val, patt_action)})
            # 检查依赖项
            self._check_dependences(action_dep)
            # 把所有的action作为一个files添加到PackageInfo
            for (action_name, (action_typ, action)) in self.actions.items():
                target = self.get_output_fullpath(action.target)
                self.package.add_target_files(
                    action_name, Path('.'), [target])
            # 添加 "all" target
            action_dep.update({'all': list(self.actions.keys())})
            self.actions.update(
                {'all': (ActionType.NONE, EmptyActionInfo('all'))})

            # 构建依赖关系图
            self.deps = DependenceGraphic(action_dep)
            # 打印项目信息
            self._print_project_info()
        except KeyError:
            raise AttributeError('Project file was missing some items.')
        except FileNotFoundError as e:
            raise AttributeError(f'Missing file {e.filename}.')

    def get_output_fullpath(self, file: Path) -> Path:
        """
        取得基于输出目录下的文件file的完整路径
        """
        return os.path.join(self.output_dir, self._eval_string(str(file)))

    def get_actions(self, name: str) -> List[str]:
        """
        按照依赖关系返回需要运行的action

        它返回的action顺序从0 ~ n满足依赖关系
        """
        return self.deps.dep_of(name)

    def _check_dependences(self, action_dep: Dict[str, List[str]]):
        """
        检查依赖项
        """
        for (action_name, action_deps) in action_dep.items():
            for dep_name in action_deps:
                if dep_name not in self.actions:
                    raise AttributeError(
                        f'Dependence "{dep_name}" of action "{action_name}" is not existed.')

    def _eval_string(self, str: str) -> str:
        """
        把{XXX:XXX}这类特殊的replacement替换为合适的值
        """
        char_token = [ch for ch in str]
        return self._eval_string_helper(char_token)

    def _eval_string_helper(self, chars: List[str]) -> str:
        """
        把{XXX:XXX}这类特殊的replacement替换为合适的值(helper)
        """
        res = ''
        name = ''
        state = State.APPEND
        # 替换值
        i = 0
        while i < len(chars):
            char = chars[i]
            if state == State.APPEND:
                i += 1
                if char == '{':
                    state = State.BRACKET
                else:
                    res += char
                    state = State.APPEND
            elif state == State.BRACKET:
                if char == '{':
                    # {{, 转义字符
                    i += 1
                    res += '{'
                    state = State.APPEND
                else:
                    # 名字
                    name = ''
                    state = State.REPLACEMENT
            elif state == State.REPLACEMENT:
                if char == '}':
                    i += 1
                    state = State.REPLACEMENT_ENDQ
                else:
                    i += 1
                    name += char
                    state = State.REPLACEMENT
            elif state == State.REPLACEMENT_ENDQ:
                if char == '}':
                    i += 1
                    name += '}'
                    state = State.REPLACEMENT
                else:
                    state = State.REPLACEMENT_END
            elif state == State.REPLACEMENT_END:
                # res += char
                # 按照":"分割并在toml_raw里面找对应的值
                names = name.split(':')
                if names[0] not in self.toml_raw:
                    raise AttributeError(f'No value named "{names[0]}".')
                value = self.toml_raw[names[0]]
                for name in names[1:]:
                    if name not in value:
                        raise AttributeError(f'No value named "{name}".')
                    value = value[name]
                res += str(value)
                state = State.APPEND
        return res

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
