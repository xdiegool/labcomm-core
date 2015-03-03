using System;

namespace se.lth.control.labcomm2006 {

  public interface Decoder {

    void register(SampleDispatcher dispatcher, 
		  SampleHandler handler);

    bool decodeBoolean();
    byte decodeByte();
    short decodeShort();
    int decodeInt();
    long decodeLong();
    float decodeFloat();
    double decodeDouble();
    String decodeString();

  }

} 
