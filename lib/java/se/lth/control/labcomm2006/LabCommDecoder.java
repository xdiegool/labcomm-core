package se.lth.control.labcomm2006;

import java.io.IOException;

public interface LabCommDecoder {
  
  public void register(LabCommDispatcher dispatcher, 
		       LabCommHandler handler) throws IOException;
  public boolean decodeBoolean() throws IOException;
  public byte decodeByte() throws IOException;
  public short decodeShort() throws IOException;
  public int decodeInt() throws IOException;
  public long decodeLong() throws IOException;
  public float decodeFloat() throws IOException;
  public double decodeDouble() throws IOException;
  public String decodeString() throws IOException;
  public int decodePacked32() throws IOException;

}
