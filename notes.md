#### Volume reduction element

- set caps on pad template -> set of all caps that you might provide -> restrict to Raw-audio Float32LE
- event function -> how are you sending the event from upstream to downstream
- how do you do the data processing in the chain function?
- GST_DEBUG=6, to see events in a pipeline. `GST_DEBUG=2,mynewfilter:6` to see events pertaining only to `mynewfilter`
  - events -> signal some kind of action or availability, can happen at any point of time during the pipeline lifecycle

- size of one audio frame = No of bytes per sample * num of channels
    - example: S32LE, two channels
    - size of one audio frame = 4 bytes * 2 = 8 bytes

- `gst-launch-1.0 -v audiotestsrc ! mynewfilter ! wavenc ! filesink location = out1.wav`
