#!/usr/bin/env python
# coding=utf-8
# Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
# MemCache_Hybrid is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.

"""Run with matching installed wheels and Torch NPU; no MetaService is needed."""

import subprocess
import sys
import textwrap
import unittest


class TestConfigStoreLifecycle(unittest.TestCase):
    def test_memcache_and_memfabric_exit_after_config_store_cleanup(self):
        child = textwrap.dedent("""
            import ctypes
            from pathlib import Path
            import socket

            import torch
            import torch_npu

            torch.npu.set_device(0)

            import memcache_hybrid
            import memfabric_hybrid as mf

            with socket.socket() as probe:
                probe.bind(('127.0.0.1', 0))
                port = probe.getsockname()[1]
            url = f'tcp://127.0.0.1:{port}'
            assert mf.initialize() == 0
            assert mf.create_config_store(url) == 0
            library = ctypes.CDLL(str(Path(mf.get_lib_path()) / 'libmf_smem.so'))
            library.smem_destroy_config_store.argtypes = [ctypes.c_char_p]
            library.smem_destroy_config_store.restype = None
            library.smem_destroy_config_store(url.encode())
            mf.uninitialize()
            print('CONFIG_STORE_CLEANUP_RETURNED', flush=True)
        """)
        result = subprocess.run([sys.executable, '-u', '-c', child], capture_output=True, text=True, timeout=60)
        self.assertIn('CONFIG_STORE_CLEANUP_RETURNED', result.stdout, result.stdout + result.stderr)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == '__main__':
    unittest.main()
