# Copyright 2014-2021 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
import Pyxsim as px
from pathlib import Path
from i2c_master_checker import I2CMasterChecker

def test_i2c_reg_ops_nack(build, capfd, request):
    cwd = Path(request.fspath).parent
    binary = f'{cwd}/i2c_master_reg_test/bin/i2c_master_reg_test.xe'

    checker = I2CMasterChecker("tile[0]:XS1_PORT_1A",
                               "tile[0]:XS1_PORT_1B",
                               tx_data = [0x99, 0x3A, 0xff, 0x05, 0xee, 0x06],
                               expected_speed = 400,
                               ack_sequence=[False, # NACK header
                                             True, True, False, # NACK before data
                                             True, False, # NACK before data
                                             True, False, # NACK before data
                                             False, # NACK address
                                             True, False, # NACK before data
                                             True, False, # NACK before data
                                             True, True, False # NACK before data
                                            ])

    tester = px.testers.PytestComparisonTester(f'{cwd}/expected/reg_ops_nack.expect',
                                                regexp = True,
                                                ordered = True)

    sim_args = ['--weak-external-drive']

    build(binary)

    px.run_with_pyxsim(binary,
                    simthreads = [checker],
                    simargs = sim_args)
                    
    tester.run(capfd.readouterr().out)

