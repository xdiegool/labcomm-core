package se.lth.control.labcomm2006;

public interface LabCommDispatcher {
    
  public Class getSampleClass();
    
  public String getName();

  public byte[] getSignature();

  public void decodeAndHandle(LabCommDecoder decoder,
			      LabCommHandler handler) throws Exception;

}

