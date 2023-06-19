# -*- coding: utf-8 -*-
# SPDX-License-Identifier: LGPL-3.0

"""
Package tool
~~~~~~~~~~~~

 Package tool

 File: pack.py
 Desc: 打包文件
 Date: 2023-06-18
 Auth: XiangYang<hinata.hoshino@foxmail.com>

"""

import os
from action import IAction
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
from project import Project, PackFileInfo, PackageFileInfo


class PackAction(IAction):
    """
    打包zip的action
    """

    def __init__(self, output_dir: Path, prj: Project, cfg: PackFileInfo):
        self.project = prj
        self.configure = cfg
        self.output_file = os.path.join(output_dir, cfg.target)

    def run(self):
        with ZipFile(self.output_file, 'w') as zip:
            categories = self.configure.categories
            # 写入源文件, 头文件等等的东东
            for file in self.project.package.package_files:
                if file.category in categories:
                    self._run_helper(zip, file)

    def _run_helper(self, fs, file: PackageFileInfo):
        sep = os.path.sep
        base_dir = file.pack_dir
        for file in file.files:
            file_name = os.path.basename(file)
            file_arhv = f'{base_dir}{sep}{file_name}'
            print(f'  - pack: Processing file \033[1;32m{file_arhv}\033[0;0m.')
            fs.write(file, file_arhv, ZIP_DEFLATED, 9)
