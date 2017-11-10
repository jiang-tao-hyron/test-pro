#!/bin/sh
#
# change config file script for G2CAM
# Copyright(C) 2002-2017 MegaChips Co.,Ltd All right reserved.
# author: "Jiangtao"<jiangtao@hyron-js.com>
#

rm -rf scene_backlight scene_indoor scene_normal scene_outdoor scene_outdoor_asp scene_room tmpfile ConfFileChg_diff.csv
mkdir scene_backlight scene_indoor scene_normal scene_outdoor scene_outdoor_asp scene_room tmpfile
mkdir tmpfile/scene_backlight tmpfile/scene_indoor tmpfile/scene_normal tmpfile/scene_outdoor tmpfile/scene_outdoor_asp tmpfile/scene_room
./ConfFileChg
rm -rf tmpfile
