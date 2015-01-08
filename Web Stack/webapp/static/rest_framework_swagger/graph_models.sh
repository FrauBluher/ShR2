#!/bin/bash

python ~/seads/manage.py graph_models -a > ~/seads/my_project.dot

dot -Tpng ~/seads/my_project.dot -o ~/seads/outfile.png
