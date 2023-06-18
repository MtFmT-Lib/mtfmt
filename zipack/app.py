# -*- coding: utf-8 -*-
# SPDX-License-Identifier: LGPL-3.0

"""
Package tool
~~~~~~~~~~~~

 Package tool

 File: app.py
 Desc: 应用程序
 Date: 2023-06-18
 Auth: XiangYang<hinata.hoshino@foxmail.com>

"""

import os
import argparse
from project import Project, ActionType
from pack import PackAction


class Application:
    """
    App类
    """

    def __init__(self, args: argparse.Namespace, output_dir='./pack'):
        """
        构造app
        """
        # 加载工程
        print(f'> Load project: \033[1;32m{args.project}\033[0;0m')
        project = Project(args.project)
        # 创建输出目录
        self.output_dir = os.path.abspath(output_dir)
        if not os.path.exists(self.output_dir):
            os.makedirs(self.output_dir)
            print('> \033[1;33mCreate a new output directory\033[0;0m')
        # 执行动作
        action: str = args.action.lower()
        if action == 'check':
            print('> \033[1;32mCheck completed\033[0;0m')
        elif action not in project.actions:
            print(f'> \033[1;31mNo action named {action}\033[0;0m')
            exit(1)
        else:
            print(f'> Running action \033[1;32m{action}\033[0;0m ...')
            self._invoke_action(action, project)

    def _invoke_action(self, action_name: str, project: Project):
        """
        执行动作
        """
        action_type = project.actions[action_name][0]
        print(f'  Action name: \033[1;33m{action_name}\033[0;0m')
        print(f'  Action type: \033[1;33m{action_type}\033[0;0m')
        if action_type == ActionType.PACK:
            runner = PackAction(f'./{self.output_dir}/{project.package.name}.zip',
                                project,
                                project.actions[action_name][1])
            runner.run()
        else:
            print(f'> \033[1;31mAction {action_name} is not support.\033[0;0m')
            exit(1)
