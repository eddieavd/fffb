# fffb  

### macOS fake force feedback plugin

## what?

### flt

`flt` is a command line utility which communicates with logitech steering wheels via the [classic ffb protocol](https://opensource.logitech.com/wiki/force_feedback/Logitech_Force_Feedback_Protocol_V1.6.pdf) and can be used to configure the steering wheel directly  

### fffb
`fffb` is a plugin for [scs games](https://www.scssoft.com/) which uses the [TelemetrySDK](https://modding.scssoft.com/wiki/Documentation/Engine/SDK/Telemetry) to read truck telemetry data, constructs custom forces and plays them on your logitech G923  

## why?

- logitech GHUB is a broken piece of `    `
- force feedback doesn't work on macOS
- i like how the wheel shakes when i press the gas pedal

## how?

the scs related stuff is ripped from the examples bundled with the TelemetrySDK.  
to interact with the wheel, i use `flt`, a small library i wrote for interacting with the G923 from macOS. it allows us to enable/disable and configure autocentering, play custom forces and have fun with the leds.  

## usage

### prebuilt binaries

binaries are available on the [releases page](https://github.com/eddieavd/fffb/releases)  
simply copy `libfffb.dylib` to the plugin directory  
(plugin directory should be next to the game's executable, default for ats would be `~/Library/Application\ Support/Steam/steamapps/common/American\ Truck\ Simulator/American\ Truck\ Simulator.app/Contents/MacOS/plugins`)

### building from source

to build `flt` and `fffb`:  

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
upon launch, you'll see the advanced sdk features popup, hit OK and the plugin initialization starts.  
if the wheel leds start flashing and the wheel turns to the right and back, wheel initialization was successful and you should be good to go!  
if you don't see the leds flash, reload the plugin by running `sdk reinit` in the in-game console  

## disclaimer

should work on any apple silicon mac  
since scs requires the binaries to be x86_64, it might work out of the box for x86_64 macs (untested as of now)  
works only with Logitech G923 PS  
the ffb protocol is similar on other logitech wheels so it should be easy to extend support, but i don't have any other wheels and don't feel like writing code i can't test  
