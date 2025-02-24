#MIT Expat license
#
#(C) 2025 Серый MLGamer <Seriy-MLGamer@yandex.ru>
#
#Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

'''
Advanced build from source code as easy as in package manager.
MakePy is a free framework for Python language which you can use to do a variety of operations on your source code and other files on different operating systems and processor architectures. An excellent alternative to Make, CMake and similar programs.
'''

from platform import system, machine
from os.path import *
from os import mkdir, symlink, remove, rmdir
from shutil import rmtree, copytree, copy, make_archive
from sys import argv, stderr
from subprocess import run

class MakePy:
	'''
	The main class of the framework. You must inherit it in your build script and override main() method. Then create an instance of inherited class and run its make() method.
	If you want to change default values, some of them must be changed in overridden __init__() method to work correctly.

	Args:
	default_target (str): the default target. If specified, targets can be omitted in command line.
	default_action (str): the default action. If specified, it can be omitted in command line.
	default_namespace (str): the default namespace (either "path" or "package"). If it's not specified, its value is "path".
	'''
	def __init__(self, default_target=None, default_action=None, default_namespace="path"):
		self.SRC="src"
		self.OBJ="obj"
		self.BIN="bin"
		self.LIB="lib"
		self.OUT="out"
		self.INSTALL_PREFIX=""
		self.PACK_PREFIX=".packpy"
		self.CC="gcc"
		self.CXX="g++"
		self.AR="ar"
		self.DLLTOOL="dlltool"
		self.GENDEF="gendef"
		self.C_COMPILE_FLAGS=[]
		self.CPP_COMPILE_FLAGS=[]
		self.CFLAGS=[]
		self.C_LINK_FLAGS=[]
		self.CPP_LINK_FLAGS=[]
		self.LDFLAGS=[]
		self.HOST_OPERATING_SYSTEM=system()
		self.HOST_ARCHITECTURE=machine()
		self.TARGET_OPERATING_SYSTEM=self.HOST_OPERATING_SYSTEM
		self.TARGET_ARCHITECTURE=self.HOST_ARCHITECTURE
		self.ACTION_BUILD="build"
		self.ACTION_INSTALL="install"
		self.ACTION_CLEAN="clean"
		self.ACTION_REMOVE="remove"
		self.ACTION_PACK="pack"
		self.ONCE=True
		self.AUTO_CLEAN=True
		self.AUTO_FOLDERS=True
		self.VERBOSE=False
		self.default_target=default_target
		self.default_action=default_action
		self.default_namespace=default_namespace
		self.targets={}
		self.clean_targets=set()
		self.clean_trees=set()
		self.remove_targets=[]
		self.pack=False
		self.pack_prefix=None
	def make(self, *arguments):
		'''
		Run the build process. It reads command line arguments, executes user build script defined in overridden main() method and performs specified action on targets.

		Args:
		*arguments (str): command line arguments. By default, they are read from command line.
		'''
		if len(arguments)==0: arguments=argv[1:]
		action=None
		targets={
			"path": [],
			"package": []
		}
		help=False
		defines={}
		state="none"
		for argument in arguments:
			if state=="none":
				if argument[0]=="-":
					if argument[1]=="-":
						if argument[2:]=="help": help=True
						elif argument[2:]=="license":
							self.license()
							exit()
						elif argument[2:]=="define": state="define"
						elif argument[2:]=="verbose": self.VERBOSE=True
						elif argument[2:]=="pack": self.pack_prefix="usr"
						elif len(argument)>7 and argument[2:7]=="pack=": self.pack_prefix=normpath(argument[7:])
						else: error_argument(argument)
					elif argument[1]=="h": help=True
					elif argument[1]=="l":
						self.license()
						exit()
					elif argument[1]=="D":
						if len(argument)==2: state="define"
						else:
							target=argument[2:].split("=", 1)
							if len(target)==2: defines[target[0]]=target[1]
							else: defines[target[0]]=True
					elif argument[1]=="v": self.VERBOSE=True
					else: error_argument(argument)
				elif action==None: action=argument
				else:
					target=argument.split(":", 1)
					if len(target)==2:
						if target[0] not in targets: error_namespace(target[0])
						targets[target[0]].append(normpath(target[1]) if target[0]=="path" else target[1])
					else: targets[self.default_namespace].append(normpath(argument) if self.default_namespace=="path" else argument)
			elif state=="define":
				target=argument.split("=", 1)
				if len(target)==2: defines[target[0]]=target[1]
				else: defines[target[0]]=True
				state="none"
		if action==None:
			if self.default_action==None: error_action()
			action=self.default_action
		if len(targets["path"])==0 and len(targets["package"])==0:
			if self.default_target==None: error_target()
			targets[self.default_namespace].append(normpath(self.default_target))
		self.pack=action==self.ACTION_PACK or self.pack_prefix!=None
		self.main(defines)
		if help: self.help()
		if self.ACTION_INSTALL in self.targets and self.ACTION_REMOVE in self.targets:
			if "package" in self.targets[self.ACTION_INSTALL] and "package" in self.targets[self.ACTION_REMOVE]:
				packages=self.targets[self.ACTION_INSTALL]["package"]
				remove_packages=self.targets[self.ACTION_REMOVE]["package"]
				for target in self.remove_targets:
					dependencies=remove_packages[target].dependencies[self.ACTION_REMOVE].packages
					for dependency in self.remove_targets:
						if dependency!=target:
							if dependency not in dependencies:
								if dependency in packages:
									if self.ACTION_INSTALL in packages[dependency].dependencies:
										if target in packages[dependency].dependencies[self.ACTION_INSTALL].packages: dependencies.append(dependency)
		for namespace in targets:
			for target in targets[namespace]:
				if action not in self.targets: error_no_action(action)
				if namespace not in self.targets[action]: error_no_target(action, namespace, target)
				if normcase(target) not in self.targets[action][namespace]: error_no_target(action, namespace, target)
				self.targets[action][namespace][normcase(target)].make(True)
	def help(self):
		'''
		Print a help message about command line usage of the build script.
		If you want to see API documentation of MakePy framework, type in Python console:
			>>> import makepy
			>>> help(makepy)
		'''
		print(basename(argv[0]), "- the build script based on MakePy framework (module \"makepy\").")
		print("Advanced build from source code as easy as in package manager.")
		print("Usage:", basename(argv[0]), "[options]", "<action>" if self.default_action==None or self.default_target==None else "[action]", "<targets>" if self.default_target==None else "[targets]")
		print("Arguments.")
		print("  action   The action being performed on the targets (see the list of available actions below).")
		if self.default_action!=None and self.default_target!=None: print("           Default action:", self.default_action)
		print("  targets  The arguments of the following format: \"<path or package>|path:<path>|package:<package>\". Symbol \":\" divides namespace and target name. If namespace is omitted, default namespace is used. Default namespace is \"", self.default_namespace, "\".", sep="")
		print("           Targets in namespace \"path\" correspond to files, folders, symbolic links, etc to exist or to be created in the filesystem.")
		print("           Targets in namespace \"package\" are to group other targets and maybe do some configuration.")
		if self.default_target!=None: print("           Default target name:", normpath(self.default_target) if self.default_namespace=="path" else self.default_target)
		print("Options.")
		print("  -h                   --help                     Show help (this message).")
		print("  -l                   --license                  Show license notice.")
		print("  -D <name>{=<value>}  --define <name>{=<value>}  Pass an argument to the build script with optional value.")
		print("  -v                   --verbose                  Show more verbose output from target recipes.")
		print("                       --pack{=<prefix>}          An auxiliary option used by MakePy to pack packages of some types using external tools.")
		if len(self.targets)!=0:
			print()
			print("Build script summary.")
			print("Available actions.")
			for action in sorted(self.targets):
				if "package" in self.targets[action]:
					print(" ", action, "(packages: ", end="")
					print(*sorted(self.targets[action]["package"]), sep=", ", end="")
					print(")")
				else: print(" ", action)
		exit()
	def license(self):
		'''
		A license notice. You can override this method to add your license notice to the build script. Do it like that:
			def license(self):
				super().license()
				print()
				print("Build script license:")
				print(...)
				...
		'''
		print("MakePy framework license:")
		print("MIT Expat license")
		print()
		print("(C) 2025 Seriy MLGamer <Seriy-MLGamer@yandex.ru>")
		print()
		print("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:")
		print()
		print("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.")
		print()
		print("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.")
	def command(self, command):
		'''
		Run a command.

		Args:
		command (list[str]): the list of command line arguments.

		Returns:
		int: exit status.
		'''
		if self.VERBOSE: print(" ", *command)
		return run(command).returncode
	def command_chain(self, *commands):
		'''
		Run multiple commands consecutively. If one of commands exits with error, subsequent commands are not run.

		Args:
		*commands (list[str]): the lists of command line arguments.

		Returns:
		int: exit status of last executed command.
		'''
		for command in commands:
			if self.VERBOSE: print(" ", *command)
			error=run(command).returncode
			if error!=0: return error
		return 0
	def executable(self, name):
		'''
		Args:
		name (str): the name of executable file.

		Returns:
		str: a path to executable file being built.
		'''
		if self.TARGET_OPERATING_SYSTEM=="Windows": return join(self.BIN, name+".exe")
		return join(self.BIN, name)
	def static_library(self, name):
		'''
		Args:
		name (str): the name of static library.

		Returns:
		str: a path to static library being built.
		'''
		return join(self.LIB, "lib"+name+".a")
	def shared_library(self, name, revision=None):
		'''
		Args:
		name (str): the name of shared library.
		revision (str): the revision of shared library. Optional.

		Returns:
		str: a path to shared library being built.
		'''
		if self.TARGET_OPERATING_SYSTEM=="Linux":
			if revision==None: return join(self.LIB, "lib"+name+".so")
			return join(self.LIB, "lib"+name+".so."+revision)
		if self.TARGET_OPERATING_SYSTEM=="Windows":
			if revision==None: return join(self.BIN, "lib"+name+".dll")
			return join(self.BIN, "lib"+name+"-"+revision+".dll")
	def install(self, path):
		'''
		Args:
		path (str): the relative install path.

		Returns:
		str: a correct full install path.
		'''
		if self.pack_prefix!=None: return join(self.pack_prefix, path)
		if self.pack: return join(self.PACK_PREFIX, path)
		return join(self.INSTALL_PREFIX, path)
	def deb(self, package, version, architecture="any"):
		'''
		Args:
		package (str): the package name.
		version (str): the package version.
		architecture (str): the package architecture. Default value is "any".

		Returns:
		str: a path to a ".deb" package.
		'''
		if architecture=="any":
			if self.TARGET_ARCHITECTURE=="x86_64" or self.TARGET_ARCHITECTURE=="AMD64": architecture="amd64"
			elif self.TARGET_ARCHITECTURE=="i386" or self.TARGET_ARCHITECTURE=="i686" or self.TARGET_ARCHITECTURE=="x86": architecture="i386"
			elif self.TARGET_ARCHITECTURE=="aarch64" or self.TARGET_ARCHITECTURE=="ARM64": architecture="arm64"
			elif "armv7" in self.TARGET_ARCHITECTURE: architecture="armhf"
			elif "arm" in self.TARGET_ARCHITECTURE: architecture="armel"
			else: architecture=self.TARGET_ARCHITECTURE
		return join(self.OUT, package+"_"+version+"_"+architecture+".deb")
	def main(self, defines):
		'''
		It's a user-defined method. Override it in your build script to initialize targets.

		Args:
		defines (dict[str, any]): arguments passed to the build script from command line.
		'''
		pass
