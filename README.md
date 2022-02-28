# Minor Car Mistake Browser

MCM Browser is a MCM web browser. It uses QtWebEngine rendering engine.

## Downloads

MCM Browser downloads are available from [homepage](minorcarmistake.com/MCMBrowser).

## Building

```sh
mkdir build && cd build
cmake ..
make && make install
```

### Install to custom prefix

When installing Falkon to custom prefix, you may need to adjust `XDG_DATA_DIRS` environment variable.

```sh
# Build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/falkon

# Run
export XDG_DATA_DIRS="$HOME/falkon/share:$XDG_DATA_DIRS"
$HOME/falkon/bin/falkon
```
