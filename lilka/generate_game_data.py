"""
PlatformIO pre-build script to generate static_game_data.h

This script:
1. Builds luac if not already built
2. Runs to_c.py to generate the static game data header
"""

import os
import subprocess
import sys
from pathlib import Path

Import("env")

# Get the project root (parent of lilka folder)
project_root = Path(env.subst("$PROJECT_DIR")).parent
generated_header = project_root / "src" / "generated" / "static_game_data.h"
luac_build_dir = project_root / "build_luac"
luac_binary = luac_build_dir / "local_lua" / "luac"

def build_luac():
    """Build luac using CMake if it doesn't exist."""
    if luac_binary.exists():
        print(f"[generate_game_data] luac already exists at {luac_binary}")
        return True
    
    print("[generate_game_data] Building luac...")
    
    # Create build directory
    luac_build_dir.mkdir(parents=True, exist_ok=True)
    
    # Run CMake configure
    cmake_cmd = ["cmake", "-DBACKEND=PC", str(project_root)]
    print(f"[generate_game_data] Running: {' '.join(cmake_cmd)}")
    result = subprocess.run(cmake_cmd, cwd=luac_build_dir)
    if result.returncode != 0:
        print(f"[generate_game_data] CMake configure failed")
        return False
    
    # Build only luac target
    make_cmd = ["make", "luac", "-j4"]
    print(f"[generate_game_data] Running: {' '.join(make_cmd)}")
    result = subprocess.run(make_cmd, cwd=luac_build_dir)
    if result.returncode != 0:
        print(f"[generate_game_data] Make failed")
        return False
    
    print(f"[generate_game_data] luac built successfully at {luac_binary}")
    return True

def generate_game_data():
    """Generate static_game_data.h using to_c.py."""
    
    # Check if header needs regeneration
    carts_dir = project_root / "carts"
    stdlib_file = project_root / "stdlib" / "stdlib.lua"
    font_file = project_root / "artifacts" / "font.lua"
    hud_file = project_root / "artifacts" / "hud.p8"
    to_c_script = project_root / "scripts" / "to_c.py"
    
    # Get modification times
    source_files = list(carts_dir.glob("*.p8")) + [stdlib_file, font_file, hud_file, to_c_script]
    
    if generated_header.exists():
        header_mtime = generated_header.stat().st_mtime
        needs_regen = any(f.stat().st_mtime > header_mtime for f in source_files if f.exists())
        if not needs_regen:
            print(f"[generate_game_data] static_game_data.h is up to date")
            return True
    
    print("[generate_game_data] Generating static_game_data.h...")
    
    # Ensure generated directory exists
    generated_header.parent.mkdir(parents=True, exist_ok=True)
    
    # Run to_c.py
    cmd = [
        sys.executable,
        str(to_c_script),
        "--emit-stdlib",
        "--luac", str(luac_binary),
        str(carts_dir)
    ]
    
    print(f"[generate_game_data] Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=project_root, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"[generate_game_data] to_c.py failed: {result.stderr}")
        return False
    
    # Write the output to the header file
    with open(generated_header, 'w') as f:
        f.write(result.stdout)
    
    print(f"[generate_game_data] Generated {generated_header}")
    return True

# Run immediately when script is loaded (pre-build phase)
print("[generate_game_data] Pre-build script starting...")

if not build_luac():
    print("[generate_game_data] ERROR: Failed to build luac")
    env.Exit(1)

if not generate_game_data():
    print("[generate_game_data] ERROR: Failed to generate static_game_data.h")
    env.Exit(1)

print("[generate_game_data] Pre-build complete")
