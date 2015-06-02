package se.lth.control.labcomm2014;

import java.io.IOException;

public class WrappingEncoder implements Encoder {

  private Encoder encoder;
  
  public WrappingEncoder(Encoder encoder) throws IOException {
    this.encoder = encoder;
  }

  public void register(SampleDispatcher dispatcher) throws IOException {
    encoder.register(dispatcher);
  }
  
  public void registerSampleRef(SampleDispatcher dispatcher) throws IOException {
    encoder.registerSampleRef(dispatcher);
  }
  
  public void begin(SampleDispatcher dispatcher) throws IOException {
    encoder.begin(dispatcher);
  }
  
  public void end(SampleDispatcher dispatcher) throws IOException {
    encoder.end(dispatcher);
  }
  
  public void encodeBoolean(boolean value) throws IOException {
    encoder.encodeBoolean(value);
  }
  
  public void encodeByte(byte value) throws IOException {
    encoder.encodeByte(value);
  }
  
  public void encodeShort(short value) throws IOException {
    encoder.encodeShort(value);
  }
  
  public void encodeInt(int value) throws IOException {
    encoder.encodeInt(value);
  }
  
  public void encodeLong(long value) throws IOException {
    encoder.encodeLong(value);
  }
  
  public void encodeFloat(float value) throws IOException {
    encoder.encodeFloat(value);
  }
  
  public void encodeDouble(double value) throws IOException {
    encoder.encodeDouble(value);
  }
  
  public void encodeString(String value) throws IOException {
    encoder.encodeString(value);
  }
  
  public void encodePacked32(int value) throws IOException {
    encoder.encodePacked32(value);
  }
  
  public void encodeSampleRef(SampleDispatcher value) throws IOException {
    encoder.encodeSampleRef(value);
  }
  
}
