namespace se.lth.control.labcomm2006 {

  using System;

  public interface SampleDispatcher {
    
    Type getSampleClass();
    
    String getName();
    
    byte[] getSignature();
    
    void decodeAndHandle(Decoder decoder,
			 SampleHandler handler);

  }

}
