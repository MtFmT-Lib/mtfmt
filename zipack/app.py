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

import argparse
from pack import PackAction
from pdscfile import GeneratePDSCFile
from typing import cast as type_case
from project import Project, ActionType, PackFileInfo, CMSISPdscFileInfo


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
            actions = project.get_actions(action)
            for action_step in actions:
                name_str = f'\033[1;32m{action}\033[0;0m'
                step_str = f'\033[1;34m{action_step}\033[0;0m'
                if action_step == action:
                    print(f'> Running action {name_str} ...', end='')
                else:
                    print(
                        f'> Running action {step_str} ...', end='')
                self._invoke_action(action_step, project)
                # 完成
                print(' \033[1;34mDone\033[0;0m')
            print(f'> Completed \033[1;32m{action}\033[0;0m')

    def _invoke_action(self, action_name: str, project: Project):
        """
        执行动作
        """
        config = project.actions[action_name][1]
        action_type = project.actions[action_name][0]
        if action_type == ActionType.NONE:
            pass
        elif action_type == ActionType.PACK:
            PackAction(project, type_case(PackFileInfo, config)).run()
        elif action_type == ActionType.CMSIS:
            pdsc_config = type_case(CMSISPdscFileInfo, config)
            GeneratePDSCFile(project, pdsc_config).run()
        else:
            print(f'> \033[1;31mAction {action_name} is not support.\033[0;0m')
            exit(1)
