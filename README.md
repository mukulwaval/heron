# Heron

#### Setup for linux:

```bash
cd scripts
sed -i 's/\r$//' setup_linux.sh
chmod +x setup_linux.sh
./setup_linux.sh
cd ..

make config=release
# or
make config=debug
```

After build, cd into the target dir, then

```bash
cd HeronGui
patchelf --remove-rpath HeronGui
patchelf --set-rpath '$ORIGIN' HeronGui
```

#### Setup for windows:

run `scripts/setupz-win.bat`
