# fffb  

### macOS fake force feedback plugin

## what?

### fffb
`fffb` is a plugin for [scs games](https://www.scssoft.com/) which uses the [TelemetrySDK](https://modding.scssoft.com/wiki/Documentation/Engine/SDK/Telemetry) to read truck telemetry data, constructs custom forces and plays them on your logitech wheel  

## why?

- logitech GHUB is a broken piece of `    `
- force feedback doesn't work on macOS
- i like how the wheel shakes when i press the gas pedal

## how?

the scs related stuff is ripped from the examples bundled with the TelemetrySDK.  
communication with the wheel is facilitated by apple's hid device API and uses logitech's classic ffb protocol.  

## usage

### prebuilt binaries

binaries are available on the [releases page](https://github.com/eddieavd/fffb/releases)  
simply copy `libfffb.dylib` to the plugin directory  
(plugin directory should be next to the game's executable, default for ats would be `~/Library/Application\ Support/Steam/steamapps/common/American\ Truck\ Simulator/American\ Truck\ Simulator.app/Contents/MacOS/plugins`)

### building from source

to build `fffb`:  

```bash
# clone the repo
git clone https://github.com/eddieavd/fffb && cd fffb

# create build directory
mkdir build && cd build

# configure and build project
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8

# create plugin directory
## should be in same directory as ets2/ats executable
## this should be the default path
mkdir ~/Library/Application\ Support/Steam/steamapps/common/American\ Truck\ Simulator/American\ Truck\ Simulator.app/Contents/MacOS/plugins

# copy plugin to plugin directory
cp libfffb.dylib ~/Library/Application\ Support/Steam/steamapps/common/American\ Truck\ Simulator/American\ Truck\ Simulator.app/Contents/MacOS/plugins
```

now you can launch ets2/ats.  
upon launch, the wheel should do a calibration run and you'll see the advanced sdk features popup, hit OK.  
if the wheel starts turning (similarly to the way it turns when plugged in), wheel initialization was successful and you should be good to go!  
if the wheel doesn't turn, you can try reloading the pluggin by running `sdk reinit` in the in-game console.  
in case this also doesn't help, feel free to raise an issue and include your `fffb` log file located at `/tmp/fffb.log`  

## disclaimer

should work on any apple silicon mac  
since scs requires the binaries to be x86_64, it might work out of the box for older x86_64 macs (untested as of now)  
works only with Logitech wheels  
i'm just a random dude writing code cause he's pissed at logitech, use at your own risk
