from platform import system

truestr = ['true', 'True', 'TRUE', '1']

debug = ARGUMENTS.get('debug') in truestr
example = ARGUMENTS.get('example') in truestr

env = Environment(CPPPATH=["include"])

if debug:
    env.Append(CPPDEFINES=['DEBUG'])

src = Glob("./src/*.c")
example_src = Glob("./example/*.c")

lib = env.StaticLibrary("6502", src)

if example:
    env.Program("example", example_src + lib)