def file_time(path):
	'''
	Get file modification time.

	Args:
	path (str): the path to file.

	Returns:
	float: file modification time or 0 if file doesn't exist.
	'''
	if not exists(path): return 0
	return max(getmtime(path), getctime(path))
def clean_key(target):
	'''
	Key function to sort clean targets.

	Args:
	target (str): a clean target.

	Returns:
	int: sorting order.
	'''
	target=abspath(target)
	result=0
	while not ismount(target):
		target=dirname(target)
		result+=1
	return result
class Target:
	'''
	The base class for all of the targets.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	action (str): the action being performed on the target.
	target (str): the unique in the action or namespace scopes name of the target. The instance of Target can be found by this name.
	dependencies (dict[str, Dependencies]): the dictionary of targets to make before making this target, i. e. dependencies. The keys of the dictionary are the actions being performed on dependencies. Optional (target can have no dependencies).
	'''
	def __init__(self, host, action, target, dependencies=None):
		self.host=host
		self.action=action
		self.target=target
		self.dependencies={} if dependencies==None else dependencies
	def make_dependencies(self):
		'''
		A common subroutine to iterate and make targets-dependencies.
		'''
		for action in self.dependencies:
			if action in self.host.targets:
				if "package" in self.host.targets[action]: targets=self.host.targets[action]["package"]
				elif len(self.dependencies[action].packages)!=0: error_no_target(action, "package", self.dependencies[action].packages[0])
				for name in self.dependencies[action].packages:
					if name not in targets: error_no_target(action, "package", name)
					targets[name].make(False)
				if "path" in self.host.targets[action]:
					targets=self.host.targets[action]["path"]
					for name in self.dependencies[action].updatable:
						if normcase(name) in targets: targets[normcase(name)].make(False)
						if not lexists(name): error_path(name)
					for name in self.dependencies[action].static:
						if not lexists(name):
							if normcase(name) in targets: targets[normcase(name)].make(True)
							if not lexists(name): error_path(name)
				else:
					for name in self.dependencies[action].updatable:
						if not lexists(name): error_path(name)
					for name in self.dependencies[action].static:
						if not lexists(name): error_path(name)
			elif len(self.dependencies[action].packages)!=0: error_no_target(action, "package", self.dependencies[action].packages[0])
			else:
				for name in self.dependencies[action].updatable:
					if not lexists(name): error_path(name)
				for name in self.dependencies[action].static:
					if not lexists(name): error_path(name)
	def make(self, update):
		'''
		A virtual method to make a certain type of target.

		Args:
		update (bool): do make the target (execute its recipe). If False, several checks are made before making the target itself.
		'''
		pass
