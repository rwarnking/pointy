# Instances


These folders contain all instances used for testing.
+ **generated** contains generated instances
+ **hand_made** contains instances created by hand
+ **real_world** contains instances from the real world

### Generated Instances

The folders *geometry* and *wolff* contain generated instances supplied from other groups.

The file names in the folder *random_generator* follow the structure given below:

```
<distribution>_<min value>_<max value>_<number of points>.txt
```

1. **Distribution** can either be uniform or pseudo random.
2. **Min Value** can be any integer.
3. **Max value** can be any integer bigger than min value.
4. **Number of Points** can be any positive integer.

### Hand-Made Instances

The file names are an attempt at describing the geometric relations between the given points.
The number at the end of the file indicates the number of points of this instance.

### Real-World Instances

The folders *wolff_real*, *world* and *world_projected* contain (partially equal) real world instances supplied from other groups.

The file names in folder *wolff_variants* describe what the containing data is based on.
The last number in the file name is the number of points of that instance.
If the filename begins with *deutsche_staedte* or *deutschland_bahnhoefe*, the two succeeding numbers are the length and height of all 
boxes of the instance, since no edge lengths were naturally given.
