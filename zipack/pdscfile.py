# -*- coding: utf-8 -*-
# SPDX-License-Identifier: LGPL-3.0

"""
Package tool
~~~~~~~~~~~~

 Package tool

 File: pdscfile.py
 Desc: PDSC FILE的生成
 Date: 2023-06-19
 Auth: XiangYang<hinata.hoshino@foxmail.com>

"""

import os
from action import IAction
from pathlib import Path
from project import Project, AutogenFileInfo, PackageFileInfo


class GeneratePDSCFile(IAction):
    """
    生成pdsc文件
    """

    def __init__(self, prj: Project, cfg: AutogenFileInfo):
        self.project = prj
        self.configure = cfg

    def run(self):
        pass
