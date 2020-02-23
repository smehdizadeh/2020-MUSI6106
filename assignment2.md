## sine wave ramp sample
440 Hz, 1 second long, Fs = 44100 Hz

### processed in MATLAB
with modulation width = 0.001s and modulation freq = 5 Hz
![MATLAB Vib_ramp](/ass2_plots/ramp_5hz_001s_MATLAB.png)

### processed in assignment 2 Vibrato implementation
with modulation width = 0.001s and modulation freq = 5 Hz
![CPP Vib_ramp](/ass2_plots/ramp_5hz_001s_CPP.png)

### difference between MATLAB and C++ outputs
computed by loading both outputs in MATLAB
First half only:
![computed diff ramp](/ass2_plots/ramp_5hz_001s_compDiff.png)

Whole sample:
![computed diff whole](/ass2_plots/ramp_5hz_001s_compDiffWhole.png)
