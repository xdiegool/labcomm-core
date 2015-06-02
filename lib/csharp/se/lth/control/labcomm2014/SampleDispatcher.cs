namespace se.lth.control.labcomm2014 {

  using System;

  public interface SampleDispatcher {
    
    String getName();
    
    byte[] getSignature();

    void decodeAndHandle(Decoder decoder,
			 SampleHandler handler);

  }

}
