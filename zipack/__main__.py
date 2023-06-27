# -*- coding: utf-8 -*-
# SPDX-License-Identifier: LGPL-3.0

"""
Package tool
~~~~~~~~~~~~

 Package tool

 File: __main__.py
 Desc: __main__
 Date: 2023-06-16
 Auth: XiangYang<hinata.hoshino@foxmail.com>

"""

import argparse
import colorama
from app import Application

if __name__ == '__main__':
    colorama.init(autoreset=True)
    print('Package tool')
    parser = argparse.ArgumentParser(description='Package tool')
    # 选项
    # 工程文件名
    parser.add_argument(
        '--project',
        type=str,
        default='./project.toml',
        help='工程文件, 默认为根目录下的"project.toml"'
    )
    # 动作
    parser.add_argument(
        'action',
        type=str,
        help='动作')
    # app
    args = parser.parse_args()
    app = Application(args)
