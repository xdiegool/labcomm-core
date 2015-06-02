package se.lth.control.labcomm2014;

import java.io.IOException;

public interface SampleDispatcher <T extends SampleType>{
    
  public String getName();

  public byte[] getSignature();

  public void decodeAndHandle(Decoder decoder,
			      SampleHandler handler) throws Exception;

}

