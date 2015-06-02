package se.lth.control.labcomm2014;

import java.io.IOException;

public interface Decoder {
  
  public void runOne() throws Exception;
  public void run() throws Exception;
  public void register(SampleDispatcher dispatcher, 
		       SampleHandler handler) throws IOException;
  public void registerSampleRef(SampleDispatcher dispatcher) throws IOException;

  public boolean decodeBoolean() throws IOException;
  public byte decodeByte() throws IOException;
  public short decodeShort() throws IOException;
  public int decodeInt() throws IOException;
  public long decodeLong() throws IOException;
  public float decodeFloat() throws IOException;
  public double decodeDouble() throws IOException;
  public String decodeString() throws IOException;
  public int decodePacked32() throws IOException;
  public SampleDispatcher decodeSampleRef() throws IOException;

}
