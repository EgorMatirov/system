#include "mono_develop.hpp"

#include <bacs/system/file.hpp>
#include <bacs/system/process.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

namespace bacs{namespace system{namespace builders
{
    namespace
    {
        struct invalid_lang_error: virtual invalid_argument_error {};
    }

    const bool mono_develop::factory_reg_hook = builder::register_new(
        "mono_develop",
        [](const std::vector<std::string> &arguments)
        {
            builder_ptr tmp(new mono_develop(arguments));
            return tmp;
        });

    static const boost::regex positional("[^=]+"), key_value("([^=]+)=(.*)");

    mono_develop::mono_develop(const std::vector<std::string> &arguments):
        m_configuration("Release")
    {
        for (const std::string &arg: arguments)
        {
            boost::smatch match;
            if (boost::regex_match(arg, match, key_value))
            {
                BOOST_ASSERT(match.size() == 3);
                const std::string key = match[1].str(), value = match[2].str();
                if (key == "configuration")
                {
                    m_configuration = value;
                }
                else
                {
                    BOOST_THROW_EXCEPTION(
                        invalid_argument_error() <<
                        invalid_argument_error::argument(arg));
                }
            }
            else
            {
                BOOST_THROW_EXCEPTION(
                    invalid_argument_error() <<
                    invalid_argument_error::argument(arg));
            }
        }
    }

    static const std::string sln_builder = R"EOF(
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
)EOF";

    static boost::filesystem::path executable_path = "executable";

    executable_ptr mono_develop::build_extracted(
        const ContainerPointer &container,
        const unistd::access::Id &owner_id,
        bunsan::tempfile &&tmpdir,
        const boost::filesystem::path &source,
        const bacs::process::ResourceLimits &resource_limits,
        bacs::process::BuildResult &result)
    {
        using boost::filesystem::recursive_directory_iterator;

        const boost::filesystem::path root =
            container->filesystem().containerPath(tmpdir.path());

        bool found = false;
        boost::filesystem::path solution;
        const boost::filesystem::path abs_source =
            container->filesystem().keepInRoot(source);
        for (recursive_directory_iterator i(abs_source),
                                          end;
             i != end;
             ++i)
        {
            if (boost::filesystem::is_regular_file(*i) &&
                i->path().extension() == ".sln")
            {
                if (found)
                {
                    result.set_status(bacs::process::BuildResult::FAILED);
                    result.set_output(str(
                        boost::format("Multiple solutions found: %1% and %2%") %
                        solution %
                        i->path()
                    ));
                    return executable_ptr();
                }
                else
                {
                    found = true;
                    solution = i->path();
                }
            }
        }
        solution = container->filesystem().containerPath(solution);

        const boost::filesystem::path executable = root / executable_path;
        const ProcessGroupPointer process_group = container->createProcessGroup();
        const ProcessPointer process = process_group->createProcess("python3");
        process->setArguments(
            process->executable(),
            "-c", sln_builder,
            "--configuration", m_configuration,
            "--output", executable,
            solution
        );
        process::setup(process_group, process, resource_limits);
        process->setCurrentPath(root);
        process->setOwnerId(owner_id);
        process->setStream(2, FdAlias(1));
        process->setStream(1, File("log", AccessMode::WRITE_ONLY));
        const ProcessGroup::Result process_group_result =
            process_group->synchronizedCall();
        const Process::Result process_result = process->result();
        const bool success = process::parse_result(
            process_group_result, process_result, *result.mutable_execution());
        result.set_output(file::read_first(tmpdir.path() / "log", 4096));
        if (success)
        {
            result.set_status(bacs::process::BuildResult::OK);
            executable_ptr tmp(new project_executable(
                container,
                std::move(tmpdir),
                executable
            ));
            return tmp;
        }
        else
        {
            result.set_status(bacs::process::BuildResult::FAILED);
            return executable_ptr();
        }
    }
}}}
