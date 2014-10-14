package se.lth.control.labcomm2006;

public interface SampleDispatcher {
    
  public Class getSampleClass();
    
  public String getName();

  public byte[] getSignature();

  public void decodeAndHandle(Decoder decoder,
			      SampleHandler handler) throws Exception;

}

