import subprocess
import os

# encrypt6 binary open ./keyfile.dat, which is in this directory
os.chdir("/krypton/krypton6")

# get encrypted string
f = open("krypton7")
ciphertext = f.read()
f.close()

# variable to hold out plaintext
known = ""

# search until we decoded complete ciphertext
while len(known) < len(ciphertext):
    # try to find which plaintext byte encodes correctly
    for b in range(ord('A'), ord('Z')):
        # put guessed plaintext byte into input file
        f = open("/tmp/cccol-ctf2/input", 'w')
        f.write(known + chr(b));
        f.close();

        # encrypt input file
        subprocess.call(["./encrypt6", "/tmp/cccol-ctf2/input", "/tmp/cccol-ctf2/output"]);

        # read encrypted output file 
        f = open("/tmp/cccol-ctf2/output", 'r')
        enc = f.read();
        f.close();

        # check if encrypted byte matches ciphertext
        if enc[len(known)] == ciphertext[len(known)]:
            known += chr(b)
            break

# delete any traces
os.remove("/tmp/cccol-ctf2/input")
os.remove("/tmp/cccol-ctf2/output")

# print the found plaintext
print known