class Dependencies:
	'''
	Lists of targets-dependencies which are divided into 3 groups: updatable paths, static paths and packages.

	Args:
	updatable (list[str]): the list of updatable paths (namespace "path"), e. g. ordinary files. If modification time of an updatable path is greater than modification time of a dependent path target, then the dependent target is made. Optional.
	static (list[str]): the list of static paths (namespace "path"), e. g. folders or sometimes symbolic links. A dependent target is made only if a static path doesn't exist in the filesystem. Optional.
	packages (list[str]): the list of packages (namespace "package"). Packages are always made, so dependent target is made if it has at least 1 package dependency. Optional.
	'''
	def __init__(self, updatable=None, static=None, packages=None):
		self.updatable=[] if updatable==None else list(map(normpath, updatable))
		self.static=[] if static==None else list(map(normpath, static))
		self.packages=[] if packages==None else packages
class Path(Target):
	'''
	A path type of target. Name of a target belongs to namespace "path" and corresponds to a path of the filesystem (file, folder, symbolic link, etc). Checks of path existence and its modification time are made before making this type of target.
	When an instance of Path is created, it's automatically considered as a clean target for a subsequent Clean target. You can configure this behavior using host.AUTO_CLEAN field (True to enable, False to disable).
	Creation of an instance also automatically creates targets for parent folders. You can configure this behavior using host.AUTO_FOLDERS field (True to enable, False to disable).

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	action (str): the action being performed on the target.
	path (str): the unique in the action or namespace scopes name of the target. It has to be a correct filesystem path. The instance of Path can be found by this name in namespace "path".
	dependencies (dict[str, Dependencies]): the dictionary of targets to make before making this target, i. e. dependencies. The keys of the dictionary are the actions being performed on dependencies. Optional (target can have no dependencies).
	'''
	def __init__(self, host, action, path, dependencies=None):
		path=normpath(path)
		super().__init__(host, action, path, dependencies)
		if action in host.targets:
			if "path" in host.targets[action]: host.targets[action]["path"][normcase(path)]=self
			else: host.targets[action]["path"]={normcase(path): self}
		else: host.targets[action]={"path": {normcase(path): self}}
		parent=dirname(path)
		if parent!="/usr/local" and parent!="/usr":
			if host.AUTO_CLEAN: host.clean_targets.add(normcase(path))
			path=dirname(parent)
			if host.AUTO_FOLDERS:
				if not (normcase(parent) in host.targets[action]["path"] or len(parent)==0 or path=="/usr/local" or path=="/usr" or ismount(parent)):
					Folder(host, parent, action)
					parent=path
					path=dirname(path)
				if host.AUTO_CLEAN:
					parent=normcase(parent)
					while not (parent in host.clean_targets or len(parent)==0 or path=="/usr/local" or path=="/usr" or ismount(parent)):
						host.clean_targets.add(parent)
						parent=path
						path=dirname(path)
	def make(self, update):
		'''
		Make a path target.

		Args:
		update (bool): do make the target (execute its recipe). If False, checks of path existence and its modification time are made before making the target itself.
		'''
		self.make_dependencies()
		if not update:
			if not lexists(self.target): update=True
			else:
				time=file_time(self.target)
				for action in self.dependencies:
					for path in self.dependencies[action].updatable:
						if file_time(path)>time:
							update=True
							break
					if update: break
				if not update:
					for action in self.dependencies:
						if len(self.dependencies[action].packages)!=0:
							update=True
							break
		if update:
			print(self.action, "path:"+self.target)
			result=self.recipe()
			if result!=0: error_recipe(result)
	def recipe(self):
		'''
		A set of commands, instructions, actions, etc. needed to reproduce the path in the filesystem.

		Returns:
		int: exit status. If not 0, a whole build process terminates with error.
		'''
		return 0
