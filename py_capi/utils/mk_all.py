#! /usr/bin/env python3

""" ファイルを生成するスクリプト

:file: mk_all.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

import os
import sys
import importlib
from argparse import ArgumentParser
from pathlib import Path

# druid のベースディレクトリ
base_dir = os.path.dirname(__file__)
druid_dir = os.path.abspath(os.path.join(base_dir, '..', '..'))

# mk_py_capi の親のディレクトリ
root_dir = os.path.abspath(os.path.join(druid_dir,
                                        'ymworks', 'py_capi', 'utils'))
#from mk_py_capi import root_dir
sys.path.append(root_dir)
sys.path.append(base_dir)
from mk_py_capi import ModuleGen, YmAddObjectLibraryTarget, AddExecutableTarget, AddLibraryTarget

# 引数の解釈
parser = ArgumentParser(prog='mk_all',
                        description="make 'druid' module")

parser.add_argument('-i', '--include_dir', type=str)
parser.add_argument('-s', '--source_dir', type=str)

args = parser.parse_args()

if args.include_dir is None:
    include_dir = os.path.abspath(os.path.join(druid_dir,
                                               'py_capi', 'include', 'pym'))
else:
    include_dir = args.include_dir

if args.source_dir is None:
    source_dir = os.path.abspath(os.path.join(druid_dir, 'py_capi'))
else:
    source_dir = args.source_dir

subname_list = [
    'tpg_types',
    'fsim',
    'dtpg',
    'condgen',
]
submodule_list = []
src_list = ['druid_module.cc']
for subname in subname_list:
    src_list.append(f'${{{subname}_SOURCES}}')
    submodule = importlib.import_module(subname)
    module_gen = ModuleGen(modulename=subname,
                           submodule=True,
                           pyclass_gen_list=submodule.gen_list,
                           namespace='DRUID',
                           header_include_files=['druid.h'],
                           source_include_files=['druid.h'])
    module_gen.make_all(include_dir=include_dir,
                        source_dir=source_dir)
    init_func = f'PyInit_{subname}'
    submodule_list.append((subname, init_func))

obj_list = ["py_druid_obj",
            "py_ymworks_obj",
            "druid_types_obj",
            "druid_fsim_obj",
            "druid_fsim_multi_combi2_obj",
            "druid_fsim_multi_combi3_obj",
            "druid_fsim_multi_bside2_obj",
            "druid_fsim_multi_bside3_obj",
            "druid_fsim_naive_combi2_obj",
            "druid_fsim_naive_combi3_obj",
            "druid_fsim_naive_bside2_obj",
            "druid_fsim_naive_bside3_obj",
            "druid_dtpg_obj",
            "druid_condgen_obj",]

src_list1 = ["druid.cc"]
for obj in obj_list:
    src_list1.append(f'$<TARGET_OBJECTS:{obj}>')
src_list1.append('${YM_SUBMODULE_OBJ_LIST}')

src_list2 = ["druid.cc"]
for obj in obj_list:
    src_list2.append(f'$<TARGET_OBJECTS:{obj}_d>')
src_list2.append('${YM_SUBMODULE_OBJ_D_LIST}')

src_list3 = ["druid.cc"]
for obj in obj_list:
    src_list3.append(f'$<TARGET_OBJECTS:{obj}_p>')
src_list3.append('${YM_SUBMODULE_OBJ_P_LIST}')

module_gen = ModuleGen(modulename="druid",
                       header_name='druid_module.h',
                       submodule_list=submodule_list,
                       namespace='DRUID',
                       header_include_files=['druid.h'],
                       source_include_files=['druid.h'],
                       include_dir_list=['${PROJECT_SOURCE_DIR}/ymworks/py_capi/include'],
                       target_list=[YmAddObjectLibraryTarget(objname='py_druid',
                                                             src_list=src_list),
                                    AddExecutableTarget(exename='druid',
                                                        src_list=src_list1,
                                                        option_list=['"-O3"'],
                                                        definition_list=['"-DNODEBUG"'],
                                                        link_list=['${YM_LIB_DEPENDS}',
                                                                   '${Python3_LIBRARIES}']),
                                    AddExecutableTarget(exename='druid_d',
                                                        src_list=src_list2,
                                                        option_list=['"-g"'],
                                                        link_list=['${YM_LIB_DEPENDS}',
                                                                   '${Python3_LIBRARIES}']),
                                    AddLibraryTarget(libname='druidlib',
                                                     shared=True,
                                                     src_list=src_list3,
                                                     link_list=['${YM_LIB_DEPENDS}',
                                                                '${Python3_LIBRARIES}'],
                                                     property_list=[('PREFIX', '""'),
                                                                    ('OUTPUT_NAME', '"druid"')])])
module_gen.make_all(include_dir=include_dir,
                    source_dir=source_dir)
