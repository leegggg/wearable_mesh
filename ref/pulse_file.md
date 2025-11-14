#

```
Dungeonlab+pulse:52,16,0,2,1/100-1,100-0,100-0,100-0,100-0,100-0,100-0,100-0,100-0,100-0,100-1+section+0,20,0,1,1/100-1,100-0,100-0,100-0,100-0,100-0,100-0,100-0,100-0,100-1
```

# Pulse protocol format

The pulse protocol format is a text-based format used to define the structure and behavior of a wave in the dglab esimu device. The file consists of several sections, each separated by a `+section+` string. Each section contains information about the dungeon's wave.

Protocol starts with `Dungeonlab+pulse:` followed by the parameters for the wave. The parameters are separated by commas and consist of the following elements:

- minFreq
- maxFreq
- durationIndex
- mode
- isOn