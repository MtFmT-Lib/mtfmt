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
from zipfile import ZipFile
from pathlib import Path
from action import IAction
from project import Project, PackageInfo


class PackAction(IAction):
    """
    打包zip的action
    """

    def __init__(self, output: str, prj: Project, cfg: PackageInfo):
        self.project = prj
        self.configure = cfg
        self.output_file = Path(output)

    def run(self):
        sep = os.path.sep
        with ZipFile(self.output_file, 'w') as zip:
            # 写入源文件, 头文件等等的东东
            for file in self.project.package.package_files:
                base_dir = file.pack_dir
                for file in file.files:
                    file_name = os.path.basename(file)
                    file_arhv = f'{base_dir}{sep}{file_name}'
                    zip.write(file, file_arhv)
