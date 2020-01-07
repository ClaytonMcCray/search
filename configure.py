import os
import argparse


def run_cmake(root_directory, build_directory):
	back = os.getcwd()
	os.chdir(build_directory)
	os.system('cmake ' + root_directory)
	os.chdir(back)


def run_make(root_directory, build_directory):
	back = os.getcwd()
	os.chdir(build_directory)
	os.system('make')
	os.chdir(back)


parser = argparse.ArgumentParser(description='Configure search build.')
parser.add_argument('--debug', type=str, default="debug")
parser.add_argument('--release', type=str, default="release")
parser.add_argument('--root', type=str, default=os.getcwd())
parser.add_argument('--build', type=bool, default=False)

args = parser.parse_args()

os.makedirs(args.debug)
os.makedirs(args.release)


run_cmake(args.root, args.debug)
run_cmake(args.root, args.release)

if args.build:
	run_make(args.root, args.debug)
	run_make(args.root, args.release)
