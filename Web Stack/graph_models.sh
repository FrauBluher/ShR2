#!/bin/bash

python manage.py graph_models -a > my_project.dot

dot -Tpng my_project.dot -o outfile.png

rm my_project.dot