class Package(Target):
	'''
	The package target. Its purpose is to group other targets and maybe do some configuration. Name of the target belongs to namespace "package". It's just any convenient name.
	It's always made whether it's an explicit build target or a dependency of a target being built. But it can be made only once or every time these conditions are met. You can configure this behavior using host.ONCE field (True to make once, False to make every time).

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	action (str): the action being performed on the target.
	name (str): the unique in the action or namespace scopes name of the target. The instance of Package can be found by this name in namespace "package".
	dependencies (dict[str, Dependencies]): the dictionary of targets to make before making this target, i. e. dependencies. The keys of the dictionary are the actions being performed on dependencies. Optional (target can have no dependencies).
	'''
	def __init__(self, host, action, name, dependencies=None):
		if host.pack and action==host.ACTION_INSTALL:
			if dependencies==None: super().__init__(host, action, name)
			else:
				new_dependencies={}
				for act in dependencies: new_dependencies[act]=dependencies[act]
				if host.ACTION_INSTALL in dependencies: new_dependencies[host.ACTION_INSTALL]=Dependencies(dependencies[host.ACTION_INSTALL].updatable, dependencies[host.ACTION_INSTALL].static)
				super().__init__(host, action, name, new_dependencies)
			self.pack=True
		else:
			super().__init__(host, action, name, dependencies)
			self.pack=False
		self.done=False
		self.once=host.ONCE
		if action in host.targets:
			if "package" in host.targets[action]: host.targets[action]["package"][name]=self
			else: host.targets[action]["package"]={name: self}
		else: host.targets[action]={"package": {name: self}}
	def make(self, update):
		'''
		Make a package target.

		Args:
		update (bool): do make the target (execute its recipe). If False, it depends on self.once field. If it's True, the target is made if it has never been made.
		'''
		if update or not (self.done and self.once):
			self.make_dependencies()
			print(self.action, "package:"+self.target)
			if not self.pack:
				result=self.recipe()
				if result!=0: error_recipe(result)
			self.done=True
	def recipe(self):
		'''
		Some commands. instructions, actions, etc. to do something.

		Returns:
		int: exit status. If not 0, a whole build process terminates with error.
		'''
		return 0
class Clean(Package):
	'''
	Clean working folder from paths generated during build process.
	To use this target correctly, first create path targets you consider belonging to some group that has to be cleaned as a whole (that group is usually some package). Then create this target. It will know what to clean.
	Action of this target is defined in host.ACTION_CLEAN.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	package (str): the name of the package cotntaining the paths above this target.
	'''
	def __init__(self, host, package):
		super().__init__(host, host.ACTION_CLEAN, package)
		self.targets=sorted(host.clean_targets, key=clean_key, reverse=True)
		host.clean_targets=set()
	def recipe(self):
		'''
		It deletes paths from the filesystem.

		Returns:
		int: 0. If some paths don't exist or can't be deleted, it's considered fine.
		'''
		for target in self.targets:
			try:
				if target in self.host.clean_trees: rmtree(target)
				elif isdir(target): rmdir(target)
				else: remove(target)
				if self.host.VERBOSE: print("  Deleted \"", target, "\".", sep="")
			except FileNotFoundError:
				if self.host.VERBOSE: print("  Already deleted \"", target, "\".", sep="")
			except:
				if self.host.VERBOSE: print("  Can't delete \"", target, "\".", sep="")
		return 0
class Remove(Clean):
	'''
	Remove installed paths.
	To use this target correctly, first create path targets you consider belonging to some group that has to be removed as a whole (they are usually targets of some package). Then create this target. It will know what to clean. It will also calculate reverse dependencies on other packages with action as in host.ACTION_INSTALL after executing host.main().
	Action of this target is defined in host.ACTION_REMOVE.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	package (str): the name of the package the paths above this target belong to.
	'''
	def __init__(self, host, package):
		Package.__init__(self, host, host.ACTION_REMOVE, package, {host.ACTION_REMOVE: Dependencies()})
		self.targets=sorted(host.clean_targets, key=clean_key, reverse=True)
		host.clean_targets=set()
		if not host.pack: host.remove_targets.append(package)
class Folder(Path):
	'''
	Create a folder.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	path (str): the path to the folder.
	action (str): the action of the target. Default is host.ACTION_BUILD.
	permissions (int): folder's access permissions in octal format. If omitted, system default permissions are used.
	'''
	def __init__(self, host, path, action=None, permissions=-1):
		if action==None: action=host.ACTION_BUILD
		parent=dirname(path)
		super().__init__(host, action, path, {action: Dependencies(static=[] if len(parent)==0 else [parent])})
		self.permissions=permissions
	def recipe(self):
		'''
		A recipe to create a folder.

		Returns:
		int: 0 on success, 1 on failure.
		'''
		if self.host.VERBOSE: print("  Create folder \"", self.target, "\".", sep="")
		try:
			if self.permissions!=-1: mkdir(self.target, self.permissions)
			else: mkdir(self.target)
		except: return 1
		return 0
