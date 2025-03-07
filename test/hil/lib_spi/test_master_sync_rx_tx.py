#!/usr/bin/env python
# Copyright 2015-2021 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
from spi_master_checker import SPIMasterChecker
from pathlib import Path
import Pyxsim as px
import pytest

mode_args = {"mode_0": 0,
             "mode_1": 1,
             "mode_2": 2,
             "mode_3": 3}

div_args = {"divider_4x": 4,
            "divider_8x": 8,
            "divider_80x": 80}

mosi_enabled_args = {"mosi_disabled": 0,
                    "mosi_enabled": 1}

miso_enabled_args = {"miso_disabled": 0,
                    "miso_enabled": 1}

full_load_args = {"not_fully_loaded": 0,
                  "fully_loaded": 1}

# If neither miso or mosi are enabled, deselect the test
def uncollect_if(mode, div, mosi_enabled, miso_enabled, full_load):
    if not (mosi_enabled or miso_enabled):
        return True

@pytest.mark.uncollect_if(func=uncollect_if)
@pytest.mark.parametrize("mode", mode_args.values(), ids=mode_args.keys())
@pytest.mark.parametrize("div", div_args.values(), ids=div_args.keys())
@pytest.mark.parametrize("mosi_enabled", mosi_enabled_args.values(), ids=mosi_enabled_args.keys())
@pytest.mark.parametrize("miso_enabled", miso_enabled_args.values(), ids=miso_enabled_args.keys())
@pytest.mark.parametrize("full_load", full_load_args.values(), ids=full_load_args.keys())
def test_spi_master_sync_rx_tx(build, capfd, nightly, request, full_load, miso_enabled, mosi_enabled, div, mode):
    if not nightly and not full_load:
        pytest.skip("Only test non-full_load nightly")

    id_string = f"{full_load}_{miso_enabled}_{mosi_enabled}_{div}_{mode}"

    cwd = Path(request.fspath).parent

    binary = f"{cwd}/spi_master_sync_rx_tx/bin/{id_string}/spi_master_sync_rx_tx_{id_string}.xe"

    checker = SPIMasterChecker("tile[0]:XS1_PORT_1C",
                               "tile[0]:XS1_PORT_1D",
                               "tile[0]:XS1_PORT_1A",
                               ["tile[0]:XS1_PORT_1B"],
                               "tile[0]:XS1_PORT_1E",
                               "tile[0]:XS1_PORT_16B")

    tester = px.testers.PytestComparisonTester(f'{cwd}/expected/master_sync.expect',
                                            regexp = True,
                                            ordered = True)
                                            
    build(directory = binary, 
            env = {"FULL_LOAD":f'{full_load}', 
                   "MISO_ENABLED":f'{miso_enabled}',
                   "MOSI_ENABLED":f'{mosi_enabled}',
                   "SPI_MODE":f'{mode}',
                   "DIVS":f'{div}'},
            bin_child = id_string)

    px.run_with_pyxsim(binary,
                       simthreads = [checker])

    tester.run(capfd.readouterr().out)
