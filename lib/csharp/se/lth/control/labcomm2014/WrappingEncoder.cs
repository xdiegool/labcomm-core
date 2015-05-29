namespace se.lth.control.labcomm2014 {

  using System;

  public class WrappingEncoder : Encoder {
    
    private Encoder encoder;

    public WrappingEncoder(Encoder encoder) {
      this.encoder = encoder;
    }

    public virtual void register(SampleDispatcher dispatcher) {
      encoder.register(dispatcher);
    }

    public virtual void registerSampleRef(SampleDispatcher dispatcher) {
      encoder.registerSampleRef(dispatcher);
    }
    
    public virtual void begin(SampleDispatcher dispatcher) {
      encoder.begin(dispatcher);
    }
    
    public virtual void end(SampleDispatcher dispatcher) {
      encoder.end(dispatcher);
    }

    public virtual void encodeBoolean(bool value) {
      encoder.encodeBoolean(value);
    }
    
    public virtual void encodeByte(byte value) {
      encoder.encodeByte(value);
    }
    
    public virtual void encodeShort(short value) {
      encoder.encodeShort(value);
    }
    
    public virtual void encodeInt(int value) {
      encoder.encodeInt(value);
    }
    
    public virtual void encodeLong(long value) {
      encoder.encodeLong(value);
    }
    
    public virtual void encodeFloat(float value) {
      encoder.encodeFloat(value);
    }
    
    public virtual void encodeDouble(double value) {
      encoder.encodeDouble(value);
    }
    
    public virtual void encodeString(String value) {
      encoder.encodeString(value);
    }
    
    public virtual void encodePacked32(Int64 value) {
      encoder.encodePacked32(value);
    }
    
    public virtual void encodeSampleRef(SampleDispatcher value) {
      encoder.encodeSampleRef(value);
    }
    
  }

}