class Symbolic_link(Path):
	'''
	Create a symbolic link.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	path (str): the path the link will point to. If this path is not absolute, it is relative to the link location.
	folder (str): the path to the folder where the link will be located. Default is working folder.
	name (str): the name of the link. Default is basename of the path.
	action (str): the action of the target. Default is host.ACTION_BUILD.
	'''
	def __init__(self, host, path, folder=None, name=None, action=None):
		self.path=normpath(path)
		if name==None: name=basename(path)
		if folder!=None:
			if not isabs(path): path=join(folder, path)
			name=join(folder, name)
		if action==None: action=host.ACTION_BUILD
		super().__init__(host, action, name, {action: Dependencies([path], [] if folder==None else [folder])})
	def recipe(self):
		'''
		A recipe to create a symbolic link.

		Returns:
		int: 0 on success, 1 on failure.
		'''
		if islink(self.target): return 0
		try: symlink(self.path, self.target)
		except: return 1
		return 0
class Object(Path):
	'''
	A base class to compile an object file. It will be located in a folder defined in host.OBJ.
	Common compile flags are defined in host.CFLAGS.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	source (str): the path to the source code file. If it is not absolute, it is relative to a path defined in host.SRC. An object file will have the name of this source file and extension ".o".
	cflags (list[str]): the compile flags specific to this target. Optional.
	'''
	def __init__(self, host, source, cflags=None):
		super().__init__(host, host.ACTION_BUILD, join(host.OBJ, splitext(basename(source))[0]+".o"), {host.ACTION_BUILD: Dependencies([source if isabs(source) else join(host.SRC, source)], [] if len(host.OBJ)==0 else [host.OBJ])})
		self.compiler=None
		self.cflags=([] if cflags==None else cflags)+host.CFLAGS
	def recipe(self):
		'''
		A recipe to compile an object file from the source code written in C or C++.

		Returns:
		int: exit status of a compiler.
		'''
		return self.host.command([self.compiler, "-c", "-o", self.target, self.dependencies[self.action].updatable[0]]+self.cflags)
class Object_C(Object):
	'''
	Compile an object file from the source code written in C. It will be located in a folder defined in host.OBJ.
	A compiler being used is defined in host.CC.
	Common compile flags are defined in host.CFLAGS.
	Compile flags specific to a C compiler are defined in host.C_COMPILE_FLAGS.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	source (str): the path to the source code file. If it is not absolute, it is relative to a path defined in host.SRC. An object file will have the name of this source file and extension ".o".
	cflags (list[str]): the compile flags specific to this target. Optional.
	'''
	def __init__(self, host, source, cflags=None):
		super().__init__(host, source, cflags)
		self.compiler=normpath(host.CC)
		self.cflags+=host.C_COMPILE_FLAGS
class Object_Cpp(Object):
	'''
	Compile an object file from the source code written in C++. It will be located in a folder defined in host.OBJ.
	A compiler being used is defined in host.CXX.
	Common compile flags are defined in host.CFLAGS.
	Compile flags specific to a C++ compiler are defined in host.CPP_COMPILE_FLAGS.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	source (str): the path to the source code file. If it is not absolute, it is relative to a path defined in host.SRC. An object file will have the name of this source file and extension ".o".
	cflags (list[str]): the compile flags specific to this target. Optional.
	'''
	def __init__(self, host, source, cflags=None):
		super().__init__(host, source, cflags)
		self.compiler=normpath(host.CXX)
		self.cflags+=host.CPP_COMPILE_FLAGS
class Executable(Path):
	'''
	A base class to link an executable file. It will be located in a folder defined in host.BIN.
	Common link flags are defined in host.LDFLAGS.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	name (str): the name of executable file. On Windows, extension ".exe" will be added.
	*objects (str): the object files required to link an executable. If some path is not absolute, it is relative to a path defined in host.OBJ.
	ldflags (list[str]): the link flags specific to this target. Optional.
	'''
	def __init__(self, host, name, *objects, ldflags=None):
		super().__init__(host, host.ACTION_BUILD, host.executable(name), {host.ACTION_BUILD: Dependencies([object if isabs(object) else join(host.OBJ, object) for object in objects], [] if len(host.BIN)==0 else [host.BIN])})
		self.compiler=None
		self.ldflags=([] if ldflags==None else ldflags)+host.LDFLAGS
	def recipe(self):
		'''
		A recipe to link an executable file from the object files produced by C or C++ compiler.

		Returns:
		int: exit status of a compiler.
		'''
		return self.host.command([self.compiler, "-o", self.target]+self.dependencies[self.action].updatable+self.ldflags)
class Executable_C(Executable):
	'''
	Link an executable file from the object files produced by C compiler. It will be located in a folder defined in host.BIN.
	A compiler being used is defined in host.CC.
	Common link flags are defined in host.LDFLAGS.
	Link flags specific to a C compiler are defined in host.C_LINK_FLAGS.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	name (str): the name of executable file. On Windows, extension ".exe" will be added.
	*objects (str): the object files required to link an executable. If some path is not absolute, it is relative to a path defined in host.OBJ.
	ldflags (list[str]): the link flags specific to this target. Optional.
	'''
	def __init__(self, host, name, *objects, ldflags=None):
		super().__init__(host, name, *objects, ldflags=ldflags)
		self.compiler=normpath(host.CC)
		self.ldflags+=host.C_LINK_FLAGS
class Executable_Cpp(Executable):
	'''
	Link an executable file from the object files produced by C++ compiler. It will be located in a folder defined in host.BIN.
	A compiler being used is defined in host.CXX.
	Common link flags are defined in host.LDFLAGS.
	Link flags specific to a C++ compiler are defined in host.CPP_LINK_FLAGS.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	name (str): the name of executable file. On Windows, extension ".exe" will be added.
	*objects (str): the object files required to link an executable. If some path is not absolute, it is relative to a path defined in host.OBJ.
	ldflags (list[str]): the link flags specific to this target. Optional.
	'''
	def __init__(self, host, name, *objects, ldflags=None):
		super().__init__(host, name, *objects, ldflags=ldflags)
		self.compiler=normpath(host.CXX)
		self.ldflags+=host.CPP_LINK_FLAGS
