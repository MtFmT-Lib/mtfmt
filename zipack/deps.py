# -*- coding: utf-8 -*-
# SPDX-License-Identifier: LGPL-3.0

"""
Package tool
~~~~~~~~~~~~

 Package tool

 File: deps.py
 Desc: 依赖关系图
 Date: 2023-06-19
 Auth: XiangYang<hinata.hoshino@foxmail.com>

"""

from graphlib import TopologicalSorter
from typing import Dict, List, Set


class DependenceGraphic:
    """
    依赖关系图
    """

    deps: Dict[str, List[str]]

    def __init__(self, deps: Dict[str, List[str]]):
        """
        从依赖map构造
        """
        self.deps = deps

    def dep_of(self, item: str) -> List[str]:
        """
        返回依赖的可行运行顺序
        """
        if item not in self.deps:
            return [item]
        else:
            deps = self.deps
            # 抓走所有的依赖项
            q = [item]
            sub_deps: Dict[str, Set[str]] = {}
            while len(q) > 0:
                name = q.pop()
                dep_items = deps[name]
                if len(dep_items) > 0:
                    sub_deps.update({name: dep_items})
                for dep_item in dep_items:
                    q.append(dep_item)
            # 返回拓扑序
            return list(TopologicalSorter(sub_deps).static_order())
