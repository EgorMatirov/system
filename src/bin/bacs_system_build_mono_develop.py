#!/usr/bin/python3

import argparse
from os.path import abspath, exists, join, basename, dirname, isfile
import xml.etree.ElementTree as etree
import os
import subprocess
import shutil
import shlex


class Project(object):

    def __init__(self, csproj):
        self._csproj = csproj
        tree = etree.parse(csproj)
        schema = tree.getroot().tag.strip('Project')
        for prop in tree.getroot().findall(schema + 'PropertyGroup'):
            output_type_prop = prop.find(schema + 'OutputType')
            if output_type_prop is not None:
                self._type = output_type_prop.text
            name = prop.find(schema + 'AssemblyName')
            if name is not None:
                self._name = name.text

    @property
    def type(self):
        return self._type

    @property
    def name(self):
        return self._name

    @property
    def root(self):
        return dirname(self._csproj)

    @property
    def csproj(self):
        return self._csproj


class Solution(object):

    def __init__(self, solution):
        self._solution = solution
        self._root = dirname(solution)
        with open(solution, 'r') as f:
            for line in f:
                if line.startswith('Project'):
                    words = [x.strip().strip('"') for x in line.split(',')]
                    csproj = join(self.root, words[1].replace('\\', '/'))
                    project = Project(csproj)
                    if project.type == 'Exe':
                        self._exe_project = project

    @property
    def solution(self):
        return solution

    @property
    def root(self):
        return dirname(self._solution)

    def executable(self, configuration):
        return join(
            self._exe_project.root,
            'bin',
            configuration,
            self._exe_project.name + '.exe'
        )


    @property
    def exe_project(self):
        return self._exe_project


def create_executable(output, executable, relative=False):
    executable = shlex.quote(executable)
    if relative:
        executable = '"$(dirname "$0")/"' + executable
    with open(output, 'w') as out:
        print('#!/bin/sh -e', file=out)
        print('exec', executable, '"$@"', file=out)
        os.fchmod(out.fileno(), os.fstat(out.fileno()).st_mode | 0o111)

def set_execute_bit(binary):
    with open(binary, 'a') as exe:
        os.fchmod(exe.fileno(), os.fstat(exe.fileno()).st_mode | 0o111)


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
    parser.add_argument('--fhs',
                        action='store_true',
                        help='Create all-sufficient hierarchy')
    parser.add_argument('--executable',
                        help='Executable name, project name by default')
    parser.add_argument('--application',
                        help='Name for application\'s subfolder, ' +
                             'project name by default')
    args = parser.parse_args()

    subprocess.check_call([
        'xbuild',
        '/property:Configuration={}'.format(args.configuration),
        args.solution
    ])
    solution = Solution(args.solution)
    set_execute_bit(solution.executable(args.configuration))
    if args.fhs:
        exe = args.executable or solution.exe_project.name
        application = args.application or solution.exe_project.name
        actual_executable = join(
            '..',
            'lib',
            application,
            basename(solution.executable(args.configuration))
        )
        srcbin = dirname(solution.executable(args.configuration))
        bindir = join(args.output, 'bin')
        appdir = join(args.output, 'lib', application)
        executable = join(bindir, exe)
        os.makedirs(bindir, exist_ok=True)
        if exists(appdir):
            shutil.rmtree(appdir)
        shutil.copytree(srcbin, appdir)
        create_executable(executable, actual_executable, relative=True)
    else:
        create_executable(
            args.output,
            abspath(solution.executable(args.configuration))
        )