class Static_library(Path):
	'''
	Link a static library from the object files. It will be located in a folder defined in host.LIB.
	An archiver being used is defined in host.AR.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	name (str): the name of static library. Prefix "lib" and extension ".a" will be added.
	*objects (str): the object files required to link a static library. If some path is not absolute, it is relative to a path defined in host.OBJ.
	'''
	def __init__(self, host, name, *objects):
		super().__init__(host, host.ACTION_BUILD, host.static_library(name), {host.ACTION_BUILD: Dependencies([object if isabs(object) else join(host.OBJ, object) for object in objects], [] if len(host.LIB)==0 else [host.LIB])})
		self.archiver=normpath(host.AR)
	def recipe(self):
		'''
		A recipe to link a static library from the object files.

		Returns:
		int: exit status of an archiver.
		'''
		return self.host.command([self.archiver, "rcs", self.target]+self.dependencies[self.action].updatable)
class Shared_library(Path):
	'''
	A base class to link a shared library. On GNU/Linux, it will be located in a folder defined in host.LIB. On Windows, it will be located in a folder defined in host.BIN.
	Common link flags are defined in host.LDFLAGS.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	name (str): the name of shared library. Prefix "lib" and extension ".so" (on GNU/Linux) or ".dll" (on Windows) will be added.
	*objects (str): the object files required to link a shared library. If some path is not absolute, it is relative to a path defined in host.OBJ.
	revision (str): the revision of shared library. Optional.
	ldflags (list[str]): the link flags specific to this target. Optional.
	'''
	def __init__(self, host, name, *objects, revision=None, ldflags=None):
		name=host.shared_library(name, revision)
		super().__init__(host, host.ACTION_BUILD, name, {host.ACTION_BUILD: Dependencies([object if isabs(object) else join(host.OBJ, object) for object in objects], [] if len(dirname(name))==0 else [dirname(name)])})
		self.os=host.TARGET_OPERATING_SYSTEM
		self.compiler=None
		self.ldflags=([] if ldflags==None else ldflags)+host.LDFLAGS
	def recipe(self):
		'''
		A recipe to link a shared library from the object files produced by C or C++ compiler.

		Returns:
		int: exit status of a compiler.
		'''
		if self.os=="Linux":
			name=splitext(basename(self.target))
			major=None
			while name[1]!=".so":
				major=name[1]
				name=splitext(name[0])
			if major!=None: return self.host.command([self.compiler, "-shared", "-fPIC", "-Wl,-soname,"+name[0]+name[1]+major, "-o", self.target]+self.dependencies[self.action].updatable+self.ldflags)
		return self.host.command([self.compiler, "-shared", "-fPIC", "-o", self.target]+self.dependencies[self.action].updatable+self.ldflags)
class Shared_library_C(Shared_library):
	'''
	Link a shared library from the object files produced by C compiler. On GNU/Linux, it will be located in a folder defined in host.LIB. On Windows, it will be located in a folder defined in host.BIN.
	A compiler being used is defined in host.CC.
	Common link flags are defined in host.LDFLAGS.
	Link flags specific to a C compiler are defined in host.C_LINK_FLAGS.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	name (str): the name of shared library. Prefix "lib" and extension ".so" (on GNU/Linux) or ".dll" (on Windows) will be added.
	*objects (str): the object files required to link a shared library. If some path is not absolute, it is relative to a path defined in host.OBJ.
	revision (str): the revision of shared library. Optional.
	ldflags (list[str]): the link flags specific to this target. Optional.
	'''
	def __init__(self, host, name, *objects, revision=None, ldflags=None):
		super().__init__(host, name, *objects, revision=revision, ldflags=ldflags)
		self.compiler=normpath(host.CC)
		self.ldflags+=host.C_LINK_FLAGS
class Shared_library_Cpp(Shared_library):
	'''
	Link a shared library from the object files produced by C++ compiler. On GNU/Linux, it will be located in a folder defined in host.LIB. On Windows, it will be located in a folder defined in host.BIN.
	A compiler being used is defined in host.CXX.
	Common link flags are defined in host.LDFLAGS.
	Link flags specific to a C++ compiler are defined in host.CPP_LINK_FLAGS.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	name (str): the name of shared library. Prefix "lib" and extension ".so" (on GNU/Linux) or ".dll" (on Windows) will be added.
	*objects (str): the object files required to link a shared library. If some path is not absolute, it is relative to a path defined in host.OBJ.
	revision (str): the revision of shared library. Optional.
	ldflags (list[str]): the link flags specific to this target. Optional.
	'''
	def __init__(self, host, name, *objects, revision=None, ldflags=None):
		super().__init__(host, name, *objects, revision=revision, ldflags=ldflags)
		self.compiler=normpath(host.CXX)
		self.ldflags+=host.CPP_LINK_FLAGS
