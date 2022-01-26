#!/usr/bin/env python3

import sys
from dune.generator.remove import removeGenerated
from argparse import ArgumentParser
parser = ArgumentParser(description='Removing generated module from dune-py')
parser.add_argument('-a', '--all', help='remove all modules', action='store_true', default=False)
parser.add_argument('modules', metavar='M', nargs='*',
                    help='base of the modules to remove')

try:
    args = parser.parse_args()
except:
    sys.exit(0)

modules = []
if args.all:
    modules = ['all']
elif len(args.modules) > 0:
    modules = args.modules
else:
    parser.print_help()
    sys.exit(0)

removeGenerated(modules)
