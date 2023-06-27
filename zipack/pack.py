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
from zipfile import ZipFile, ZIP_DEFLATED
from project import Project, PackFileInfo, PackageFileInfo


class PackAction(IAction):
    """
    打包zip的action
    """

    def __init__(self, prj: Project, cfg: PackFileInfo):
        self.project = prj
        self.configure = cfg

    def run(self):
        output_file = self.project.get_output_fullpath(self.configure.target)
        with ZipFile(output_file, 'w') as zip:
            categories = self.configure.categories
            # 写入源文件, 头文件等等的东东
            for file in self.project.package.package_files:
                if file.category in categories:
                    self._run_helper(zip, file)

    def _run_helper(self, fs, file_info: PackageFileInfo):
        sep = os.path.sep
        base_dir = file_info.pack_dir
        for file in file_info.files:
            file_name = os.path.basename(file)
            file_arhv = f'{base_dir}{sep}{file_name}'
            fs.write(file, file_arhv, ZIP_DEFLATED, 9)
