package se.lth.control.labcomm2014;

import java.io.IOException;

public class WrappingDecoder implements Decoder {

  private Decoder decoder;

  public WrappingDecoder(Decoder decoder) {
    this.decoder = decoder;
  }

  public void runOne() throws Exception {
    decoder.runOne();
  }
  
  public void run() throws Exception {
    decoder.run();
  }    
  
  public void register(SampleDispatcher dispatcher, 
                       SampleHandler handler) throws IOException{
    decoder.register(dispatcher, handler);
  }
  
  public void registerSampleRef(SampleDispatcher dispatcher) throws IOException {
    decoder.registerSampleRef(dispatcher);
  }
  
  public boolean decodeBoolean() throws IOException {
    return decoder.decodeBoolean();
  }
  
  public byte decodeByte() throws IOException {
    return decoder.decodeByte();
  }
  
  public short decodeShort() throws IOException {
    return decoder.decodeShort();
  }
  
  public int decodeInt() throws IOException {
    return decoder.decodeInt();
  }
  
  public long decodeLong() throws IOException {
    return decoder.decodeLong();
  }
  
  public float decodeFloat() throws IOException {
    return decoder.decodeFloat();
  }
  
  public double decodeDouble() throws IOException {
    return decoder.decodeDouble();
  }
  
  public String decodeString() throws IOException {
    return decoder.decodeString();
  }
  
  public int decodePacked32() throws IOException {
    return decoder.decodePacked32();
  }
  
  public SampleDispatcher decodeSampleRef() throws IOException {
    return decoder.decodeSampleRef();
  }
  
}
