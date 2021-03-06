package se.lth.control.labcomm2014;

import java.io.IOException;

public interface Encoder {

  public void register(SampleDispatcher dispatcher) throws IOException;
  public void registerSampleRef(SampleDispatcher dispatcher) throws IOException;
  public void begin(Class<? extends SampleType> c) throws IOException;
  public void end(Class<? extends SampleType> c) throws IOException;

  public void begin(int t) throws IOException;
  public int getTypeId(Class<? extends SampleType> c) throws IOException;

  public void encodeBoolean(boolean value) throws IOException;
  public void encodeByte(byte value) throws IOException;
  public void encodeShort(short value) throws IOException;
  public void encodeInt(int value) throws IOException;
  public void encodeLong(long value) throws IOException;
  public void encodeFloat(float value) throws IOException;
  public void encodeDouble(double value) throws IOException;
  public void encodeString(String value) throws IOException;
  public void encodePacked32(long value) throws IOException;
  public void encodeSampleRef(Class value) throws IOException;

}
