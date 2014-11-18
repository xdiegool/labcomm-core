namespace se.lth.control.labcomm {

  using System;

  public interface Encoder {
    
    void register(SampleDispatcher dispatcher);
    void registerSampleRef(SampleDispatcher dispatcher);
    void begin(Type c);
    void end(Type c);

    void encodeBoolean(bool value);
    void encodeByte(byte value);
    void encodeShort(short value);
    void encodeInt(int value);
    void encodeLong(long value);
    void encodeFloat(float value);
    void encodeDouble(double value);
    void encodeString(String value);
    void encodePacked32(Int64 value);
    void encodeSampleRef(Type value);
    
  }

}
