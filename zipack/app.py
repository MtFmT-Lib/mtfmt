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
from pdscfile import GeneratePDSCFile


class Application:
    """
    App类
    """

    def __init__(self, args: argparse.Namespace, output_dir='./target_package'):
        """
        构造app
        """
        # 加载工程
        print(f'> Load project: \033[1;32m{args.project}\033[0;0m')
        project = Project(output_dir, args.project)
        # 执行动作
        action: str = args.action.lower()
        if action == 'check':
            print('> \033[1;32mCheck completed\033[0;0m')
        elif action not in project.actions:
            print(f'> \033[1;31mNo action named {action}\033[0;0m')
            exit(1)
        else:
            self._invoke_action(action, project)

    def _invoke_action(self, action_name: str, project: Project):
        """
        执行动作
        """
        action_type = project.actions[action_name][0]
        print(f'> Running action \033[1;32m{action_name}\033[0;0m ...')
        print(f'  Action name: \033[1;33m{action_name}\033[0;0m')
        print(f'  Action type: \033[1;33m{action_type}\033[0;0m')
        if action_type == ActionType.PACK:
            runner = PackAction(project,
                                project.actions[action_name][1])
            runner.run()
        elif action_type == ActionType.CMSIS:
            runner = GeneratePDSCFile(project,
                                      project.actions[action_name][1])
            runner.run()
        else:
            print(f'> \033[1;31mAction {action_name} is not support.\033[0;0m')
            exit(1)
        # 完成
        print(f'> Completed action \033[1;32m{action_name}\033[0;0m.')
