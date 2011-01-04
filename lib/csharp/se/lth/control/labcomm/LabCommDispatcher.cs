namespace se.lth.control.labcomm {

  using System;

  public interface LabCommDispatcher {
    
    Type getSampleClass();
    
    String getName();
    
    byte[] getSignature();
    
    void decodeAndHandle(LabCommDecoder decoder,
			 LabCommHandler handler);

  }

}