class Static_DLL_interface(Path):
	'''
	Generate a static interface library for the ".dll" shared library. Unlike ".so" shared libraries, ".dll" shared libraries can't be linked by compiler the way they become linked at program startup. So, they need static interface libraries to be generated in order to link ".dll" this way.
	A static interface library will be located in a folder defined in host.LIB.
	A generator being used is defined in host.DLLTOOL.
	A ".def" file extractor being used is defined in host.GENDEF.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	name (str): the name of static library. Prefix "lib" and extension ".a" will be added.
	dll (str): the path to the ".dll" shared library required to generate a static interface library. If it is not absolute, it is relative to a path defined in host.BIN.
	deffile (str): the path to the ".def" file with shared library symbols. Optional (it can be extracted automatically from the library).
	'''
	def __init__(self, host, name, dll, deffile=None):
		super().__init__(host, host.ACTION_BUILD, host.static_library(name), {host.ACTION_BUILD: Dependencies([dll if isabs(dll) else join(host.BIN, dll)]+([] if deffile==None else [deffile]), [] if len(host.LIB)==0 else [host.LIB])})
		self.generator=normpath(host.DLLTOOL)
		self.extractor=normpath(host.GENDEF)
	def recipe(self):
		'''
		A recipe to generate a static interface library for the ".dll" shared library and sometimes extract a temporary ".def" file before that.

		Returns:
		int: exit status of a procedure sequence.
		'''
		if len(self.dependencies[self.action].updatable)==2: return self.host.command([self.generator, "--dllname", self.dependencies[self.action].updatable[0], "--input-def", self.dependencies[self.action].updatable[1], "--output-lib", self.target])
		dll=self.dependencies[self.action].updatable[0]
		deffile=splitext(basename(dll))[0]+".def"
		result=self.host.command_chain(
			[self.extractor, dll],
			[self.generator, "--dllname", dll, "--input-def", deffile, "--output-lib", self.target]
		)
		if result!=0: return result
		if self.host.VERBOSE: print("  Delete temporary file \"", deffile, "\".", sep="")
		try: remove(deffile)
		except: return 1
		return 0
class Def(Path):
	'''
	Extract a ".def" file from the ".dll" shared library. It's needed, for example, to generate a static interface library for ".dll" shared library via Static_DLL_interface target.
	A ".def" file will have a name of the ".dll" shared library with extension ".def" instead of ".dll".
	A ".def" file extractor being used is defined in host.GENDEF.
	Action of this target is defined in host.ACTION_BUILD.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	dll (str): the path to the ".dll" shared library required to generate a static interface library. If it is not absolute, it is relative to a path defined in host.BIN.
	folder (str): the path to the folder where the ".def" file will be located. Default is working folder.
	'''
	def __init__(self, host, dll, folder=None):
		super().__init__(host, host.ACTION_BUILD, splitext(basename(dll))[0]+".def" if folder==None else join(folder, splitext(basename(dll))[0]+".def"), {host.ACTION_BUILD: Dependencies([dll if isabs(dll) else join(host.BIN, dll)], [] if folder==None else [folder])})
		self.extractor=normpath(host.GENDEF)
	def recipe(self):
		'''
		A recipe to extract a ".def" file from the ".dll" shared library.

		Returns:
		int: exit status of an extractor.
		'''
		return self.host.command([self.extractor, dll])
class Install(Path):
	'''
	Install (copy) the file, or the folder (recursively), or the symbolic link to the specified path with the specified name.
	Action of this target is defined in host.ACTION_INSTALL.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	path (str): the path to the file/folder/symbolic link being installed.
	folder (str): the path to the folder where the chosen path will be located. If it is not absolute, it is relative to a path defined in host.INSTALL_PREFIX. If omitted, the chosen path will be put into host.INSTALL_PREFIX.
	name (str): the name of a path being installed. Default is basename of the path.
	'''
	def __init__(self, host, path, folder=None, name=None):
		if name==None: name=basename(path)
		if folder==None:
			if host.pack_prefix!=None: folder=host.pack_prefix
			elif host.pack: folder=host.PACK_PREFIX
			else: folder=host.INSTALL_PREFIX
		elif not isabs(folder): folder=host.install(folder)
		name=join(folder, name)
		super().__init__(host, host.ACTION_INSTALL, name, {
			host.ACTION_BUILD: Dependencies([path]),
			host.ACTION_INSTALL: Dependencies(static=[] if len(folder)==0 else [folder])
		})
		if isdir(path) and not islink(path):
			self.folder=True
			host.clean_trees.add(normcase(normpath(name)))
		else: self.folder=False
	def recipe(self):
		'''
		An install recipe.

		Returns:
		int: 0 on success, 1 on failure.
		'''
		name=self.dependencies[self.host.ACTION_BUILD].updatable[0]
		try:
			if self.host.VERBOSE: print("  Copy path \"", name, "\" to \"", self.target, "\".", sep="")
			if self.folder: copytree(name, self.target, symlinks=True)
			else: copy(name, self.target, follow_symlinks=False)
		except: return 1
		return 0
class Pack_archive(Package):
	'''
	Pack an archive package with installed files.
	It's a package target that works like an alias for convenient command line access to an underlying Archive target.
	An archive will be located in a folder defined in host.OUT.
	Action of this target is defined in host.ACTION_PACK.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	package (str): the name of the package or group of packages to be packed.
	name (str): the path to the archive with one of those required and supported extensions: ".tar.gz", ".tar.xz", ".tar.bz2", ".tar", ".zip". If it is not absolute, it is relative to a path defined in host.OUT.
	*packages (str): the packages being included into archive. By default, package defined in package argument is included.
	'''
	def __init__(self, host, package, name, *packages):
		super().__init__(host, host.ACTION_PACK, package, {host.ACTION_PACK: Dependencies([name if isabs(name) else join(host.OUT, name)])})
		if len(packages)==0: Archive(host, name, package)
		else: Archive(host, name, *packages)
