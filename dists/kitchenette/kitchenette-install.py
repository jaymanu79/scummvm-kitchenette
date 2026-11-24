#!/usr/bin/env python3
"""Prepare a Kitchenette game folder for ScummVM without running the Windows installer.

The projector, the Xtras and a few casts are only stored inside the Wise
installer (Installation.EXE). Wise keeps each file as a raw deflate stream,
so we scan the installer for deflate streams and keep the ones whose MD5
matches a known file of the French CD.

Usage: kitchenette-install.py CD_DIR DEST_DIR
  CD_DIR   contents of the CD (mounted, or extracted with `7z x Kitchenette.iso`)
  DEST_DIR folder to create; it receives the CD contents plus the installed files
"""

import hashlib
import os
import shutil
import sys
import zlib

KNOWN_FILES = {
    "7d66b27e86feb4819bdb7e950cfdf991": "Kitchenette.exe",
    "839ff66b222152056a4f184e3649ac04": "Lingo.ini",
    "51a2a4139c4a372424597f48a8b11c5e": "jojo.cct",
    "a527232e338fc7f2a180615d220be765": "Faites vos questions.exe",
    "9d1f46720ed0fe7c6bc6a66da4706b86": "Themes/Intit.cst",
    "678a0b11488ffc47d54232279b120497": "Themes/listQ.cst",
    "6511a67aefaee11ee351bc36e4541028": "Themes/themes.cst",
    "c3b886e9132a35be3a27673758ec60a5": "Jojo/ph.cst",
    "afed1b60f430728f8e0cce7b2c1ffed1": "Xtras/Cpuxtra.x32",
    "97b3565915543cc0aaaef152a8ee792c": "Xtras/Fileio.x32",
    "3206501beccb298f2e7f7d6f2aa40ad3": "Xtras/Mix32.x32",
    "97b8ac9939254df41a78821b9346c3a0": "Xtras/Sound Import Export.x32",
    "90766e3179dfdf6f8a6b9e807877ef6a": "Xtras/Swadcmpr.x32",
    "fff5043703a60fc9da3306a5e43470c6": "Xtras/mssx32.x32",
}


def find_ci(directory, name):
    """Find a file in a directory, ignoring case (CD file systems vary)."""
    for entry in os.listdir(directory):
        if entry.lower() == name.lower():
            return os.path.join(directory, entry)
    return None


def carve_deflate_streams(data):
    """Yield every complete raw deflate stream found in data."""
    pos = 0
    end = len(data) - 2
    while pos < end:
        decomp = zlib.decompressobj(-15)
        try:
            out = decomp.decompress(data[pos:pos + 16 * 1024 * 1024])
        except zlib.error:
            pos += 1
            continue
        if decomp.eof and len(out) >= 64:
            yield out
            pos += len(data[pos:pos + 16 * 1024 * 1024]) - len(decomp.unused_data)
        else:
            pos += 1


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        return 1
    cd_dir, dest = sys.argv[1], sys.argv[2]

    installer = find_ci(cd_dir, "Installation.EXE")
    if not installer:
        print("Installation.EXE not found in", cd_dir)
        return 1

    print("Copying the CD contents to", dest)
    shutil.copytree(cd_dir, dest, dirs_exist_ok=True)

    print("Scanning", installer, "(takes a few minutes)")
    with open(installer, "rb") as f:
        data = f.read()

    found = set()
    for stream in carve_deflate_streams(data):
        name = KNOWN_FILES.get(hashlib.md5(stream).hexdigest())
        if name and name not in found:
            path = os.path.join(dest, name)
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, "wb") as f:
                f.write(stream)
            found.add(name)
            print("  ", name)
            if len(found) == len(KNOWN_FILES):
                break

    missing = sorted(set(KNOWN_FILES.values()) - found)
    if missing:
        print("Missing (different CD version?):", ", ".join(missing))
        return 1
    print("Done. Add", dest, "in ScummVM.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
