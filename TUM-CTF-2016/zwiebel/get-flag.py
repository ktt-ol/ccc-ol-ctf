#!/usr/bin/env python

from __future__ import print_function
import sys
import r2pipe
import subprocess
import os

keydata = {}

def build_ptrace_lib():
    # simple library for LD_PRELOAD, which unblocks ptrace
    f = open("ptrace.c", "w")
    f.write("long ptrace(int request, ...) { return 0; }")
    f.close()
    subprocess.check_call(["gcc", "-shared", "-o", "ptrace.so", "ptrace.c"])

def clean_ptrace_lib():
    # delete temporary files
    os.remove("ptrace.c")
    os.remove("ptrace.so")

def keydata2string():
    # generate string from the keydata we found
    str = ""
    for i in sorted(keydata.keys()):
        byte = 0x00
        for b in sorted(keydata[i].keys()):
            if keydata[i][b] == 1:
                byte |= b
            else:
                byte &= ~b
        str += chr(byte)
    return str

def extract_block_info():
    # disassemble current block
    ops = r2.cmdj('pdj 0x20 @ rip')

    byte = None
    bit  = None
    set  = None
    rip  = None
    bp   = None

    for op in ops:
        # invalid data may come after valid data
        if op["type"] == "invalid":
            print(" DONE!")
            return (None, None)
        # byte from input string, that is checked in this block
        if op["opcode"].startswith("mov al, byte"):
            param = op['opcode'][13:].replace("[rax", "").replace("]", "")
            if param == "":
                param = "0"
            elif param.startswith(" + "):
                param = param[3:]
            param = int(param, 0)
            byte = param
        # bit in byte, that is checked in this block
        elif op["opcode"].startswith("and al, "):
            param = op['opcode'][8:]
            param = int(param, 0)
            if bin(param).count("1") != 1:
                print("\nError: Bitmask checks for more than one bit:", param)
                return (None, None)
            bit = param
        # je => bit should be set
        elif op["opcode"].startswith("je "):
            set = 1
            rip = op["offset"] + op["size"]
        # je => bit should not be set
        elif op["opcode"].startswith("jne "):
            set = 0
            rip = op["offset"] + op["size"]
        # jump to next block
        elif op["opcode"].startswith("jmp "):
            bp = op["offset"]
            break

    # check, that we analysed block successfully
    if set is None or byte is None or bit is None or bp is None:
        print("\nError: Could not extract more key information")
        return (None, None)

    # append key information
    if not byte in keydata:
        keydata[byte] = {}
    keydata[byte][bit] = set

    # return information to find next block
    return (rip, bp)



# start zwiebel process in the background with ptrace support
build_ptrace_lib()
penv = os.environ.copy()
penv["LD_PRELOAD"] = "./ptrace.so"
p = subprocess.Popen(["./zwiebel"], env=penv, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
clean_ptrace_lib()

# attach to zwiebel process
r2 = r2pipe.open("%d" % p.pid, flags = ["-d"])

# extract_block_info() expects intel syntax
r2.cmd("e asm.pseudo = false")
r2.cmd("e asm.syntax = intel")

# avoid printing > 1600 hitinfo lines
r2.cmd("e cmd.hitinfo = false")

# wait for shc to be called
r2.cmd("db obj.shc")
p.stdin.write("\n");
r2.cmd("dc")

layer=0
while True:
    sys.stdout.write("\ranalysing layer: {}...".format(layer))
    rip, breakpoint = extract_block_info()
    if rip is None or breakpoint is None:
        break
    # skip behind input string check
    r2.cmd("dr rip = {}".format(hex(rip)))
    # continue to next block
    r2.cmd("db {}".format(hex(breakpoint)))
    r2.cmd("dc")
    r2.cmd("db -{}".format(hex(breakpoint)))
    r2.cmd("ds")
    layer+=1

print("\nFlag:", keydata2string())

p.kill()