class Archive(Path):
	'''
	Pack an archive package with installed files.
	It's a path target with a recipe to pack archive packages.
	An archive will be located in a folder defined in host.OUT.
	Action of this target is defined in host.ACTION_PACK.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	name (str): the path to the archive with one of those required and supported extensions: ".tar.gz", ".tar.xz", ".tar.bz2", ".tar", ".zip". If the path is not absolute, it is relative to a path defined in host.OUT.
	*packages (str): the packages being included into archive. At least one package must be set.
	'''
	def __init__(self, host, name, *packages):
		clean=host.AUTO_CLEAN
		host.AUTO_CLEAN=False
		super().__init__(host, host.ACTION_PACK, name if isabs(name) else join(host.OUT, name), {
			host.ACTION_INSTALL: Dependencies(packages=packages),
			host.ACTION_PACK: Dependencies(static=[] if isabs(name) or len(host.OUT)==0 else [host.OUT])
		})
		host.AUTO_CLEAN=clean
	def recipe(self):
		'''
		A recipe to pack an archive package with files installed into a temporary folder.

		Returns:
		int: exit status of the procedure sequence. If the sequence is interrupted, sometimes the temporary folder and installed files are not removed.
		'''
		if self.host.VERBOSE: print("  Check extension.")
		name=splitext(basename(self.target))
		if name[1]==".gz":
			new_name=splitext(name[0])
			if new_name[1]!=".tar": return 1
			name=new_name
			format="gztar"
		elif name[1]==".bz2":
			new_name=splitext(name[0])
			if new_name[1]!=".tar": return 1
			name=new_name
			format="bztar"
		elif name[1]==".xz":
			new_name=splitext(name[0])
			if new_name[1]!=".tar": return 1
			name=new_name
			format="xztar"
		elif name[1]==".tar": format="tar"
		elif name[1]==".zip": format="zip"
		else: return 1
		if self.host.VERBOSE: print("  Copy installed paths into folder \"", name[0], "\".", sep="")
		try: copytree(self.host.PACK_PREFIX, join(self.host.PACK_PREFIX, name[0]), symlinks=True)
		except: return 2
		if self.host.VERBOSE: print("  Make archive \"", self.target, "\".", sep="")
		try: make_archive(join(dirname(self.target), name[0]), format, self.host.PACK_PREFIX, name[0])
		except: return 3
		if self.host.VERBOSE: print("  Remove installed files.")
		try: rmtree(self.host.PACK_PREFIX)
		except: return 4
		if self.host.VERBOSE: print("  Remove some more folders if possible.")
		parent=dirname(self.host.PACK_PREFIX)
		while len(parent)!=0 and not ismount(parent):
			try: rmdir(parent)
			except: break
			parent=dirname(parent)
		return 0
class Deb(Path):
	'''
	Pack a ".deb" package using tools from Debian package "debhelper" and a special "debian/rules" file provided with MakePy framework as "sample debian rules".
	A package with a standard Debian name "<package>_<version>_<architecture>.deb" will be packed. It will be located in a folder defined in host.OUT.
	Action of this target is defined in host.ACTION_PACK.

	Args:
	host (MakePy): the instance of MakePy the target will be bound to and interact with.
	package (str): the name of the package to be packed.
	version (str): the package version.
	architecture (str): the package architecture. Default value is "any".
	'''
	def __init__(self, host, package, version, architecture="any"):
		super().__init__(host, host.ACTION_PACK, host.deb(package, version, architecture), {host.ACTION_PACK: Dependencies(["debian/rules"], [] if len(host.OUT)==0 else [host.OUT])})
		self.package=package
		if architecture=="any":
			if host.TARGET_ARCHITECTURE=="x86_64" or host.TARGET_ARCHITECTURE=="AMD64": self.architecture="amd64"
			elif host.TARGET_ARCHITECTURE=="i386" or host.TARGET_ARCHITECTURE=="i686" or host.TARGET_ARCHITECTURE=="x86": self.architecture="i386"
			elif host.TARGET_ARCHITECTURE=="aarch64" or host.TARGET_ARCHITECTURE=="ARM64": self.architecture="arm64"
			elif "armv7" in host.TARGET_ARCHITECTURE: self.architecture="armhf"
			elif "arm" in host.TARGET_ARCHITECTURE: self.architecture="armel"
			else: self.architecture=host.TARGET_ARCHITECTURE
		else: self.architecture=architecture
	def recipe(self):
		'''
		A recipe to build a ".deb" package. It executes "debian/rules" makefile, and it in turn executes the build script with different actions and, of course, tools from Debian package "debhelper". Make sure to install it.
		Additional files that can't be automatically removed can be created.

		Returns:
		int: exit status of a makefile.
		'''
		return self.host.command(["env", "DEB_HOST_ARCH="+self.architecture, "fakeroot", "debian/rules", "binary", "PACKAGE="+self.package, "OUT="+self.target]+(["FLAGS=--verbose"] if self.host.VERBOSE else []))
def error(): print(basename(argv[0]), ": error: ", sep="", end="", file=stderr)
def error_argument(argument):
	error()
	print("invalid argument:", argument, file=stderr)
	print("Tip: enter command \"", basename(argv[0]), " --help\" to learn script usage.", sep="")
	exit(1)
def error_namespace(namespace):
	error()
	print("invalid namespace:", namespace, file=stderr)
	print("Tip: currently, only namespaces \"path\" and \"package\" are present in MakePy.")
	exit(2)
def error_action():
	error()
	print("action is not defined.", file=stderr)
	print("Tip: either enter the action in command line or set default action in the build script via constructor:")
	print("    <MakePy derivative>(default_action=<action>)")
	exit(3)
def error_target():
	error()
	print("target is not defined.", file=stderr)
	print("Tip: either enter the targets in command line after the action or set default target in the build script via constructor:")
	print("    <MakePy derivative>(default_target=<target>)")
	exit(4)
def error_no_action(action):
	error()
	print("unavailable action:", action, file=stderr)
	print("Tip: enter command \"", basename(argv[0]), " --help\" to see a list of available actions.", sep="")
	exit(5)
def error_no_target(action, namespace, target):
	error()
	print("no recipe to perform action \"", action, "\" on target \"", namespace, ":", target, "\".", sep="", file=stderr)
	print("Tip: create an instance of Target for this target in the build script.")
	exit(6)
def error_path(path):
	error()
	print("path has not been created:", path, file=stderr)
	exit(7)
def error_recipe(status):
	error()
	print("recipe has returned exit status:", status, file=stderr)
	exit(status)