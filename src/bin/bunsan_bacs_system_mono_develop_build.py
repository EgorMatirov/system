#!/usr/bin/python3

import argparse
from os.path import abspath, join, basename, dirname, isfile
import xml.etree.ElementTree as etree
import os
import subprocess
import shutil
import shlex


def getprojectinfo(csproj):
    tree = etree.parse(csproj)
    schema = tree.getroot().tag.strip('Project')
    for prop in tree.getroot().findall(schema + 'PropertyGroup'):
        outputtypeprop = prop.find(schema + 'OutputType')
        if outputtypeprop is not None:
            outtype = outputtypeprop.text
        name = prop.find(schema + 'AssemblyName')
        if name is not None:
            projName = name.text
    return (projName, outtype)


def GetExecutableProjectPath(solution, configuration):
    with open(solution, 'r') as f:
        for line in f:
            if line.startswith('Project'):
                words = [x.strip().strip('"') for x in line.split(',')]
                csproj = join(dirname(solution), words[1].replace('\\', '/'))
                proj = dirname(csproj)
                projname, projtype = getprojectinfo(csproj)
                if projtype == 'Exe':
                    return join(
                        proj,
                        'bin',
                        configuration,
                        projname + '.' + 'exe'
                    )
        return None


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Extracts executable name from *.sln'
    )
    parser.add_argument('solution',
                        help='Solution file (*.sln)')
    parser.add_argument('-c', '--configuration',
                        default='Release',
                        help='Configuration')
    parser.add_argument('-o', '--output',
                        required=True,
                        help='Output file')
    args = parser.parse_args()

    subprocess.check_call([
        'xbuild',
        '/property:Configuration={}'.format(args.configuration),
        args.solution
    ])
    exe = GetExecutableProjectPath(args.solution, args.configuration)
    with open(args.output, 'w') as out:
        print('#!/bin/sh -e', file=out)
        print('exec', shlex.quote(abspath(exe)), '"$@"', file=out)
    os.chmod(args.output, 0o777)
