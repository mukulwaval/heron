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

#### Setup for windows:

run `scripts/setupz-win.bat`
