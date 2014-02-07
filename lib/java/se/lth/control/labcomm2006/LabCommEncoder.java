package se.lth.control.labcomm2006;

import java.io.IOException;

public interface LabCommEncoder {

  public void register(LabCommDispatcher dispatcher) throws IOException;
  public void begin(Class<? extends LabCommSample> c) throws IOException;
  public void end(Class<? extends LabCommSample> c) throws IOException;
  public void encodeBoolean(boolean value) throws IOException;
  public void encodeByte(byte value) throws IOException;
  public void encodeShort(short value) throws IOException;
  public void encodeInt(int value) throws IOException;
  public void encodeLong(long value) throws IOException;
  public void encodeFloat(float value) throws IOException;
  public void encodeDouble(double value) throws IOException;
  public void encodeString(String value) throws IOException;
  public void encodePacked32(long value) throws IOException;

}
