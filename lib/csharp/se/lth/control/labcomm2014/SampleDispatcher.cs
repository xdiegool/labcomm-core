namespace se.lth.control.labcomm2014 {

  using System;

  public interface SampleDispatcher {
    
    SampleDispatcher getSampleIdentity();
    
    String getName();
    
    byte[] getSignature();

    void decodeAndHandle(Decoder decoder,
			 SampleHandler handler);

  }

}
