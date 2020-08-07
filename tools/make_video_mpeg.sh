#!/bin/bash
ffmpeg -pattern_type glob -framerate 60 -i "./frames/*.ppm" -codec copy output.mpeg
